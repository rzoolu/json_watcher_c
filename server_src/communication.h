#pragma once
#include "access_points_data.h"

// Initializes IPC, shall be called before send_* functions
void init_communication(void);

// Cleanup communication resources, shall be send after communication is finished
void close_communication(void);

// Sends new_ssid message
void send_new_ssid(const access_point* new_ssid);

// Sends ssid removed message
void send_removed_ssid(const access_point* old_ssid);

// Sends ssid param change message
// @from structure with old values
// @to strucure with new values
// @changes list of changed params
void send_param_change(const access_point* from, const access_point* to, const param_changes changes);