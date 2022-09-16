#pragma once

#include <assert.h>

// IPC interface defintion
// File to be shared between two application

#define IPC_KEY_PATH "./server"
#define IPC_KEY_ID 42

#define MAX_MESSAGE_DATA_LENGTH 1024u
#define MAX_NUMBER_OF_PARAMETERS 10u

#define MAX_SSID_LENGTH 32u
#define MAX_PARAM_NAME_LENGTH 16u
#define MAX_PARAM_VALUE_LENGTH 16u

// Generic message definition.
// Defines maximum size of message and allows to receive arbitrary message.
// Message structs defined below has the same layout,
// thus message can be casted to concrete message type.
typedef struct message
{
    long message_type;
    char message_data[MAX_MESSAGE_DATA_LENGTH];
} message;

// Checks whether max msg size is not grater than possible maximum
#define VALIDATE_MSG_MAX_SIZE(msg_max_size) static_assert((msg_max_size) < sizeof(message), \
                                                          "Message size exceeds maximum message size");

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

#define MAX_SIZE_OF_NEW_SSID_message (sizeof(NEW_SSID_message) + sizeof(parameter_value) * MAX_NUMBER_OF_PARAMETERS)
VALIDATE_MSG_MAX_SIZE(MAX_SIZE_OF_NEW_SSID_message);

// Message for informing about SSID removal
#define MSG_ID_REMOVED_SSID 20

typedef struct REMOVED_SSID_message
{
    long message_type;
    char removed_ssid[MAX_SSID_LENGTH];
} REMOVED_SSID_message;

#define MAX_SIZE_OF_REMOVED_SSID_message (sizeof(REMOVED_SSID_message))
VALIDATE_MSG_MAX_SIZE(MAX_SIZE_OF_REMOVED_SSID_message);

// Message for informing about SSID parameters change
#define MSG_ID_SSID_CHANGED 30

typedef struct SSID_CHANGED_message
{
    long message_type;
    char changed_ssid[MAX_SSID_LENGTH];
    unsigned int number_of_changed_parameters;
    parameter_value_diff changed_parameters[]; // flexible array
} SSID_CHANGED_message;

#define MAX_SIZE_OF_SSID_CHANGED_message (sizeof(SSID_CHANGED_message) + sizeof(parameter_value_diff) * MAX_NUMBER_OF_PARAMETERS)
VALIDATE_MSG_MAX_SIZE(MAX_SIZE_OF_SSID_CHANGED_message);