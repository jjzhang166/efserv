/**
 * @author : xiaozhuai
 * @date   : 17/3/18
 */

#include "ServEnv.h"

ServEnv ServEnv::instance;

string __dump_list__[] = {
        KEY_LISTEN,                     DEFAULT_LISTEN,
        KEY_PORT,                       DEFAULT_PORT,
        KEY_DIR_INDEXS,                 DEFAULT_DIR_INDEXS,
        KEY_DIR_INDEXS_TPL,             DEFAULT_DIR_INDEXS_TPL,
        KEY_ERR_TPL,                    DEFAULT_ERR_TPL,
        KEY_ACCESS_RULE,                DEFAULT_ACCESS_RULE,
        KEY_BUILT_IN,                   DEFAULT_BUILT_IN,
};

#define OVERRIDE_FLAG_STR(overridden)           ( overridden ? "\033[32m[ custom  ] \033[0m" : "\033[33m[ default ] \033[0m" )

ServEnv::ServEnv() : webRoot(DEFAULT_WEB_ROOT),
                     absoluteWebRoot(DEFAULT_WEB_ROOT),
                     customWebRoot(false),
                     ini(NULL) {


}

ServEnv::~ServEnv() {
    if (ini != NULL) delete ini;
}


void ServEnv::setWebRoot(string path) {
    webRoot = path;
    rtrim(webRoot, '/');
    FileHandler file(webRoot);
    absoluteWebRoot = file.getAbsolutePath();
    customWebRoot = true;
}

string ServEnv::getWebRoot() {
    return webRoot;
}

string ServEnv::getAbsoluteWebRoot() {
    return absoluteWebRoot;
}

void ServEnv::parseConfig(string path) {
    if (path == "") {
        path = webRoot + "/" + DEFAULT_INI_FILE;
        LOGI("No provided config file, try \"%s\"", path);
    }
    ini = Ini::parse(path);
    if (ini->perror == Ini::NO_ERROR) {
        LOGI("Parse config file \"%s\" %ssuc%s", path, ANSI_COLOR_GREEN, ANSI_COLOR_NORMAL);
    } else {
        LOGI(
                "Parse config file \"%s\" %sfailed%s, maybe file not exist, will use default values for all params",
                path, ANSI_COLOR_ORANGE, ANSI_COLOR_NORMAL
        );
    }
}

string ServEnv::getConfig(string key, string defaultValue) {
    return ini->get(key, defaultValue);
}

bool ServEnv::customConfig(string key) {
    return ini->exist(key);
}

void ServEnv::dumpWebRoot() {
    if (!customWebRoot) {
        LOGI("No provided web root, use %s/var/www%s", ANSI_COLOR_BLUE, ANSI_COLOR_NORMAL);
    } else {
        LOGI("Web root is %s%s%s, absolute path is %s%s%s",
             ANSI_COLOR_BLUE, webRoot, ANSI_COLOR_NORMAL,
             ANSI_COLOR_BLUE, absoluteWebRoot, ANSI_COLOR_NORMAL
        );
    }
}

void ServEnv::dumpConfigs() {
    for (int i = 0; i < sizeof(__dump_list__) / sizeof(string) / 2; ++i) {
        string key = __dump_list__[i * 2];
        string defaultValue = __dump_list__[i * 2 + 1];
        LOGI(
                "%s %s%s = %s%s",
                OVERRIDE_FLAG_STR(customConfig(key)),
                ANSI_COLOR_BLUE, key, getConfig(key, defaultValue), ANSI_COLOR_NORMAL
        );
    }
}