/**
 * @author : xiaozhuai
 * @date   : 17/3/19
 */

#ifndef EFSERV_EVENTLOOP_H
#define EFSERV_EVENTLOOP_H

#include <fstream>
#include <vector>
#include <ev.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <http_parser.h>
#include <sys/ioctl.h>
#include "Response.h"
#include "FileHandler.h"
#include "ServEnv.h"
#include "AccessRule.h"

#define MAX_ALLOWED_CLIENT                  102400
#define READ_SOCKET_BUFFER_MAX_SIZE         4096
#define READ_FILE_BUFFER_MAX_SIZE           4096

class ClientInfo{
    public:
        string url;
        bool urlEndWithSlash;
        FileHandler* file;
        http_parser *parser;
        FILE* file_fp;
        ev_io* io;
        int fd;
        bool read_complete;

        ClientInfo(int fd, ev_io* io) : file(NULL), parser(NULL), file_fp(NULL), io(NULL), read_complete(false)
                {
            this->fd = fd;
            this->io = io;
        }

        ~ClientInfo(){
            close(fd);
            if(io!=NULL) free(io);
            if(file_fp!=NULL) fclose(file_fp);
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

    private:
        static inline bool outOfWebRoot(ClientInfo* client){
            string webRoot = SERV_ENV.getAbsoluteWebRoot();
            string path = client->file->getAbsolutePath();
            return path.length()<webRoot.length();
        }

};


#endif //EFSERV_EVENTLOOP_H
