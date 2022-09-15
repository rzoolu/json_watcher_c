#pragma once
#include <ipc_interface.h>

// Printing/presentation functions for each message type

void print_NEW_SSID_message(const NEW_SSID_message* message);

void print_REMOVED_SSID_message(const REMOVED_SSID_message* message);

void print_SSID_CHANGED_message(const SSID_CHANGED_message* message);
