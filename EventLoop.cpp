/**
 * @author : xiaozhuai
 * @date   : 17/3/19
 */

#include "EventLoop.h"

ClientInfo* EventLoop::client_list[MAX_ALLOWED_CLIENT];
struct ev_loop* EventLoop::main_loop;
ev_io EventLoop::ev_io_accept;
ev_idle EventLoop::repeat_watcher;
ev_async EventLoop::ready_watcher;
int EventLoop::serv_sock;
http_parser_settings EventLoop::settings;
string EventLoop::listen_addr;
int EventLoop::port;

int EventLoop::on_url(http_parser *parser, const char *at, size_t length) {
    int fd = ((ClientInfo*)parser->data)->fd;
    string url(at, length);
    ClientInfo* client = client_list[fd];


    client->url = url;

    client->urlEndWithSlash = (url[url.length()-1] == '/');

    client->parser = parser;

    string filePath = SERV_ENV.getAbsoluteWebRoot()+url;
    client->file = new FileHandler(filePath);
    LOGI("GET %s [%d]", url, fd);
    return 0;
}

int EventLoop::on_message_complete(http_parser *parser) {
    int fd = ((ClientInfo*)parser->data)->fd;
    ClientInfo* client = client_list[fd];
    LOGD("on_message_complete [%d]", fd);
    respond_to_client(client);
    return 0;
}

void EventLoop::close_client(int fd) {
    ClientInfo *client = client_list[fd];
    if(client != NULL){
        if(client->io!=NULL){
            ev_io_stop(main_loop, client->io);
        }
        delete(client);
        client_list[fd] = NULL;
    }
}

void EventLoop::accept_handler(struct ev_loop *loop, ev_io *ev_io_accept, int e) {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_sock;
    ev_io *ev_io_client = (struct ev_io*) malloc(sizeof(struct ev_io));
    if(ev_io_client == NULL) {
        LOGW("malloc error in accept_cb");
        return ;
    }
    if(EV_ERROR & e) {
        LOGW("error event in accept");
        return ;
    }
    client_sock = accept(ev_io_accept->fd, (struct sockaddr*)&client_addr, &client_len);
    if(client_sock < 0){
        LOGW("accept error");
        return;
    }
    if(client_sock > MAX_ALLOWED_CLIENT) {
        LOGW("fd out of range [%d]", client_sock);
        close(client_sock);
        return;
    }
    if(client_list[client_sock] != NULL) {
        printf("client_sock not NULL, fd is [%d]\n", client_sock);
        return;
    }
    LOGD("client connected [%d]", client_sock);
    client_list[client_sock] = new ClientInfo(client_sock, ev_io_client);

    ev_io_init(ev_io_client, client_io_handler, client_sock, EV_READ /*| EV_WRITE*/); // no need for EV_WRITE
    ev_io_start(main_loop, ev_io_client);
}

void EventLoop::client_io_handler(struct ev_loop *loop, struct ev_io *ev_io_client, int e) {

    int fd = ev_io_client->fd;
    ClientInfo* client = client_list[fd];

    if(EV_ERROR & e){
        LOGW("error client io event [%d]", fd);
        close_client(fd);
        return;
    }

    if(EV_READ & e){
        LOGD("client io read event [%d]", fd);
        char buffer[READ_SOCKET_BUFFER_MAX_SIZE+1];     // last byte '\0'
        int n;
        if(ioctl(fd, FIONREAD, &n)){
            LOGW("ioctl socket FIONREAD err [%d]", fd);
            close_client(fd);
            return;
        }

        size_t readSize = (size_t)(n<READ_SOCKET_BUFFER_MAX_SIZE ? n : READ_SOCKET_BUFFER_MAX_SIZE);

        ssize_t recved = (size_t)recv(fd, buffer, readSize, 0);
        if(recved < 0) {
            LOGW("read client err [%d]", fd);
            return;
        }
        if(recved == 0) {
            LOGD("client disconnected [%d]", fd);
            close_client(fd);
            return;
        } else {
            buffer[recved] = '\0';
            //LOGI("receive client message [%d]: %s\n", fd, buffer);
            http_parser *parser = client->parser;
            if(parser==NULL){
                parser = (http_parser *)malloc(sizeof(http_parser));
                http_parser_init(parser, HTTP_REQUEST);
                parser->data = client;
            }
            size_t nparsed = http_parser_execute(parser, &settings, buffer, (size_t)recved);
            if (parser->upgrade) {
                LOGW("do not support protocol upgrade [%d]", fd);
                close_client(fd);
            } else if (nparsed != (size_t)recved) {
                LOGW("parse http error, msg: %s [%d]", http_errno_description(parser->http_errno), fd);
                close_client(fd);
            }
        }
        return;
    }
}

int EventLoop::file_open_done(eio_req *req) {

    ClientInfo* client = (ClientInfo*)req->data;

    if(req->result < 0){
        LOGW("err open file [%d]", client->fd);
        close_client(client->fd);
    }else{
        int file_fd = (int)req->result;
        off_t content_length = client->file->size();
        Response::respondHeader(client->fd, client->file->getMimeType(), content_length);
        client->file_fd = file_fd;
        eio_sendfile(client->fd, file_fd, 0, content_length, 0, file_send_done, client);
    }
    return 0;
}

int EventLoop::file_send_done(eio_req *req) {

    ClientInfo* client = (ClientInfo*)req->data;

    if(req->result < 0){
        LOGW("err open file [%d]", client->fd);
        close_client(client->fd);
    }else{
//        close(client->file_fd);
//        client->file_fd = -1;
        close_client(client->fd);
    }
    return 0;
}

void EventLoop::respond_to_client(ClientInfo *client) {
    int fd = client->fd;
    LOGD("client io write event [%d]", fd);

    if(!ACCESS_RULE.permissible(client->url)){             // check permission in .efserv_access
        Response::respondErr(fd, 403);
        goto __end;
    }

    if(!client->file->exist()){                            // 404
        Response::respondErr(fd, 404);
        goto __end;
    }

    if(outOfWebRoot(client)){
        Response::respondErr(fd, 403);
        goto __end;
    }

    if(client->file->isFile()){                                                // is file

        if(client->urlEndWithSlash){                                           // if end with '/', it's considered to be a directory, but it's a file, so 404
            Response::respondErr(fd, 404);
            goto __end;
        }

        if(!eio_open(client->file->getAbsolutePath().c_str(), O_RDONLY, 0, 0, file_open_done, client)){
            LOGW("err eio open file");
            Response::respondErr(fd, 500);
            goto __end;
        }
        return;
    }else if(client->file->isDir()){                                             // is dir

        if(!client->urlEndWithSlash){                                            // if not end with '/', it's considered to be a file, but it's a dir, redirect to add a '/'
            Response::respondRedirection(fd, 301, client->url+"/");
            goto __end;
        }

        if(stoi(SERV_ENV.getConfig(KEY_DIR_INDEXS, DEFAULT_DIR_INDEXS))) {       // dir indexs enable
            vector<FileHandler> files = client->file->listDir();
            Response::respondIndexs(fd, files, client->url);
            goto __end;
        }else{                                                                   // of course 403
            Response::respondErr(fd, 403);
            goto __end;
        }
    }else{                                                                       // 500
        Response::respondErr(fd, 500);
        goto __end;
    }

    __end:
    close_client(fd);
    return;
}


void EventLoop::want_poll() {
    ev_async_send (main_loop, &ready_watcher);
}

void EventLoop::done_poll() {

}

void EventLoop::repeat (EV_P_ ev_idle *w, int revents) {
    if (eio_poll () != -1)
        ev_idle_stop (main_loop, w);
}

void EventLoop::ready (EV_P_ ev_async *w, int revents) {
    if (eio_poll () == -1)
        ev_idle_start (main_loop, &repeat_watcher);
}

void EventLoop::init() {
    main_loop = EV_DEFAULT;

    settings.on_url = on_url;
    settings.on_message_complete = on_message_complete;

    Response::loadTpl();
}

void EventLoop::start() {
    serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;

    listen_addr = SERV_ENV.getConfig(KEY_LISTEN, DEFAULT_LISTEN);
    port = stoi(SERV_ENV.getConfig(KEY_PORT, DEFAULT_PORT));

    serv_addr.sin_addr.s_addr = inet_addr(listen_addr.c_str());
    serv_addr.sin_port = htons(port);


    if( ::bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) != 0 ){
        LOGE("addr bind error");
        exit(1);
    }

    if( listen(serv_sock, SOMAXCONN) < 0 ) {
        LOGE("listen error");
        exit(1);
    }

    int bReuseaddr=1;
    if(setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&bReuseaddr, sizeof(bReuseaddr)) != 0) {
        printf("set sock option reuse addr error[%d]", serv_sock);
        exit(1);
    }

//    int bKeepAlive=1;
//    if(setsockopt(serv_sock, SOL_SOCKET, SO_KEEPALIVE, (const char*)&bKeepAlive, sizeof(bKeepAlive)) != 0){
//        printf("set sock option keep alive error[%d]", serv_sock);
//        exit(1);
//    }
//
//    int bKeepAliveTime = MAX_KEEP_ALIVE_TIME;
//#ifdef __APPLE__
//    if(setsockopt(serv_sock, IPPROTO_TCP, TCP_KEEPALIVE, &bKeepAliveTime, sizeof(bKeepAliveTime)) != 0){
//#else
//    if(setsockopt(serv_sock, IPPROTO_TCP, TCP_KEEPIDLE, &bKeepAliveTime, sizeof(bKeepAliveTime)) != 0){
//#endif
//        printf("set sock option keep alive time error[%d]", serv_sock);
//        exit(1);
//    }

    LOGI("Listen %s%s:%d%s ......",
         ANSI_COLOR_BLUE,
         SERV_ENV.getConfig(KEY_LISTEN, DEFAULT_LISTEN),
         stoi(SERV_ENV.getConfig(KEY_PORT, DEFAULT_PORT)),
         ANSI_COLOR_NORMAL
    );

    ev_idle_init (&repeat_watcher, repeat);
    ev_async_init (&ready_watcher, ready);
    ev_async_start (main_loop, &ready_watcher);
    eio_init (want_poll, done_poll);

    ev_io_init(&ev_io_accept, accept_handler, serv_sock, EV_READ);
    ev_io_start(main_loop, &ev_io_accept);

    ev_run(main_loop, 0);

}