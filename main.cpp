#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <getopt.h>
#include <signal.h>
#include "ServEnv.h"
#include "log.h"
#include "EventLoop.h"
#include "AccessRule.h"

string config_path = "";

void print_usage() {
    printf(
            "EzerFileServer\n"
                    "Author : xiaozhuai\n"
                    "Email  : 798047000@qq.com\n"
                    "Usage  : efserv [OPTION]...\n"
                    "\n"
                    "All arguments are long options\n"
                    "  --config <file>           Define the ini config path, it will be \".efserv_config\" under web root by default\n"
                    "  --root <dir>              Define the web root path, it will be \"/var/www\" by default\n"
                    "  --log-level <level>       Define the log level, available levels are : disable, error, warning, info, debug\n"
                    "  --help                    Print this help message\n"
    );
}

void parse_arguments(int argc, char **argv) {

    int option_index = 0;
    struct option long_options[] = {
            {"config",    required_argument, NULL, 0},
            {"root",      required_argument, NULL, 0},
            {"log-level", required_argument, NULL, 0},
            {"help",      no_argument,       NULL, 0},
            {0, 0, 0,                              0}
    };

    #define EXIT_IF_NO_ARGUMENT()     if (optarg == NULL) { print_usage(); exit(1); }

    string logLevel;

    while (getopt_long(argc, argv, "", long_options, &option_index) != -1) {
        switch (option_index) {
            case 0:
                EXIT_IF_NO_ARGUMENT();
                config_path = optarg;
                break;

            case 1:
                EXIT_IF_NO_ARGUMENT();
                SERV_ENV.setWebRoot(optarg);
                break;

            case 2:
                EXIT_IF_NO_ARGUMENT();
                logLevel = optarg;
                if      (logLevel == "debug"  )   { set_log_level(LOG_LEVEL_DEBUG);   }
                else if (logLevel == "info"   )   { set_log_level(LOG_LEVEL_INFO);    }
                else if (logLevel == "warning")   { set_log_level(LOG_LEVEL_WARNING); }
                else if (logLevel == "error"  )   { set_log_level(LOG_LEVEL_ERROR);   }
                else if (logLevel == "disable")   { set_log_level(LOG_LEVEL_DISABLE); }
                break;

            case 3:
                print_usage();
                exit(0);

            default:
                break;

        }
    }
}

void signal_handler(int signal) {
    if (signal == SIGINT) {
        printf("\r");               // hide ^C, when press CTRL+C, terminal will print ^C, so print a '\r' to override it
        LOGI("Stopped by user, sigint");
        exit(0);
    }
}

void register_signal_handler() {
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = signal_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);
}

int main(int argc, char **argv) {

    register_signal_handler();

    parse_arguments(argc, argv);
    SERV_ENV.dumpWebRoot();
    SERV_ENV.parseConfig(config_path);
    SERV_ENV.dumpConfigs();
    ACCESS_RULE.loadAccessRule();

    EventLoop::init();
    EventLoop::start();

    exit(0);
}