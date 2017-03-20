/**
 * @author : xiaozhuai
 * @date   : 17/3/16
 */

#ifndef EZERFILESERVER_LOG_H
#define EZERFILESERVER_LOG_H

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tinyformat.h>

#define LOG_LEVEL_DEBUG         -1;
#define LOG_LEVEL_INFO          0;
#define LOG_LEVEL_WARNING       1;
#define LOG_LEVEL_ERROR         2;
#define LOG_LEVEL_DISABLE       10000;

extern time_t __now_time__;
extern char*  __now_time_str__;
extern int __log_level__;


#define set_log_level(level)                __log_level__ = level

#define print_time()                        time(&__now_time__);                                \
                                            __now_time_str__ = asctime(gmtime(&__now_time__));  \
                                            __now_time_str__[strlen(__now_time_str__) - 1] = 0; \
                                            printf("[%s] ", __now_time_str__);


#define ANSI_COLOR_RED                      "\033[31m"
#define ANSI_COLOR_ORANGE                   "\033[33m"
#define ANSI_COLOR_NORMAL                   "\033[0m"
#define ANSI_COLOR_GREEN                    "\033[32m"
#define ANSI_COLOR_BLUE                     "\033[34m"

#define color_end()                         printf("\n%s", ANSI_COLOR_NORMAL)
#define color_start(color)                  printf(color)

#define LOG(level, color, prefix, ...)      if(level>=__log_level__){               \
                                                color_start(color);                 \
                                                print_time();                       \
                                                printf(prefix);                     \
                                                tfm::printf(__VA_ARGS__);           \
                                                color_end();                        \
                                            }


#define LOGE(...)                           LOG(2,  ANSI_COLOR_RED,    "E: ", __VA_ARGS__)
#define LOGW(...)                           LOG(1,  ANSI_COLOR_ORANGE, "W: ", __VA_ARGS__)
#define LOGI(...)                           LOG(0,  ANSI_COLOR_NORMAL, "I: ", __VA_ARGS__)
#define LOGD(...)                           LOG(-1, ANSI_COLOR_GREEN,  "D: ", __VA_ARGS__)

#endif //EZERFILESERVER_LOG_H
