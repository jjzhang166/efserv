/**
 * @author : xiaozhuai
 * @date   : 17/3/19
 */

#include "Response.h"

string Response::dir_indexs_html;
string Response::err_html;


void Response::loadTpl() {
    FileHandler dirIndexsTpl(SERV_ENV.getConfig(KEY_DIR_INDEXS_TPL, DEFAULT_DIR_INDEXS_TPL));
    FileHandler errTpl(SERV_ENV.getConfig(KEY_ERR_TPL, DEFAULT_ERR_TPL));
    if(dirIndexsTpl.exist()){
        dir_indexs_html = dirIndexsTpl.readAsText();
    }else{
        dir_indexs_html = "{{file_list}}";
    }
    if(errTpl.exist()){
        err_html = errTpl.readAsText();
    }else{
        err_html = "{{status_code}} {{msg}}";
    }
}

void Response::respondHeader(int fd, string mimetype, off_t content_length) {
    header(fd, "HTTP/1.1 200 OK");
    header(fd, "Content-Type", mimetype);
    header(fd, "Content-Length", to_string(content_length));
    header_end(fd);
}

void Response::respondContent(int fd, const char *content, size_t length) {
    send(fd, content, length, 0);
}

string assign(string format, string key, string value){
    key = "{{" + key + "}}";
    format = StringUtils::replaceAll(format, key, value);
    return format;
}

void Response::respondErr(int fd, int status_code) {
    string msg;
    switch (status_code){
        case 403:
            msg = "Forbidden";
            break;
        case 404:
            msg = "Not Found";
            break;
        default:
            status_code = 500;
        case 500:
            msg = "Internal Server Error";
            break;
    }

    header(fd, "HTTP/1.1 "+to_string(status_code)+" "+msg);


    string content;
    content = assign(err_html, "status_code", to_string(status_code));
    content = assign(content, "msg", msg);
    content = assign(content, "efserv_version", EFSERV_VERSION);
    size_t len = content.length();

    header(fd, "Content-Type", "text/html");
    header(fd, "Content-Length", to_string(len));
    header_end(fd);
    respondContent(fd, content.c_str(), len);
}

void Response::respondIndexs(int fd, vector<FileHandler> files, string url, bool outputJson) {

    header(fd, "HTTP/1.1 200 OK");
    if(outputJson){
        size_t fileCount = files.size();
        string fileListJson = "[\n";
        for(int i=0; i<fileCount; i++){

            FileHandler file = files[i];

            /**
             * ignore the url that are denied
             */
            string tmp = url;
            rtrim(tmp, '/');
            tmp += "/" + file.getName();
            if(!ACCESS_RULE.permissible(tmp)){
                continue;
            }

            string obj = tfm::format(
                    "    {\n"
                    "        \"is_file\":%s,\n"
                    "        \"is_link\":%s,\n"
                    "        \"name\":\"%s\",\n"
                    "        \"ext\":\"%s\",\n"
                    "        \"mime_type\":\"%s\",\n"
                    "        \"ctime\":%ld,\n"
                    "        \"mtime\":%ld,\n"
                    "        \"atime\":%ld\n"
                    "    }",
                    file.isFile() ? "true" : "false",
                    file.isLink() ? "true" : "false",
                    file.getName(),
                    file.getExt(),
                    file.getMimeType(),
                    file.getCreateTime(),
                    file.getModifyTime(),
                    file.getAccessTime()
            );
            if(i==fileCount-1)  obj += "\n";
            else                obj += ",\n";
            fileListJson += obj;
        }
        fileListJson += "]";
        string data = tfm::format(
                "{\n"
                "    \"path\":\"%s\",\n"
                "    \"list\":%s\n"
                "}",
                url,
                fileListJson
        );
        size_t len = data.length();
        header(fd, "Content-Type", "application/json");
        header(fd, "Content-Length", to_string(len));
        header_end(fd);
        respondContent(fd, data.c_str(), len);
    }else {
        string content = assign(dir_indexs_html, "url", url);
        content = assign(content, "efserv_version", EFSERV_VERSION);
        size_t len = content.length();
        header(fd, "Content-Type", "text/html");
        header(fd, "Content-Length", to_string(len));
        header_end(fd);
        respondContent(fd, content.c_str(), len);
    }
}

void Response::respondRedirection(int fd, int status_code, string location) {
    string msg;
    switch(status_code){
        case 300:
            msg = "Multiple Choices";
            break;
        case 301:
            msg = "Moved Permanently";
            break;
        default:
            status_code = 302;
        case 302:
            msg = "Found (Moved Temporarily)";
            break;
        case 303:
            msg = "See Other";
            break;
        case 304:
            msg = "Not Modified";
            break;
        case 305:
            msg = "Use Proxy";
            break;
        case 306:
            msg = "Switch Proxy (Unused)";
            break;
        case 307:
            msg = "Temporary Redirect";
            break;
    }
    header(fd, "HTTP/1.1 "+to_string(status_code)+" "+msg);
    header(fd, "Location", location);
    header_end(fd);
}