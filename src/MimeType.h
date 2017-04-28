/**
 * @author : xiaozhuai
 * @date   : 17/3/20
 */

#ifndef EFSERV_MIMETYPE_H
#define EFSERV_MIMETYPE_H

#include <unordered_map>
#include <string>
#include <vector>
#include "log.h"
#include "trim.h"
#include "StringUtils.h"

#define MIME_TYPE           (MimeType::instance)

using namespace std;

class MimeType {
    public:
        static MimeType instance;
        string fromExtension(string ext);

    private:
        MimeType();
        void insert(string exts, string mimetype);
        unordered_map<string, string> map;




};


#endif //EFSERV_MIMETYPE_H
