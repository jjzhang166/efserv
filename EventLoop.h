/**
 * @author : xiaozhuai
 * @date   : 17/3/19
 */

#ifndef EFSERV_EVENTLOOP_H
#define EFSERV_EVENTLOOP_H

#include <fstream>
#include <vector>
#include <ev.h>
#include <eio.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <http_parser.h>
#include <sys/ioctl.h>
#include <poll.h>
#include <fcntl.h>
#include "Response.h"
#include "FileHandler.h"
#include "ServEnv.h"
#include "AccessRule.h"

#define MAX_ALLOWED_CLIENT                  102400
#define READ_SOCKET_BUFFER_MAX_SIZE         4096

class ClientInfo{
    public:
        string url;
        string path;
        string query;
        bool urlEndWithSlash;
        FileHandler* file;
        http_parser *parser;
        int file_fd;
        ev_io* io;
        int fd;

        ClientInfo(int fd, ev_io* io) : file(NULL), parser(NULL), file_fd(-1), io(NULL)
                {
            this->fd = fd;
            this->io = io;
        }

        ~ClientInfo(){
            close(fd);
            if(io!=NULL) free(io);
            if(file_fd!=-1) close(file_fd);
            if(parser!=NULL) free(parser);
            if(file!=NULL) delete(file);
        }

};


class EventLoop {

    public:
        static void init();
        static void start();

    private:
        static ClientInfo *client_list[MAX_ALLOWED_CLIENT];
        static struct ev_loop *main_loop;
        static ev_io ev_io_accept;
        static ev_idle repeat_watcher;
        static ev_async ready_watcher;
        static int serv_sock;
        static http_parser_settings settings;

    private:
        static string listen_addr;
        static int port;

    private:
        static int on_url(http_parser *parser, const char *at, size_t length);
        static int on_message_complete(http_parser *parser);

    private:
        static void close_client(int fd);
        static void accept_handler(struct ev_loop *loop, ev_io *ev_io_accept, int e);
        static void client_io_handler(struct ev_loop *loop, struct ev_io *ev_io_client, int e);
        static void respond_to_client(ClientInfo *client);
        static void want_poll (void);
        static void done_poll (void);
        static void ready (EV_P_ ev_async *w, int revents);
        static void repeat (EV_P_ ev_idle *w, int revents);
        static int file_open_done(eio_req *req);
        static int file_send_done(eio_req *req);

    private:
        static inline bool outOfWebRoot(ClientInfo* client){
            string webRoot = SERV_ENV.getAbsoluteWebRoot();
            string path = client->file->getAbsolutePath();
            string tmp = path.substr(0, webRoot.length());
            return webRoot != tmp;
        }

};


#endif //EFSERV_EVENTLOOP_H
