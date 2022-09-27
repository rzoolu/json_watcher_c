#include "message_printer.h"

#include <utils.h>

#include <czmq.h>
#include <stdio.h>

#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define YELLOW "\033[1;33m"
#define RESET_COLOR "\033[0m"

#define COLOR_PRINT(color, format, ...)              \
    printf(color format RESET_COLOR, ##__VA_ARGS__); \
    fflush(stdout);

void print_NEW_SSID_message(zmsg_t* message)
{
    // 5 frames: ssid param value param value
    assert(zmsg_size(message) == 5);

    char* ssid = zmsg_popstr(message);
    assert(ssid);
    COLOR_PRINT(GREEN, "SSID: %s is added to the list with following parameters:\n", ssid);
    SAFE_FREE(ssid);

    // todo:
    for (int i = 0; i < 2; ++i)
    {
        char* param_name = zmsg_popstr(message);
        assert(param_name);
        char* param_value = zmsg_popstr(message);
        assert(param_value);

        COLOR_PRINT(GREEN, "%s: %s\n", param_name, param_value);

        SAFE_FREE(param_name);
        SAFE_FREE(param_value);
    }
}

void print_REMOVED_SSID_message(zmsg_t* message)
{
    // 1 frame: ssid
    assert(zmsg_size(message) == 1);

    char* ssid = zmsg_popstr(message);
    assert(ssid);

    COLOR_PRINT(RED, "SSID: %s is removed for the list.\n", ssid);
    SAFE_FREE(ssid);
}

void print_SSID_CHANGED_message(zmsg_t* message)
{
    // 7 frames: ssid param old_value new_value param old_value new_value
    assert(zmsg_size(message) == 7);

    char* ssid = zmsg_popstr(message);
    assert(ssid);

    COLOR_PRINT(YELLOW, "SSID: %s value(s) has changed:\n", ssid);
    SAFE_FREE(ssid);

    // todo:
    for (int i = 0; i < 2; ++i)
    {
        char* param_name = zmsg_popstr(message);
        assert(param_name);
        char* from = zmsg_popstr(message);
        assert(from);
        char* to = zmsg_popstr(message);
        assert(to);

        COLOR_PRINT(YELLOW, "%s from %s to %s\n", param_name, from, to);

        SAFE_FREE(param_name);
        SAFE_FREE(from);
        SAFE_FREE(to);
    }
}