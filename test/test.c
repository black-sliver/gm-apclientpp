#include "../include/gm-apclientpp.h"
#include <stdbool.h>
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
#define SCOUTS "[1]"  // any location that exists for the slot


#define RES(x) ((x) ? "OK": "err")


bool poll_for(const char* marker, const char* error_marker)
{
    unsigned long t = (unsigned long)time(NULL);

    printf("polling ...\n");
    while ((unsigned long)time(NULL) - t < 5) {
        const char* script = apclient_poll();
        if (strcmp(script, "{}") != 0) {
            printf("%s\n", script);
            if (strstr(script, marker) != NULL)
                return true; // connected
            if (error_marker && strstr(script, error_marker) != NULL)
                return false; // unexpected result
            if (strstr(script, "_disconnected(") != NULL)
                return false; // unexpected disconnect
        }
    }
    return false;
}

int main(int argc, char** argv)
{
    // test data for render_json
    const char* chat = "{\"cmd\":\"PrintJSON\",\"data\":[{\"text\":\"Player1: Hello, world!\"}],"
                       "\"message\":\"Hello, world!\",\"slot\":1,\"team\":0,\"type\":\"Chat\"}";
    const char* rendered;

    // init lib and connect
    printf("init: %s\n", RES(apclient_init(1)));
    printf("start connect: %s\n", RES(apclient_connect("", GAME, HOST)));
    printf("set items handling %d: %s\n", ITEMS_HANDLING, RES(apclient_set_items_handling(ITEMS_HANDLING)));
    printf("set version %d.%d.%d: %s\n", VERSION, RES(apclient_set_version(VERSION)));

    // wait for socket_connected and room_info
    if (!poll_for("ap_room_info(", NULL))
        goto exit;
    printf("\n");

    // login
    printf("send connect slot %s: %s\n", SLOT, RES(apclient_connect_slot(SLOT, "", TAGS)));
    printf("send connect update: %s\n", RES(apclient_connect_update(TAGS2)));

    // wait for slot connected
    if (!poll_for("ap_slot_connected(", "ap_slot_connected"))
        goto exit;
    printf("\n");

    // test render_json
    rendered = apclient_render_json(chat, 0);
    if (strcmp(rendered, "Player1: Hello, world!") == 0) {
        printf("render json: OK\n");
    } else {
        printf("render json: err (\"%s\")\n%s\n", apclient_poll()); // poll should print the error
        goto exit;
    }

    // send chat
    printf("send say \"%s\": %s\n", "Hello, world!", RES(apclient_say("Hello, world!")));

    // wait for receiving it back
    if (!poll_for("Hello, world!", NULL))
        goto exit;
    printf("\n");

    // send scout
    printf("send scouts: %s: %s\n", SCOUTS, RES(apclient_location_scouts(SCOUTS, 0)));

    // server may answer in different order, so definitely wait for reply before sending Goodbye
    if (!poll_for("ap_location_info(", NULL))
        goto exit;
    printf("\n");

    // send Goodbye to chat
    printf("send say \"%s\": %s\n", "Goodbye!", RES(apclient_say("Goodbye!")));

    // wait for receiving the Goodbye message back
    if (!poll_for("Goodbye!", NULL))
        goto exit;
    printf("\n");

    // done
    apclient_disconnect();

    printf("stopping\n");

    apclient_deinit();
    return 0;

exit:
    fprintf(stderr, "\nUnexpected or no response\n");
    apclient_deinit();    
    return 1;
}
