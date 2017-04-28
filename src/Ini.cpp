/**
 * @author : xiaozhuai
 * @date   : 17/3/18
 */

#include "Ini.h"

#define MAX_LINE_SIZE          10240                 // max chars per line
#define MAX_KV_SIZE            (MAX_LINE_SIZE / 2)

Ini *Ini::parse(string path) {
    char buf[MAX_LINE_SIZE];
    char tmpKey[MAX_KV_SIZE];
    char tmpValue[MAX_KV_SIZE];

    Ini *ini = new Ini();

    ifstream in(path);
    if (!in) { ini->perror = 1; return ini; }

    while (!in.eof()) {
        in.getline(buf, MAX_LINE_SIZE);
        if (sscanf(buf, "%s = %s", tmpKey, tmpValue) == 2)
            ini->set(tmpKey, tmpValue);
    }

    in.close();
    ini->perror = 0;
    return ini;
}

Ini *Ini::parseString(string str) {
    stringstream ss;
    ss << str;
    char buf[MAX_LINE_SIZE];
    char tmpKey[MAX_KV_SIZE];
    char tmpValue[MAX_KV_SIZE];

    Ini *ini = new Ini();

    while(!ss.eof()){
        ss.getline(buf, MAX_LINE_SIZE);
        if (sscanf(buf, "%s = %s", tmpKey, tmpValue) == 2)
            ini->set(tmpKey, tmpValue);
    }

    return ini;
}

void Ini::set(string key, string value) {
    map[key] = value;
}

string Ini::get(string key, string defaultValue) {
    if (exist(key))
        return map[key];
    else
        return defaultValue;
}

bool Ini::exist(string key) {
    return map.find(key) != map.end();
}