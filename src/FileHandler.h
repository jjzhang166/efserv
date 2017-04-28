/**
 * @author : xiaozhuai
 * @date   : 17/3/18
 */

#ifndef EFSERV_FILE_HANDLER_H
#define EFSERV_FILE_HANDLER_H

#include <string>
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fstream>
#include <vector>
#include "trim.h"
#include "MimeType.h"

#define MAX_PATH_LEN        4096

using namespace std;

class FileHandler {
    public:
        FileHandler(string path);


    public:
        bool exist();
        bool isFile();
        bool isDir();
        bool isLink();
        time_t getCreateTime();
        time_t getModifyTime();
        time_t getAccessTime();
        string getName();
        string getAbsolutePath();
        string getExt();
        string getMimeType();
        off_t size();
        string readAsText();
        vector<FileHandler> listDir();

    private:
        struct stat m_fileStat;
        bool m_exist;
        string m_path;

};


#endif //EFSERV_FILE_HANDLER_H
