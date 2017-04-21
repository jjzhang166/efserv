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

#define toms(a)     ((a).tv_sec*1000 + (a).tv_nsec/1000000)

time_t FileHandler::getCreateTime() {
    if(!m_exist) return 0;
#ifdef __APPLE__
    return toms(m_fileStat.st_ctimespec);
#else
    return toms(m_fileStat.st_ctim);
#endif
}

time_t FileHandler::getModifyTime() {
    if(!m_exist) return 0;
#ifdef __APPLE__
    return toms(m_fileStat.st_mtimespec);
#else
    return toms(m_fileStat.st_mtim);
#endif
}

time_t FileHandler::getAccessTime() {
    if(!m_exist) return 0;
#ifdef __APPLE__
    return toms(m_fileStat.st_atimespec);
#else
    return toms(m_fileStat.st_mtim);
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
    string name = getName();
    size_t p = name.find_last_of('.');
    if(p==-1 || p==name.length()-1) return "";
    else{
        string ext = name.substr(p+1);
        StringUtils::toLower(ext);
        return ext;
    }
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