/**
 * @author : xiaozhuai
 * @date   : 17/3/20
 */

#include "StringUtils.h"

vector<string> StringUtils::split(string str, string pattern, int limit) {
    string::size_type pos;
    vector<string> result;
    str += pattern;//扩展字符串以方便操作
    int size=str.size();
    for(int i=0; i<size; i++)
    {
        pos = str.find(pattern, i);
        if(pos < size)
        {
            string s=str.substr(i, pos-i);
            result.push_back(s);
            i = pos+pattern.size() - 1;
        }
        if(limit>=2 && result.size()==limit-1){
            string s=str.substr(i+1, size-i-1-pattern.size());
            result.push_back(s);
            break;
        }
    }
    return result;
}
