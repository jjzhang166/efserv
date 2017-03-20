/**
 * @author : xiaozhuai
 * @date   : 17/3/19
 */

#include "EventLoop.h"

ClientInfo* EventLoop::client_list[MAX_ALLOWED_CLIENT];
struct ev_loop* EventLoop::main_loop;
ev_io EventLoop::ev_io_accept;
int EventLoop::serv_sock;
http_parser_settings EventLoop::settings;
string EventLoop::listen_addr;
int EventLoop::port;

int EventLoop::on_url(http_parser *parser, const char *at, size_t length) {
    int fd = ((ClientInfo*)parser->data)->fd;
    string url(at, length);
    ClientInfo* client = client_list[fd];


    client->url = url;
    client->parser = parser;

    string filePath = SERV_ENV.getAbsoluteWebRoot()+url;
    client->file = new FileHandler(filePath);
    LOGI("GET %s [%d]", url, fd);
    return 0;
}

int EventLoop::on_message_complete(http_parser *parser) {
    int fd = ((ClientInfo*)parser->data)->fd;
    ClientInfo* client = client_list[fd];
    client->read_complete = true;
    LOGD("on_message_complete [%d]", fd);

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

    ev_io_init(ev_io_client, client_io_handler, client_sock, EV_READ | EV_WRITE);
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
        char buffer[BUFFER_SIZE];
        ssize_t recved = recv(fd, buffer, BUFFER_SIZE, 0);
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

            http_parser *parser = (http_parser *)malloc(sizeof(http_parser));
            http_parser_init(parser, HTTP_REQUEST);

            parser->data = client;

            size_t nparsed = http_parser_execute(parser, &settings, buffer, recved);

            if (parser->upgrade) {
                LOGW("do not support protocol upgrade [%d]", fd);
                close_client(fd);
            } else if (nparsed != recved) {
                LOGW("parse http error, msg: %s [%d]", http_errno_description(parser->http_errno), fd);
                close_client(fd);
            }
        }
        return;
    }

    if((EV_WRITE & e) && client->read_complete){
        LOGD("client io write event [%d]", fd);
        ClientInfo* client = client_list[fd];

        if(!ACCESS_RULE.permissible(client->url)){             // check permission in .efserv_access
            Response::respondErr(fd, 403);
            goto end_write;
        }

        if(!client->file->exist()){                            // 404
            Response::respondErr(fd, 404);
            goto end_write;
        }

        if(outOfWebRoot(client)){
            Response::respondErr(fd, 403);
            goto end_write;
        }

        if(client->file->isFile()){                                                // is file
            FILE* file_fp = client->file_fp;
            if(file_fp == NULL){
                file_fp = fopen(client->file->getAbsolutePath().c_str(), "rb");
                if(file_fp==NULL){             // 404
                    Response::respondErr(fd, 404);
                    goto end_write;
                }
                client->file_fp = file_fp;
                off_t content_length = client->file->size();
                Response::respondHeader(fd, client->file->getMimeType(), content_length);
            }


            if(feof(file_fp)) goto end_write;

            char buffer[BUFFER_SIZE];
            ssize_t len = fread(buffer, 1, sizeof(buffer), file_fp);

            if(len==0) goto wait_next_write;

            Response::respondContent(fd, buffer, len);
            goto wait_next_write;
        }else if(stoi(SERV_ENV.getConfig(KEY_DIR_INDEXS, DEFAULT_DIR_INDEXS))){     // is dir and dir indexs enable
            vector<FileHandler> files = client->file->listDir();
            Response::respondIndexs(fd, files, client->url);
            goto end_write;
        }else{                                                              // of course 403
            Response::respondErr(fd, 403);
            goto end_write;
        }

        end_write:
        close_client(fd);

        wait_next_write:

        return;
    }




}



void EventLoop::init() {
    main_loop = ev_default_loop(0);

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
    int port = stoi(SERV_ENV.getConfig(KEY_PORT, DEFAULT_PORT));

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
        printf("set sock option error in reuse addr[%d]", serv_sock);
        exit(1);
    }

    LOGI("Listen %s%s:%d%s ......",
         ANSI_COLOR_BLUE,
         SERV_ENV.getConfig(KEY_LISTEN, DEFAULT_LISTEN),
         stoi(SERV_ENV.getConfig(KEY_PORT, DEFAULT_PORT)),
         ANSI_COLOR_NORMAL
    );

    ev_io_init(&ev_io_accept, accept_handler, serv_sock, EV_READ);
    ev_io_start(main_loop, &ev_io_accept);
    ev_run(main_loop, 0);

}