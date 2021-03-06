/**
 * @author : xiaozhuai
 * @date   : 17/3/20
 */

#ifndef EFSERV_STRINGUTILS_H
#define EFSERV_STRINGUTILS_H

#include <string>
#include <vector>
#include <algorithm>

using namespace std;

class StringUtils {
    public:
        static vector<string> split(string str, string pattern, int limit=-1);
        static string replaceAll(string str, string find, string replace);
        static void toUpper(string &str);
        static void toLower(string &str);

};


#endif //EFSERV_STRINGUTILS_H
