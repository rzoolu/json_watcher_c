#pragma once

typedef struct _zmsg_t zmsg_t;

// Printing/presentation functions for each message type

void print_NEW_SSID_message(zmsg_t* message);

void print_REMOVED_SSID_message(zmsg_t* message);

void print_SSID_CHANGED_message(zmsg_t* message);
