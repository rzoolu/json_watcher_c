#include "communication.h"

#include <trace.h>

#include <czmq.h>

#include <errno.h>
#include <string.h>

static zsock_t* pub_socket = NULL;

void init_communication(void)
{
    assert(pub_socket == NULL);

    // create zmq 'publish' socket
    pub_socket = zsock_new_pub("tcp://*:4559");

    if (pub_socket == NULL)
    {
        TRACE_ERROR("ZMQ socket creation problem: %d (%s)", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    TRACE_INFO("Communication initalized.");
}

void cleanup_communication(void)
{
    zsock_destroy(&pub_socket);

    TRACE_DEBUG("ZMQ socket destroyed.");
}

void send_new_ssid(const access_point* new_ssid)
{
    assert(new_ssid);
    assert(pub_socket);

    TRACE_INFO("New SSID for %s msg sent.", new_ssid->ssid);

    if (zsock_send(pub_socket, "sss4s4", "AP_WATCH/NEW_AP", new_ssid->ssid,
                   "SNR", new_ssid->SNR,
                   "channel", new_ssid->channel) != 0)
    {
        TRACE_ERROR("ZMQ sending error: %d (%s)", errno, strerror(errno));
    }
}

void send_removed_ssid(const access_point* old_ssid)
{
    assert(old_ssid);
    assert(pub_socket);

    TRACE_INFO("Removed SSID %s msg sent.", old_ssid->ssid);

    if (zsock_send(pub_socket, "ss", "AP_WATCH/REMOVED_AP", old_ssid->ssid) != 0)
    {
        TRACE_ERROR("ZMQ sending error: %d (%s)", errno, strerror(errno));
    }
}

void send_param_change(const access_point* from, const access_point* to, const param_changes changes)
{
    // TODO:
    (void)changes;

    assert(from);
    assert(to);
    assert(pub_socket);

    TRACE_INFO("Param changed for %s msg sent.", from->ssid);

    if (zsock_send(pub_socket, "sss44s44", "AP_WATCH/CHANGED_AP", from->ssid,
                   "SNR", from->SNR, to->SNR,
                   "channel", from->channel, to->channel) != 0)
    {
        TRACE_ERROR("ZMQ sending error: %d (%s)", errno, strerror(errno));
    }
}
