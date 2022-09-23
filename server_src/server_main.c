#define _GNU_SOURCE // for sigabbrev_np

#include "access_points_data.h"
#include "communication.h"
#include "file_monitor.h"
#include "json_parser.h"

#include <trace.h>

#include <signal.h>
#include <stdlib.h>
#include <string.h>

static const char* monitored_file = NULL;

static void file_modified(void)
{
    TRACE_INFO("Handling of file modification.");

    access_point_map* new_map = parse_json_from_file(monitored_file);

    if (!new_map)
    {
        TRACE_DEBUG("Ignore modification as it seems it's not complete.");
        return;
    }

    access_point_map* current_map = get_current_access_points();

    // go through new_list and detect SSID addition or change of parameters
    for (size_t i = 0; i < map_size(new_map); ++i)
    {
        list_change change =
            detect_changed_ssid(current_map, &new_map[i].value);

        switch (change.change_type)
        {
        case NEW_SSID:
        {
            send_new_ssid(&new_map[i].value);
        }
        break;
        case PARAM_CHANGE:
        {
            // detect what parameters has changed between two access_point structs
            param_changes changed_params =
                detect_param_changes(&current_map[change.changed_ssid_index].value, &new_map[i].value);

            send_param_change(&current_map[change.changed_ssid_index].value, &new_map[i].value, changed_params);
        }
        break;

        case NO_CHANGE:
        default:
            // do nothing
            break;
        }
    }

    // go through current_list and detect SSID removal
    for (size_t i = 0; i < map_size(current_map); ++i)
    {
        if (find_ssid_in_map(new_map, current_map[i].value.ssid) == AP_MAP_NOT_FOUND)
        {
            send_removed_ssid(&current_map[i].value);
        }
    }

    // replace current list with set new list
    set_current_access_points(new_map);
}

static void file_deleted(void)
{
    TRACE_INFO("Handling of file deletion.");

    access_point_map* current_map = get_current_access_points();

    // notify about all ssid being removed
    for (size_t i = 0; i < map_size(current_map); ++i)
    {
        send_removed_ssid(&current_map[i].value);
    }
}

static void init_data(const char* file_path)
{
    access_point_map* parsed = parse_json_from_file(file_path);

    if (!parsed)
    {
        TRACE_ERROR("Initial JSON file parsing error.");
        exit(EXIT_FAILURE);
    }

    set_current_access_points(parsed);
}

static void cleanup_data()
{
    set_current_access_points(NULL);
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

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        TRACE_INFO("Missing path, usage: %s /file/path/name.json", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char* file_path = argv[1];

    init_data(file_path);
    init_communication();
    init_signal_handling();

    TRACE_INFO("Start monitoring file changes for %s", file_path);

    monitored_file = file_path;
    const file_monitor_callbacks cbs = {.file_modified = file_modified,
                                        .file_deleted = file_deleted};

    monitor_file(file_path, cbs);

    cleanup_communication();
    cleanup_data();
    return EXIT_SUCCESS;
}
