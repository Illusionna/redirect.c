#include <stdio.h>


static inline void print_how_to_use_app(char *app_name) {
    printf("------------------------------------------------------------------------------\n");
    printf(">>>>                 Welcome to use HTTP Redirect Server!                 <<<<\n");
    printf("------------------------------------------------------------------------------\n");
    printf("Usage:\n");
    printf("    %s run [port]                - Start the server (11111 for default)\n", app_name);
    printf("    %s list                      - View all current routes\n", app_name);
    printf("    %s add [old_url] [new_url]   - Add a redirect route\n", app_name);
    printf("    %s del [new_url]             - Delete the redirect route\n", app_name);
    printf("------------------------------------------------------------------------------\n");
    printf("Example:\n");
    printf("    %s  run\n", app_name);
    printf("    %s  run  8080\n", app_name);
    printf("    %s  list\n", app_name);
    printf("    %s  add  https://orzzz.net/index.html  /test\n", app_name);
    printf("    %s  add  https://example.com\n", app_name);
    printf("    %s  add  https://www.baidu.com/  /xyz/0x5f3759df\n", app_name);
    printf("    %s  del  /xyz/0x5f3759df\n", app_name);
    printf("    %s  del  /test\n", app_name);
    printf("------------------------------------------------------------------------------\n");
}


static inline void print_how_to_use_app_run(char *app_name) {
    printf("Usage:\n");
    printf("    \x1b[1;37;42m%s run [port]                - Start the server (11111 for default)\x1b[0m\n", app_name);
    printf("    %s list                      - View all current routes\n", app_name);
    printf("    %s add [old_url] [new_url]   - Add a redirect route\n", app_name);
    printf("    %s del [new_url]             - Delete the redirect route\n", app_name);
    printf("------------------------------------------------------------------------------\n");
    printf("Example:\n");
    printf("    \x1b[1;37;42m%s  run\x1b[0m\n", app_name);
    printf("    \x1b[1;37;42m%s  run  8080\x1b[0m\n", app_name);
    printf("    %s  list\n", app_name);
    printf("    %s  add  https://orzzz.net/index.html  /test\n", app_name);
    printf("    %s  add  https://example.com\n", app_name);
    printf("    %s  add  https://www.baidu.com/  /xyz/0x5f3759df\n", app_name);
    printf("    %s  del  /xyz/0x5f3759df\n", app_name);
    printf("    %s  del  /test\n", app_name);
    printf("------------------------------------------------------------------------------\n");
}


static inline void print_how_to_use_app_add(char *app_name) {
    printf("Usage:\n");
    printf("    %s run [port]                - Start the server (11111 for default)\n", app_name);
    printf("    %s list                      - View all current routes\n", app_name);
    printf("    \x1b[1;37;42m%s add [old_url] [new_url]   - Add a redirect route\x1b[0m\n", app_name);
    printf("    %s del [new_url]             - Delete the redirect route\n", app_name);
    printf("------------------------------------------------------------------------------\n");
    printf("Example:\n");
    printf("    %s  run\n", app_name);
    printf("    %s  run  8080\n", app_name);
    printf("    %s  list\n", app_name);
    printf("    \x1b[1;37;42m%s  add  https://orzzz.net/index.html  /test\x1b[0m\n", app_name);
    printf("    \x1b[1;37;42m%s  add  https://example.com\x1b[0m\n", app_name);
    printf("    \x1b[1;37;42m%s  add  https://www.baidu.com/  /xyz/0x5f3759df\x1b[0m\n", app_name);
    printf("    %s  del  /xyz/0x5f3759df\n", app_name);
    printf("    %s  del  /test\n", app_name);
    printf("------------------------------------------------------------------------------\n");
}


static inline void print_how_to_use_app_del(char *app_name) {
    printf("Usage:\n");
    printf("    %s run [port]                - Start the server (11111 for default)\n", app_name);
    printf("    %s list                      - View all current routes\n", app_name);
    printf("    %s add [old_url] [new_url]   - Add a redirect route\n", app_name);
    printf("    \x1b[1;37;42m%s del [new_url]             - Delete the redirect route\x1b[0m\n", app_name);
    printf("------------------------------------------------------------------------------\n");
    printf("Example:\n");
    printf("    %s  run\n", app_name);
    printf("    %s  run  8080\n", app_name);
    printf("    %s  list\n", app_name);
    printf("    %s  add  https://orzzz.net/index.html  /test\n", app_name);
    printf("    %s  add  https://example.com\n", app_name);
    printf("    %s  add  https://www.baidu.com/  /xyz/0x5f3759df\n", app_name);
    printf("    \x1b[1;37;42m%s  del  /xyz/0x5f3759df\x1b[0m\n", app_name);
    printf("    \x1b[1;37;42m%s  del  /test\x1b[0m\n", app_name);
    printf("------------------------------------------------------------------------------\n");
}


static inline void print_how_to_use_app_list(char *app_name) {
    printf("Usage:\n");
    printf("    %s run [port]                - Start the server (11111 for default)\n", app_name);
    printf("    \x1b[1;37;42m%s list                      - View all current routes\x1b[0m\n", app_name);
    printf("    %s add [old_url] [new_url]   - Add a redirect route\n", app_name);
    printf("    %s del [new_url]             - Delete the redirect route\n", app_name);
    printf("------------------------------------------------------------------------------\n");
    printf("Example:\n");
    printf("    %s  run\n", app_name);
    printf("    %s  run  8080\n", app_name);
    printf("    \x1b[1;37;42m%s  list\x1b[0m\n", app_name);
    printf("    %s  add  https://orzzz.net/index.html  /test\n", app_name);
    printf("    %s  add  https://example.com\n", app_name);
    printf("    %s  add  https://www.baidu.com/  /xyz/0x5f3759df\n", app_name);
    printf("    %s  del  /xyz/0x5f3759df\n", app_name);
    printf("    %s  del  /test\n", app_name);
    printf("------------------------------------------------------------------------------\n");
}