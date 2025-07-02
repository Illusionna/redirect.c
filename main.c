#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./utils/handler.h"
#include "./utils/usage.inl"

#define PROXY_PORT 11111



int main(int argc, char *argv[], char *env[]) {
    if (argc < 2) {
        print_how_to_use_app(argv[0]);
        return 0;
    }

    if (strcmp(argv[1], "run") == 0) {
        if (argc == 2) {
            return run_server(argv[0], PROXY_PORT);
        } else if (argc == 3) {
            int port = atoi(argv[2]);
            if (port > 0 && port < 65536) {
                return run_server(argv[0], port);
            } else {
                printf("\x1b[1;37;41mError:\x1b[0m the host port must be at 1 ~ 65535.\n");
                return 1;
            }
        } else {
            print_how_to_use_app_run(argv[0]);
            return 0;
        }
    }
    
    else if (strcmp(argv[1], "add") == 0) {
        if (argc == 3) {
            return add_route(argv[0], argv[2], NULL);
        } else if (argc == 4) {
            return add_route(argv[0], argv[2], argv[3]);
        } else {
            print_how_to_use_app_add(argv[0]);
            return 0;
        }
    }

    else if (strcmp(argv[1], "del") == 0) {
        if (argc != 3) {
            print_how_to_use_app_del(argv[0]);
            return 0;
        } else {
            return delete_route(argv[0], argv[2]);
        }
    }

    else if (strcmp(argv[1], "list") == 0) {
        if (argc != 2) {
            print_how_to_use_app_list(argv[0]);
            return 0;
        } else {
            return list_route(argv[0]);
        }
    }

    else {
        print_how_to_use_app(argv[0]);
        return 0;
    }

    return 0;
}