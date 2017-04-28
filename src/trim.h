/**
 * @author : xiaozhuai
 * @date   : 17/3/17
 */

#ifndef EZERFILESERVER_TRIM_H
#define EZERFILESERVER_TRIM_H

#include <string>
using namespace std;


#define ltrim(text, ch)             if (!text.empty()) text.erase(0, text.find_first_not_of(ch));

#define rtrim(text, ch)             if (!text.empty()) text.erase(text.find_last_not_of(ch) + 1);

#define  trim(text, ch)             ltrim(text, ch); rtrim(text, ch);

#endif //EZERFILESERVER_TRIM_Hß
