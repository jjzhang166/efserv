/**
 * @author : xiaozhuai
 * @date   : 17/3/18
 */

#ifndef EFSERV_INIPARSER_H
#define EFSERV_INIPARSER_H

#include <string>
#include <unordered_map>
#include <fstream>
#include <sstream>

using namespace std;

class Ini {
    public:
        static Ini *parse(string path);
        static Ini *parseString(string str);

    public:
        enum {
            NO_ERROR = 0,
            FILE_READ_ERROR
        };
        bool perror;

    public:
        string get(string key, string defaultValue);
        void set(string key, string value);
        bool exist(string key);


    private:
        unordered_map<string, string> map;


};


#endif //EFSERV_INIPARSER_H
