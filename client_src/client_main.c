#include "message_printer.h"

#include <ipc_interface.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <trace.h>

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

static void receive_msg(void)
{
    message received_msg;
    // receive any message in blocking manner

    const size_t max_size_of_payload = sizeof(received_msg) - sizeof(received_msg.message_type);
    ssize_t received =
        msgrcv(msg_queue_id, &received_msg, max_size_of_payload, 0, 0);

    if (received < 0)
    {
        TRACE_ERROR("msgrcv error.");
        exit(EXIT_FAILURE);
    }

    TRACE_DEBUG("Received message, type: %ld, payload size: %ld", received_msg.message_type, received);

    switch (received_msg.message_type)
    {
    case MSG_ID_NEW_SSID:
        print_NEW_SSID_message((NEW_SSID_message*)&received_msg);
        break;
    case MSG_ID_REMOVED_SSID:
        print_REMOVED_SSID_message((REMOVED_SSID_message*)&received_msg);
        break;
    case MSG_ID_SSID_CHANGED:
        print_SSID_CHANGED_message((SSID_CHANGED_message*)&received_msg);
        break;
    default:
        TRACE_ERROR("Unknown message with id: %ld", received_msg.message_type);
        break;
    }
}

int main(int /*argc*/, char* /*argv*/[])
{
    init_communication();

    // Infinite message loop
    for (;;)
    {
        TRACE_INFO("Start waiting for messages.");
        receive_msg();
    }

    return 0;
}