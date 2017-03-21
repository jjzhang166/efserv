/**
 * @author : xiaozhuai
 * @date   : 17/3/19
 */

#ifndef EFSERV_RESPONSE_H
#define EFSERV_RESPONSE_H

#include <string>
#include <sys/socket.h>
#include "ServEnv.h"
#include "FileHandler.h"
#include "version.h"
#include "StringUtils.h"
#include "AccessRule.h"

class Response {
    public:
        static void loadTpl();

    private:
        static string dir_indexs_html;
        static string err_html;

    public:
        static void respondErr(int fd, int status_code);
        static void respondIndexs(int fd, vector<FileHandler> files, string url);

        static void respondHeader(int fd, string mimetype, size_t content_length);
        static void respondContent(int fd, const char* content, size_t length);

        static void respondRedirection(int fd, int status_code, string location);

    private:
        static inline void header(int fd, string line){
            string tmp = line+"\r\n";
            send(fd, tmp.c_str(), tmp.length(), 0);
        }

        static inline void header(int fd, string key, string value){
            string tmp = key+": "+value+"\r\n";
            send(fd, tmp.c_str(), tmp.length(), 0);
        }

        static inline void header_end(int fd){
            send(fd, "\r\n", 2, 0);
        }


};


#endif //EFSERV_RESPONSE_H
