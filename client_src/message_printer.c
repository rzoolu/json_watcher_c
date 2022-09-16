#include "message_printer.h"
#include <stdio.h>

#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define YELLOW "\033[1;33m"
#define RESET_COLOR "\033[0m"

#define COLOR_PRINT(color, format, ...)              \
    printf(color format RESET_COLOR, ##__VA_ARGS__); \
    fflush(stdout);

void print_NEW_SSID_message(const NEW_SSID_message* message)
{
    COLOR_PRINT(GREEN, "SSID: %s is added to the list with following parameters:\n", message->new_ssid);

    for (size_t i = 0; i < message->number_of_parameters; ++i)
    {
        COLOR_PRINT(GREEN, "%s: %s\n",
                    message->parameters[i].param_name,
                    message->parameters[i].param_value);
    }
}

void print_REMOVED_SSID_message(const REMOVED_SSID_message* message)
{
    COLOR_PRINT(RED, "SSID: %s is removed for the list.\n", message->removed_ssid);
}

void print_SSID_CHANGED_message(const SSID_CHANGED_message* message)
{
    COLOR_PRINT(YELLOW, "SSID: %s value(s) has changed:\n", message->changed_ssid);

    for (size_t i = 0; i < message->number_of_changed_parameters; ++i)
    {
        COLOR_PRINT(YELLOW, "%s from %s to %s\n",
                    message->changed_parameters[i].param_name,
                    message->changed_parameters[i].param_old_value,
                    message->changed_parameters[i].param_new_value);
    }
}