#include "access_points_data.h"
#include "communication.h"
#include "file_monitor.h"
#include "json_parser.h"

#include <trace.h>

#include <stb/stb_ds.h>
#include <stdlib.h>

#define ACCESS_POINTS_PATH "./access_points.json"

static void file_modified(void)
{
    TRACE_INFO("Handling of file modification.");

    access_point_map* new_map = parse_json(ACCESS_POINTS_PATH);

    if (!new_map)
    {
        TRACE_DEBUG("Ignore modification as it seems it's not complete.");
        return;
    }

    access_point_map* current_map = get_current_access_points();

    // go through new_list and detect SSID addition or change of parameters
    for (int i = 0; i < shlen(new_map); ++i)
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
    for (int i = 0; i < shlen(current_map); ++i)
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

    // TODO: notify about all ssid being removed?
}

static void init_data(void)
{
    access_point_map* parsed = parse_json(ACCESS_POINTS_PATH);

    if (!parsed)
    {
        TRACE_ERROR("Initial JSON file parsing error.");
        exit(EXIT_FAILURE);
    }

    set_current_access_points(parsed);
}

int main(int /*argc*/, char* /*argv*/[])
{
    init_data();
    init_communication();

    TRACE_INFO("Start monitoring file changes for %s", ACCESS_POINTS_PATH);

    const file_monitor_callbacks cbs = {.file_modified = file_modified,
                                        .file_deleted = file_deleted};

    monitor_file(ACCESS_POINTS_PATH, cbs);

    close_communication();

    return 0;
}
