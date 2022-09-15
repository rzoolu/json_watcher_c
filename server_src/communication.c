#include "communication.h"
#include <ipc_interface.h>
#include <trace.h>

#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>

static int msg_queue_id = 0;

void init_communication(void)
{
    key_t key = ftok(IPC_KEY_PATH, IPC_KEY_ID);
    msg_queue_id = msgget(key, 0666 | IPC_CREAT);

    if (msg_queue_id < 0)
    {
        TRACE_ERROR("Message queue creation problem.");
        exit(EXIT_FAILURE);
    }

    TRACE_INFO("Communication initalized.");
}

void close_communication(void)
{
    if (msgctl(msg_queue_id, IPC_RMID, NULL) != 0)
    {
        TRACE_ERROR("Message queue cannot be removed.");
    }
}

void send_new_ssid(const access_point* new_ssid)
{
    const size_t numberOfParameters = 2u;

    const size_t total_size_of_msg =
        sizeof(NEW_SSID_message) + sizeof(parameter_value) * numberOfParameters;

    NEW_SSID_message* new_ssid_msg = malloc(total_size_of_msg);

    TRACE_INFO("New SSID for %s msg sent. Msg size: %ld, number of changed params %ld",
               new_ssid->ssid, total_size_of_msg - sizeof(new_ssid_msg->message_type), numberOfParameters);

    new_ssid_msg->message_type = MSG_ID_NEW_SSID;

    strncpy(new_ssid_msg->new_ssid, new_ssid->ssid, MAX_SSID_LENGTH);
    // force null termination
    new_ssid_msg->new_ssid[MAX_SSID_LENGTH - 1] = 0;

    new_ssid_msg->number_of_parameters = numberOfParameters;

    strncpy(new_ssid_msg->parameters[0].param_name, "SNR", MAX_PARAM_NAME_LENGTH);
    snprintf(new_ssid_msg->parameters[0].param_value, MAX_PARAM_VALUE_LENGTH, "%u", new_ssid->SNR);

    strncpy(new_ssid_msg->parameters[1].param_name, "channel", MAX_PARAM_NAME_LENGTH);
    snprintf(new_ssid_msg->parameters[1].param_value, MAX_PARAM_VALUE_LENGTH, "%u", new_ssid->channel);

    msgsnd(msg_queue_id, new_ssid_msg, (total_size_of_msg - sizeof(new_ssid_msg->message_type)), 0);

    free(new_ssid_msg);
}

void send_removed_ssid(const access_point* old_ssid)
{
    TRACE_INFO("Removed SSID %s msg sent.", old_ssid->ssid);

    REMOVED_SSID_message removed_ssid_msg;
    removed_ssid_msg.message_type = MSG_ID_REMOVED_SSID;

    strncpy(removed_ssid_msg.removed_ssid, old_ssid->ssid, MAX_SSID_LENGTH);
    // force null termination
    removed_ssid_msg.removed_ssid[MAX_SSID_LENGTH - 1] = 0;

    msgsnd(msg_queue_id, &removed_ssid_msg, sizeof(removed_ssid_msg) - sizeof(removed_ssid_msg.message_type), 0);
}

void send_param_change(const access_point* from, const access_point* to, const param_changes changes)
{

    unsigned int number_of_changed_params = 0u;
    if (changes.channel_change)
    {
        ++number_of_changed_params;
    }

    if (changes.SNR_change)
    {
        ++number_of_changed_params;
    }

    const size_t total_size_of_msg =
        sizeof(SSID_CHANGED_message) + number_of_changed_params * sizeof(parameter_value_diff);

    SSID_CHANGED_message* param_change_msg = malloc(total_size_of_msg);

    TRACE_INFO("Param changed for %s msg sent. Msg size: %ld, number of changed params %d",
               from->ssid, total_size_of_msg - sizeof(param_change_msg->message_type), number_of_changed_params);

    param_change_msg->message_type = MSG_ID_SSID_CHANGED;
    param_change_msg->number_of_changed_parameters = number_of_changed_params;

    strncpy(param_change_msg->changed_ssid, from->ssid, MAX_SSID_LENGTH);
    param_change_msg->changed_ssid[MAX_SSID_LENGTH - 1] = 0;

    if (changes.channel_change)
    {
        --number_of_changed_params;
        strncpy(param_change_msg->changed_parameters[number_of_changed_params].param_name,
                "channel", MAX_PARAM_NAME_LENGTH);

        snprintf(param_change_msg->changed_parameters[number_of_changed_params].param_old_value,
                 MAX_PARAM_VALUE_LENGTH, "%u", from->channel);

        snprintf(param_change_msg->changed_parameters[number_of_changed_params].param_new_value,
                 MAX_PARAM_VALUE_LENGTH, "%u", to->channel);
    }

    if (changes.SNR_change)
    {
        --number_of_changed_params;
        strncpy(param_change_msg->changed_parameters[number_of_changed_params].param_name,
                "SNR", MAX_PARAM_NAME_LENGTH);

        snprintf(param_change_msg->changed_parameters[number_of_changed_params].param_old_value,
                 MAX_PARAM_VALUE_LENGTH, "%u", from->SNR);

        snprintf(param_change_msg->changed_parameters[number_of_changed_params].param_new_value,
                 MAX_PARAM_VALUE_LENGTH, "%u", to->SNR);
    }

    msgsnd(msg_queue_id, param_change_msg, (total_size_of_msg - sizeof(param_change_msg->message_type)), 0);

    free(param_change_msg);
}