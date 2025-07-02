#ifndef _HANDLER_H_
#define _HANDLER_H_


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <signal.h>


#if defined(__OS_UNIX__)
    #include <unistd.h>
#elif defined(__OS_WINDOWS__)
    #include <windows.h>
#endif


#include "md5.h"
#include "socket.h"
#include "thread.h"
#include "hashmap.h"


#define COROUTINE_PORT 22222


void interrupt_handler(int semaphore);


int http_redirect(void *args);


int coroutine_handler(void *args);


int coroutine_server(void *args);


int run_server(char *app_name, int port);


int add_route(char *app_name, char *old_url, char *new_url);


int delete_route(char *app_name, char *url);


int list_route(char *app_name);


#endif