#include "../include/gm-apclientpp.h"
#include <stdio.h>
#include <string.h>
#include <time.h>


#define GAME "Test"
#define HOST "localhost"
#define SLOT "Player1"
#define TAGS "[]"
#define TAGS2 "[\"deathlink\"]"
#define VERSION 0, 4, 4
#define ITEMS_HANDLING 7


#define RES(x) ((x) ? "OK": "err")


int main(int argc, char** argv)
{
    int res = 1;
    unsigned long t = (unsigned long)time(NULL);
    const char* script = "";

    printf("init: %s\n", RES(apclient_init(1)));
    printf("start connect: %s\n", RES(apclient_connect("", GAME, HOST)));
    printf("set items handling %d: %s\n", ITEMS_HANDLING, RES(apclient_set_items_handling(ITEMS_HANDLING)));
    printf("set version %d.%d.%d: %s\n", VERSION, RES(apclient_set_version(VERSION)));

    // wait for socket_connected and room_info
    printf("polling ...\n");
    while ((unsigned long)time(NULL) - t < 5) {
        script = apclient_poll();
        if (strcmp(script, "{}") != 0) {
            printf("%s\n", script);
            if (strstr(script, "ap_room_info(") != NULL)
                break; // connected
        }
    }
    if (strlen(script) < 3)
        goto exit;

    printf("send connect slot %s: %s\n", SLOT, RES(apclient_connect_slot(SLOT, "", TAGS)));
    printf("send connect update: %s\n", RES(apclient_connect_update(TAGS2)));
    printf("send say \"%s\": %s\n", "Hello, world!", RES(apclient_say("Hello, world!")));
    printf("send say \"%s\": %s\n", "Goodbye!", RES(apclient_say("Goodbye!")));

    printf("polling ...\n");
    while ((unsigned long)time(NULL) - t < 5) {
        script = apclient_poll();
        if (strcmp(script, "{}") != 0) {
            printf("%s\n", script);
            if (strstr(script, "_disconnected(") != NULL) {
                printf("disconnected\n");
                break; // disconnected
            }
            if (strstr(script, "Goodbye!") != NULL) {
                printf("done\n");
                res = 0;
                break; // got the final expected message
            }
        }
    }

    apclient_disconnect();

    printf("stopping\n");

    apclient_deinit();
    return res;

exit:
    apclient_deinit();    
    return 1;
}
