#include "handler.h"


static Mutex mutex;
static HashMap *dict = NULL;
static Socket S = SOCKET_INVALID;
static Socket C = SOCKET_INVALID;
static volatile int PROXY_SERVICE_RUNNING = 1;  // Make sure that all threads use it up to date.


void interrupt_handler(int semaphore) {
    printf("\n[\x1b[1;32mInterrupt\x1b[0m] disconnect socket and free memory.\n");
    PROXY_SERVICE_RUNNING = 0;
    if (C != SOCKET_INVALID) socket_close(C);
    if (S != SOCKET_INVALID) socket_close(S);
    socket_destroy();
    hashmap_destroy(dict);
    mutex_destroy(&mutex);
}


int http_redirect(void *args) {
    Socket c = (Socket)(intptr_t)args;

    char buffer[1024];
    int n = socket_recv(c, buffer, sizeof(buffer) - 1, 0);
    if (n <= 0) {
        printf("\x1b[1;37;45mWarning:\x1b[0m \x1b[1;31mreceiving data failed, redirect service exception.\x1b[0m\n");
        socket_close(c);
        return 1;
    }
    buffer[n] = '\0';

    char method[16];
    char path[256];
    char version[32];

    if (sscanf(buffer, "%s %s %s", method, path, version) == 3) {
        mutex_lock(&mutex);
        char *url = hashmap_get(dict, path);
        mutex_unlock(&mutex);

        if (url == NULL) {
            char response[128] = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n<html><body><h1>404 Not Found</h1></body></html>\n";
            socket_send(c, response, strlen(response), 0);
            socket_close(c);
            return 0;
        }

        char response[1024];
        snprintf(
            response, 1024,
            "HTTP/1.1 302 Found\r\n"
            "Location: %s\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: 0\r\n"
            "Connection: close\r\n"
            "\r\n", url
        );
        socket_send(c, response, strlen(response), 0);
        socket_close(c);
        return 0;
    } else {
        char response[128] = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n<html><body><h1>HTTP parsing failed\n</h1></body></html>";
        socket_send(c, response, strlen(response), 0);
        socket_close(c);
        return 1;
    }
}


/**
 * @brief The function that takes over the lifespan of socket is responsible for disconnecting socket.
 * @param args It is recommended to pass socket value (not reference).
 * @return Not important.
**/
int coroutine_handler(void *args) {
    Socket c = (Socket)(intptr_t)args;

    char buffer[1024];
    int n = socket_recv(c, buffer, sizeof(buffer) - 1, 0);
    if (n <= 0) {
        printf("\x1b[1;37;45mWarning:\x1b[0m \x1b[1;31mreceiving data failed, communication coroutine service exception.\x1b[0m\n");
        socket_close(c);
        return 1;
    }
    buffer[n] = '\0';

    if (strcmp(buffer, "LIST") == 0) {
        mutex_lock(&mutex);
        char *response = hashmap_print(dict);
        mutex_unlock(&mutex);
        socket_send(c, response, strlen(response), 0);
        free(response);
    }
    
    else if (strncmp(buffer, "ADD:", 4) == 0) {
        char old_url[768];
        char new_url[256];
        int result = sscanf(buffer + 4, "%[^|]|%s", old_url, new_url);
        if (result == 2) {
            mutex_lock(&mutex);
            hashmap_put(dict, new_url, old_url);
            mutex_unlock(&mutex);
        } else {
            socket_send(c, "Warning: \"ADD\" parsing failed.", 30, 0);
        }
    }

    else if (strncmp(buffer, "DEL:", 4) == 0) {
        char url[256];
        int result = sscanf(buffer, "DEL:%s", url);
        if (result == 1) {
            mutex_lock(&mutex);
            if (hashmap_remove(dict, url)) socket_send(c, "Warning: not found it.", 22, 0);
            mutex_unlock(&mutex);
        } else {
            socket_send(c, "Warning: \"DEL\" parsing failed.", 30, 0);
        }
    }

    else socket_send(c, "Warning: unknown command.", 25, 0);

    socket_close(c);
    return 0;
}


int coroutine_server(void *args) {
    // Initialize a socket.
    if (socket_init()) {
        printf("\x1b[1;37;41mError:\x1b[0m \x1b[1;31minitialize socket failed, communication coroutine service exception.\x1b[0m\n");
        return 1;
    }

    // Create the coroutine communication server.
    C = socket_create(AF_INET, SOCK_STREAM, 0);
    if (C == SOCKET_INVALID) {
        printf("\x1b[1;37;41mError:\x1b[0m \x1b[1;31mcreate socket failed, communication coroutine service exception.\x1b[0m\n");
        return 1;
    }

    // Set port reuse.
    if (socket_setopt(C, SOL_SOCKET, SO_REUSEADDR) == SOCKET_INVALID) {
        printf("\x1b[1;37;41mError:\x1b[0m \x1b[1;31msocket port reuse failed, communication coroutine service exception.\x1b[0m\n");
        socket_close(C);
        return 1;
    }

    // Configure socket.
    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    socket_config(&server, AF_INET, "127.0.0.1", COROUTINE_PORT);

    // Bind socket.
    if (socket_bind(C, &server, sizeof(server)) == SOCKET_INVALID) {
        printf("\x1b[1;37;41mError:\x1b[0m \x1b[1;31msocket bind failed, communication coroutine service exception.\x1b[0m\n");
        socket_close(C);
        return 1;
    }

    // Monitor socket.
    if (socket_listen(C, 4) == SOCKET_INVALID) {
        printf("\x1b[1;37;41mError:\x1b[0m \x1b[1;31msocket listen failed, communication coroutine service exception.\x1b[0m\n");
        socket_close(C);
        return 1;
    }

    // The coroutine communication loop body.
    while (PROXY_SERVICE_RUNNING) {
        struct sockaddr_in client;
        int client_length = sizeof(client);

        Socket c = socket_accept(C, &client, &client_length);
        if (c == SOCKET_INVALID) {
            if (PROXY_SERVICE_RUNNING) printf("\x1b[1;37;45mWarning:\x1b[0m \x1b[1;31msocket accept failed, communication coroutine service exception.\x1b[0m\n");
            continue;
        }

        printf("coroutine client %s:%d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
        /*
        Value passing must be used, not reference passing.
    
        (Incorrect) >>> thread_create(&thread, coroutine_handler, &c);

        The value of variable `c` in stack segment (not heap segment) will change after the next while loop. If it is passed by reference, then the `c` in stack of the sub-thread might be changed.
        */
        Thread thread;
        if (thread_create(&thread, coroutine_handler, (void *)(intptr_t)c)) {
            printf("\x1b[1;37;45mWarning:\x1b[0m \x1b[1;31mcreate coroutine handler thread failed, communication coroutine service exception.\x1b[0m\n");
            socket_close(c);
            continue;
        }
        thread_detach(&thread);
    }
    return 0;
}


int run_server(char *app_name, int port) {
    // CTRL + C.
    signal(SIGINT, interrupt_handler);

    // Create a mutex lock.
    if (mutex_create(&mutex, 1)) {
        printf("\x1b[1;37;41mError:\x1b[0m create mutex lock failed.\n");
        return 1;
    }

    // Create a HashMap data structure to store key-value pairs.
    dict = hashmap_create();

    // Start the coroutine server to ensure the communication service.
    Thread thread;
    if (thread_create(&thread, coroutine_server, NULL)) {
        printf("\x1b[1;37;41mError:\x1b[0m create coroutine server thread failed.\n");
        hashmap_destroy(dict);
        mutex_destroy(&mutex);
        return 1;
    }
    thread_detach(&thread);

    // Initialize a socket.
    if (socket_init()) {
        printf("\x1b[1;37;41mError:\x1b[0m initialize socket failed.\n");
        hashmap_destroy(dict);
        mutex_destroy(&mutex);
        return 1;
    }

    // Create the proxy redirection server.
    S = socket_create(AF_INET, SOCK_STREAM, 0);
    if (S == SOCKET_INVALID) {
        printf("\x1b[1;37;41mError:\x1b[0m create socket failed.\n");
        socket_destroy();
        hashmap_destroy(dict);
        mutex_destroy(&mutex);
        return 1;
    }

    // Set port reuse.
    if (socket_setopt(S, SOL_SOCKET, SO_REUSEADDR) == SOCKET_INVALID) {
        printf("\x1b[1;37;41mError:\x1b[0m socket port reuse failed.\n");
        socket_close(S);
        socket_destroy();
        hashmap_destroy(dict);
        mutex_destroy(&mutex);
        return 1;
    }

    // Configure socket.
    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    socket_config(&server, AF_INET, "127.0.0.1", port);

    // Bind socket.
    if (socket_bind(S, &server, sizeof(server)) == SOCKET_INVALID) {
        printf("\x1b[1;37;41mError:\x1b[0m socket bind failed.\n");
        socket_close(S);
        socket_destroy();
        hashmap_destroy(dict);
        mutex_destroy(&mutex);
        return 1;
    }

    // Monitor socket.
    if (socket_listen(S, 128) == SOCKET_INVALID) {
        printf("\x1b[1;37;41mError:\x1b[0m socket listen failed.\n");
        socket_close(S);
        socket_destroy();
        hashmap_destroy(dict);
        mutex_destroy(&mutex);
        return 1;
    }

    printf(" * Serving Socket app '%s'\n", app_name);
    #if defined(__OS_UNIX__)
        printf(" * Redirector process id [\x1b[1;36m%d\x1b[0m]\n", getpid());
    #elif defined(__OS_WINDOWS__)
        printf(" * Redirector process id [\x1b[1;36m%ld\x1b[0m]\n", GetCurrentProcessId());
    #endif
    printf(" * Running on \x1b[1;32mhttp://127.0.0.1:%d\x1b[0m\n", port);
    printf("\x1b[1;33mPress CTRL+C to quit...\x1b[0m\n");

    // The main loop body.
    while (PROXY_SERVICE_RUNNING) {
        struct sockaddr_in client;
        int client_length = sizeof(client);

        Socket c = socket_accept(S, &client, &client_length);
        if (c == SOCKET_INVALID) {
            if (PROXY_SERVICE_RUNNING) printf("\x1b[1;37;45mWarning:\x1b[0m socket accept failed.\n");
            continue;
        }

        printf("redirector client %s:%d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));

        Thread thread;
        if (thread_create(&thread, http_redirect, (void *)(intptr_t)c)) {
            printf("\x1b[1;37;45mWarning:\x1b[0m \x1b[1;31mcreate redirector thread failed.\x1b[0m\n");
            socket_close(c);
            continue;
        }
        thread_detach(&thread);
    }

    #if defined(__OS_UNIX__)
        printf("|--> Finished server process id [\x1b[1;36m%d\x1b[0m]\n", getpid());
    #elif defined(__OS_WINDOWS__)
        printf("|--> Finished server process id [\x1b[1;36m%ld\x1b[0m]\n", GetCurrentProcessId());
    #endif
    return 0;
}


int add_route(char *app_name, char *old_url, char *new_url) {
    if (socket_init()) {
        printf("\x1b[1;37;41mError:\x1b[0m \x1b[1;31minitialize socket failed, communication coroutine service exception.\x1b[0m\n");
        return 1;
    }

    Socket s = socket_create(AF_INET, SOCK_STREAM, 0);
    if (s == SOCKET_INVALID) {
        printf("\x1b[1;37;41mError:\x1b[0m \x1b[1;31mcreate socket failed, communication coroutine service exception.\x1b[0m\n");
        socket_close(s);
        socket_destroy();
        return 1;
    }

    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    socket_config(&server, AF_INET, "127.0.0.1", COROUTINE_PORT);

    if (socket_connect(s, &server, sizeof(server)) == SOCKET_INVALID) {
        printf("\x1b[1;37;41mError:\x1b[0m is the redirector server running?\n");
        socket_close(s);
        socket_destroy();
        return 1;
    }

    char buffer[1024];
    if (new_url == NULL) {
        char *digest = md5(old_url);
        digest = truncate_string(digest, 6);
        snprintf(buffer, 1024, "ADD:%s|/%s", old_url, digest);
        free(digest);
    } else {
        snprintf(buffer, 1024, "ADD:%s|%s", old_url, new_url);
    }

    if (socket_send(s, buffer, strlen(buffer), 0) == SOCKET_INVALID) {
        printf("\x1b[1;37;41mError:\x1b[0m failed to send \"ADD\" data to the redirector server.\n");
        socket_close(s);
        socket_destroy();
        return 1;
    }

    char response[128];
    int bytes = socket_recv(s, response, sizeof(response) - 1, 0);
    if (bytes > 0) {
        response[bytes] = '\0';
        printf("%s\n", response);
    }

    socket_close(s);
    socket_destroy();
    return 0;
}


int delete_route(char *app_name, char *url) {
    if (socket_init()) {
        printf("\x1b[1;37;41mError:\x1b[0m \x1b[1;31minitialize socket failed, communication coroutine service exception.\x1b[0m\n");
        return 1;
    }

    Socket s = socket_create(AF_INET, SOCK_STREAM, 0);
    if (s == SOCKET_INVALID) {
        printf("\x1b[1;37;41mError:\x1b[0m \x1b[1;31mcreate socket failed, communication coroutine service exception.\x1b[0m\n");
        socket_close(s);
        socket_destroy();
        return 1;
    }

    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    socket_config(&server, AF_INET, "127.0.0.1", COROUTINE_PORT);

    if (socket_connect(s, &server, sizeof(server)) == SOCKET_INVALID) {
        printf("\x1b[1;37;41mError:\x1b[0m is the redirector server running?\n");
        socket_close(s);
        socket_destroy();
        return 1;
    }

    char buffer[256];
    snprintf(buffer, 256, "DEL:%s", url);
    if (socket_send(s, buffer, strlen(buffer), 0) == SOCKET_INVALID) {
        printf("\x1b[1;37;41mError:\x1b[0m failed to send \"DEL\" data to the redirector server.\n");
        socket_close(s);
        socket_destroy();
        return 1;
    }

    char response[128];
    int bytes = socket_recv(s, response, sizeof(response) - 1, 0);
    if (bytes > 0) {
        response[bytes] = '\0';
        printf("%s\n", response);
    }

    socket_close(s);
    socket_destroy();
    return 0;
}


int list_route(char *app_name) {
    if (socket_init()) {
        printf("\x1b[1;37;41mError:\x1b[0m \x1b[1;31minitialize socket failed, communication coroutine service exception.\x1b[0m\n");
        return 1;
    }

    Socket s = socket_create(AF_INET, SOCK_STREAM, 0);
    if (s == SOCKET_INVALID) {
        printf("\x1b[1;37;41mError:\x1b[0m \x1b[1;31mcreate socket failed, communication coroutine service exception.\x1b[0m\n");
        socket_close(s);
        socket_destroy();
        return 1;
    }

    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    socket_config(&server, AF_INET, "127.0.0.1", COROUTINE_PORT);

    if (socket_connect(s, &server, sizeof(server)) == SOCKET_INVALID) {
        printf("\x1b[1;37;41mError:\x1b[0m is the redirector server running?\n");
        socket_close(s);
        socket_destroy();
        return 1;
    }

    if (socket_send(s, "LIST", 4, 0) == SOCKET_INVALID) {
        printf("\x1b[1;37;41mError:\x1b[0m failed to send \"LIST\" data to the redirector server.\n");
        socket_close(s);
        socket_destroy();
        return 1;
    }

    char response[1024 * 1024];
    int bytes = socket_recv(s, response, sizeof(response) - 1, 0);
    if (bytes > 0) {
        response[bytes] = '\0';
        printf("%s\n", response);
    }

    socket_close(s);
    socket_destroy();
    return 0;
}