#define _GNU_SOURCE // for sigabbrev_np

#include "message_printer.h"

#include <msg_interface.h>
#include <trace.h>
#include <utils.h>

#include <czmq.h>

#include <errno.h>
#include <signal.h>
#include <string.h>

static zsock_t* sub_socket = NULL;

static void init_communication(void)
{
    assert(sub_socket == NULL);

    // create zmq 'subscribe' socket, for all AP_WATCH events
    sub_socket = zsock_new_sub("tcp://127.0.0.1" ":" ZMQ_PORT, AP_WATCH_IF);

    if (sub_socket == NULL)
    {
        TRACE_ERROR("ZMQ socket creation problem: %d (%s)", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    TRACE_INFO("Communication initalized.");
}

static void cleanup_communication(void)
{
    zsock_destroy(&sub_socket);

    TRACE_DEBUG("ZMQ socket destroyed.");
}

static void termination_handler(int signum)
{
    TRACE_INFO("Signal SIG%s (%d) received, cleanup.", sigabbrev_np(signum), signum);

    cleanup_communication();

    _Exit(EXIT_SUCCESS);
}
static void init_signal_handling(void)
{
    signal(SIGINT, termination_handler);
    signal(SIGTERM, termination_handler);
    signal(SIGQUIT, termination_handler);
}

static void receive_msg(void)
{
    assert(sub_socket);

    char* msg_id;
    zmsg_t* msg;
    if (zsock_recv(sub_socket, "sm", &msg_id, &msg) != 0)
    {
        TRACE_ERROR("ZMQ receive error: %d (%s)", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    TRACE_DEBUG("Received message: %s, number of frames: %lu, total size in bytes: %lu",
                msg_id, zmsg_size(msg), zmsg_content_size(msg));

    if (strcmp(msg_id, FULL_MSG_ID(AP_WATCH_IF, NEW_SSID_MSG)) == 0)
    {
        print_NEW_SSID_message(msg);
    }
    else if (strcmp(msg_id, FULL_MSG_ID(AP_WATCH_IF, REMOVED_SSID_MSG)) == 0)
    {
        print_REMOVED_SSID_message(msg);
    }
    else if (strcmp(msg_id, FULL_MSG_ID(AP_WATCH_IF, CHANGED_SSID_MSG)) == 0)
    {
        print_SSID_CHANGED_message(msg);
    }
    else
    {
        TRACE_ERROR("Unknown message with topic: %s", msg_id);
    }

    SAFE_FREE(msg_id);
    zmsg_destroy(&msg);
}

int main(int UNUSED(argc), char* UNUSED(argv[]))
{
    init_communication();
    init_signal_handling();

    // Currently infinite message loop, close with ctrl+c or kill
    for (;;)
    {
        TRACE_INFO("Start waiting for message.");
        receive_msg();
    }

    cleanup_communication();
    return 0;
}