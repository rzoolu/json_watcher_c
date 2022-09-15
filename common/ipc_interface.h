#pragma once

// IPC interface defintion
// File to be shared between two application

#define IPC_KEY_PATH "./server"
#define IPC_KEY_ID 42

#define MAX_MESSAGE_DATA_LENGTH 1024
#define MAX_SSID_LENGTH 32
#define MAX_PARAM_NAME_LENGTH 16
#define MAX_PARAM_VALUE_LENGTH 16

// Generic message definition.
// Defines maximum size of message and allows to receive arbitrary message.
// Message structs defined below has the same layout,
// thus message can be casted to concrete message type.
typedef struct message
{
    long message_type;
    char message_data[MAX_MESSAGE_DATA_LENGTH];
} message;

// Parameter - value pair in form of strings
typedef struct parameter_value
{
    char param_name[MAX_PARAM_NAME_LENGTH];
    char param_value[MAX_PARAM_VALUE_LENGTH];
} parameter_value;

// Parameter - value difference
// to express change of given parameter from old to new value.
typedef struct parameter_value_diff
{
    char param_name[MAX_PARAM_NAME_LENGTH];
    char param_old_value[MAX_PARAM_VALUE_LENGTH];
    char param_new_value[MAX_PARAM_VALUE_LENGTH];
} parameter_value_diff;

// Message for informing about new SSID
#define MSG_ID_NEW_SSID 10

typedef struct NEW_SSID_message
{
    long message_type;
    char new_ssid[MAX_SSID_LENGTH];
    unsigned int number_of_parameters;
    parameter_value parameters[]; // flexible array
} NEW_SSID_message;

// Message for informing about SSID removal
#define MSG_ID_REMOVED_SSID 20

typedef struct REMOVED_SSID_message
{
    long message_type;
    char removed_ssid[MAX_SSID_LENGTH];
} REMOVED_SSID_message;

// Message for informing about SSID parameters change
#define MSG_ID_SSID_CHANGED 30

typedef struct SSID_CHANGED_message
{
    long message_type;
    char changed_ssid[MAX_SSID_LENGTH];
    unsigned int number_of_changed_parameters;
    parameter_value_diff changed_parameters[]; // flexible array
} SSID_CHANGED_message;
