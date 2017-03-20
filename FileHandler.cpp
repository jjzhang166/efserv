/**
 * @author : xiaozhuai
 * @date   : 17/3/18
 */

#include "FileHandler.h"
#include "log.h"

FileHandler::FileHandler(string path) {
    m_path = path;
    rtrim(m_path, '/');
    int c = stat(m_path.c_str(), &m_fileStat);
    m_exist = ( c == 0 );
}

bool FileHandler::exist() {
    return m_exist;
}

bool FileHandler::isFile() {
    if(!m_exist) return false;
    return S_ISREG(m_fileStat.st_mode);
}

bool FileHandler::isDir() {
    if(!m_exist) return false;
    return S_ISDIR(m_fileStat.st_mode);
}

bool FileHandler::isLink() {
    if(!m_exist) return false;
    return S_ISLNK(m_fileStat.st_mode);
}

time_t FileHandler::getCreateTime() {
    if(!m_exist) return 0;
#ifdef __APPLE__
    return m_fileStat.st_ctimespec.tv_sec;
#else
    return m_fileStat.st_ctim.tv_sec;
#endif
}

time_t FileHandler::getModifyTime() {
    if(!m_exist) return 0;
#ifdef __APPLE__
    return m_fileStat.st_mtimespec.tv_sec;
#else
    return m_fileStat.st_mtim.tv_sec;
#endif
}

time_t FileHandler::getAccessTime() {
    if(!m_exist) return 0;
#ifdef __APPLE__
    return m_fileStat.st_atimespec.tv_sec;
#else
    return m_fileStat.st_mtim.tv_sec;
#endif
}

string FileHandler::getAbsolutePath() {
    char absolutePath[MAX_PATH_LEN];
    realpath(m_path.c_str(), absolutePath);
    return string(absolutePath);
}

string FileHandler::getName() {
    string absolutePath = getAbsolutePath();
    return absolutePath.erase(0, absolutePath.find_last_of('/')+1);
}

string FileHandler::getExt() {
    string absolutePath = getAbsolutePath();
    return absolutePath.erase(0, absolutePath.find_last_of('.')+1);
}

string FileHandler::getMimeType() {

    return isFile() ? MIME_TYPE.fromExtension(getExt()) : "";
}

off_t FileHandler::size() {
    if(!m_exist) return 0;
    return m_fileStat.st_size;
}

string FileHandler::readAsText() {
    char buffer[4096];
    ifstream in(m_path, ios_base::in);
    if(!in) return "";
    string content = "";
    string line;
    while(!in.eof()){
        getline(in, line);
        content += line;
        content += '\n';
    }
    content.erase(content.end()-1);
    return content;
}

vector<FileHandler> FileHandler::listDir() {
    string absolutePath = getAbsolutePath();

    DIR *ptr_dir;
    struct dirent *dir_entry;
    ptr_dir = opendir(absolutePath.c_str());

    vector<FileHandler> files;

    while((dir_entry = readdir(ptr_dir)) != NULL){
        /**
         * ignore "." & ".."
         */
        if(strcmp(dir_entry->d_name,".") == 0 || strcmp(dir_entry->d_name,"..") == 0 )
            continue;

        files.push_back(FileHandler(absolutePath+"/"+dir_entry->d_name));
    }
    return files;
}