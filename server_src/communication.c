#include "communication.h"

#include <msg_interface.h>
#include <trace.h>

#include <czmq.h>

#include <errno.h>
#include <string.h>

static zsock_t* pub_socket = NULL;

void init_communication(void)
{
    assert(pub_socket == NULL);

    // create zmq 'publish' socket
    pub_socket = zsock_new_pub("tcp://*" ":" ZMQ_PORT);

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

static void send_msg(const char *full_msg_id, const zmsg_t *msg_body)
{
    if (zsock_send(pub_socket, MSG_PICTURE, full_msg_id, msg_body) != 0)
    {
        TRACE_ERROR("ZMQ sending %s error: %d (%s)", full_msg_id, errno, strerror(errno));
    }
}

void send_new_ssid(const access_point* new_ssid)
{
    assert(new_ssid);
    assert(pub_socket);

    TRACE_INFO("New SSID for %s msg sent.", new_ssid->ssid);

    zmsg_t *new_ap_msg_body = zmsg_new();
    const unsigned int number_of_params = 2;

    zmsg_addstr(new_ap_msg_body, new_ssid->ssid);
    
    zmsg_addstrf(new_ap_msg_body, "%u", number_of_params);

    zmsg_addstr(new_ap_msg_body, "SNR");
    zmsg_addstrf(new_ap_msg_body, "%u", new_ssid->SNR);

    zmsg_addstr(new_ap_msg_body, "channel");
    zmsg_addstrf(new_ap_msg_body, "%u", new_ssid->channel);

    send_msg(FULL_MSG_ID(AP_WATCH_IF, NEW_SSID_MSG), new_ap_msg_body);

    zmsg_destroy(&new_ap_msg_body);

}

void send_removed_ssid(const access_point* old_ssid)
{
    assert(old_ssid);
    assert(pub_socket);

    TRACE_INFO("Removed SSID %s msg sent.", old_ssid->ssid);

    zmsg_t *removed_ap_msg_body = zmsg_new();
    zmsg_addstr(removed_ap_msg_body, old_ssid->ssid);

    send_msg(FULL_MSG_ID(AP_WATCH_IF, REMOVED_SSID_MSG), removed_ap_msg_body);

    zmsg_destroy(&removed_ap_msg_body);

}

void send_param_change(const access_point* from, const access_point* to, const param_changes changes)
{
    assert(from);
    assert(to);
    assert(pub_socket);

    TRACE_INFO("Param changed for %s msg sent.", from->ssid);

    zmsg_t *changed_ap_msg_body = zmsg_new();
    zmsg_addstr(changed_ap_msg_body, from->ssid);

    unsigned int number_of_params = 0;
    if (changes.SNR_change)
    {
        ++number_of_params;
    }

    if (changes.channel_change)
    {
        ++number_of_params;
    }
    zmsg_addstrf(changed_ap_msg_body, "%u", number_of_params);

    if (changes.SNR_change)
    {
        zmsg_addstr(changed_ap_msg_body, "SNR");
        zmsg_addstrf(changed_ap_msg_body, "%u", from->SNR);
        zmsg_addstrf(changed_ap_msg_body, "%u", to->SNR);
    }

    if (changes.channel_change)
    {
        zmsg_addstr(changed_ap_msg_body, "channel");
        zmsg_addstrf(changed_ap_msg_body, "%u", from->channel);
        zmsg_addstrf(changed_ap_msg_body, "%u", to->SNR);
    }

    send_msg(FULL_MSG_ID(AP_WATCH_IF, CHANGED_SSID_MSG), changed_ap_msg_body);

    zmsg_destroy(&changed_ap_msg_body);
}
