/**
 * @author : xiaozhuai
 * @date   : 17/3/20
 */

#include "AccessRule.h"

AccessRule AccessRule::instance;

void AccessRule::loadAccessRule() {

    string ruleFileName = SERV_ENV.getConfig(KEY_ACCESS_RULE, DEFAULT_ACCESS_RULE);

    /**
     * denied .efserv_access and .efserv_config by default
     */

    rules.push_back(Rule(false, ".efserv_config"));
    rules.push_back(Rule(false, ruleFileName));


    string rulePath = SERV_ENV.getAbsoluteWebRoot() + "/" + ruleFileName;
    FileHandler ruleFile(rulePath);
    string ruleText = ruleFile.readAsText();

    /**
     * first split by "\n"
     */
    vector<string> lines = StringUtils::split(ruleText, "\n");

    /**
     * parse each line
     */
    for(int i=0; i<lines.size(); i++){
        string line = lines[i];

        /**
         * trim line
         */
        trim(line, " ");

        /**
         * skip empty line
         */
        if(line.size()==0) continue;

        /**
         * skip comment line
         */
        if(line[0]=='#') continue;

        /**
         * split by '#' to avoid in-line comment
         */
        vector<string> tmp = StringUtils::split(line, "#", 2);
        string ruleBody = tmp[0];
        trim(ruleBody, " ");

        if(ruleBody.size()<3){
            LOGW("Access rule not recognized, file: %s, line: %d, %s", rulePath, (i+1), ruleBody);
            continue;
        }
        bool permitted;
        switch (ruleBody[0]){
            case '+':
                permitted = true;
                break;
            case '-':
                permitted = false;
                break;
            default:
                LOGW("Access rule not recognized, file: %s, line: %d, %s", rulePath, (i+1), ruleBody);
                continue;
        }
        if(ruleBody[1]==' '){
            string pattern = ruleBody.substr(2);
            rules.push_back(Rule(permitted, pattern));
            LOGD("Access rule recognized, permitted: %s, pattern: %s", permitted?"yes":"no", pattern);
        }else{
            LOGW("Access rule not recognized, file: %s, line: %d, %s", rulePath, (i+1), ruleBody);
        }
    }
}

bool AccessRule::permissible(string url) {
    bool permitted = true;
    /**
     * go through all rules, the last matched rule will be effective
     */
    for(int i=0; i<rules.size(); i++){
        regex r(rules[i].pattern);
        if(regex_match(url, r))
            permitted = rules[i].permitted;
    }
    return permitted;
}