/**
 * @author : xiaozhuai
 * @date   : 17/3/20
 */

#ifndef EFSERV_ACCESSRULE_H
#define EFSERV_ACCESSRULE_H

#include <string>
#include <vector>
#include <regex>
#include "ServEnv.h"
#include "FileHandler.h"
#include "trim.h"
#include "StringUtils.h"
#include "log.h"

using namespace std;

#define ACCESS_RULE             (AccessRule::instance)

class Rule{
    public:
        bool permitted;
        string pattern;
        Rule(bool p, string r){
            permitted = p;
            pattern = r;
        }
};



class AccessRule {
    public:
        static AccessRule instance;
        bool permissible(string url);
        void loadAccessRule();

    private:
        vector<Rule> rules;


};


#endif //EFSERV_ACCESSRULE_H
