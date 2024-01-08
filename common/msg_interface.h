#pragma once

#include <assert.h>

#define ZMQ_PORT "4559"

// Message interface defintion
// File to be shared between two application

// Defines unique msg indetifier, e.g: "AP_WATCH/NEW_SSID"
#define FULL_MSG_ID(iface, msg) iface "/" msg

// s - msg full id, m - msg body
#define MSG_PICTURE "sm"

#define AP_WATCH_IF "AP_WATCH"

// AP_WATCH_IF messges:

#define NEW_SSID_MSG "NEW_SSID"
// msg picture: "s4(s,s)+": ssid,number_of_params,(param,value)+

#define REMOVED_SSID_MSG "REMOVED_SSID"
// msg picture: "s": ssid

#define CHANGED_SSID_MSG "CHANGED_SSID"
// msg picture: "s4(s,s)+": ssid,number_of_changed_params,(param_name,old_value,new_value)+
