/**
 * @author : xiaozhuai
 * @date   : 17/3/18
 */

#ifndef EFSERV_SERVENV_H
#define EFSERV_SERVENV_H

#define SERV_ENV                    (ServEnv::instance)

#define KEY_LISTEN                  "listen"
#define KEY_PORT                    "port"
#define KEY_DIR_INDEXS              "dir_indexs"
#define KEY_DIR_INDEXS_TPL          "dir_indexs_tpl"
#define KEY_ERR_TPL                 "err_tpl"
#define KEY_ACCESS_RULE             "access_rule"

#define DEFAULT_LISTEN              "0.0.0.0"
#define DEFAULT_PORT                "80"
#define DEFAULT_DIR_INDEXS          "1"
#define DEFAULT_DIR_INDEXS_TPL      "/usr/local/efserv/tpl/dir_indexs.html"
#define DEFAULT_ERR_TPL             "/usr/local/efserv/tpl/err.html"
#define DEFAULT_ACCESS_RULE         ".efserv_access"

#define DEFAULT_WEB_ROOT            "/var/www"
#define DEFAULT_INI_FILE            ".efserv_config"


#define MAX_KEEP_ALIVE_TIME         5

#include "Ini.h"
#include "trim.h"
#include "FileHandler.h"
#include "log.h"

class ServEnv {
    public:
        static ServEnv instance;
        ServEnv();
        ~ServEnv();

    public:
        void setWebRoot(string path);
        string getWebRoot();
        string getAbsoluteWebRoot();

    private:
        string webRoot;
        string absoluteWebRoot;
        bool customWebRoot;

    public:
        void parseConfig(string path);
        string getConfig(string key, string defaultValue);
        bool customConfig(string key);

    public:
        void dumpWebRoot();
        void dumpConfigs();

    private:
        Ini* ini;


};


#endif //EFSERV_SERVENV_H
