#include "access_points_data.h"

#include <stdlib.h>
#include <trace.h>

#define STB_DS_IMPLEMENTATION
#include <stb/stb_ds.h>

access_point_map* CURRENT_AP_HASH_MAP = NULL;

access_point_map* get_current_access_points(void)
{
    return CURRENT_AP_HASH_MAP;
}

void set_current_access_points(access_point_map* map)
{
    shfree(CURRENT_AP_HASH_MAP);
    CURRENT_AP_HASH_MAP = map;
}

access_point_map* create_access_point_map(void)
{
    access_point_map* new_map = NULL;
    sh_new_strdup(new_map);

    return new_map;
}

void delete_access_point_map(access_point_map* map)
{
    shfree(map);
}

access_point_map* add_to_map(access_point_map* map, const access_point* ap)
{
    shput(map, ap->ssid, *ap);

    return map;
}

size_t map_size(access_point_map* map)
{
    return shlen(map);
}

int find_ssid_in_map(access_point_map* map, const char* ssid)
{
    return shgeti(map, ssid);
}

list_change detect_changed_ssid(access_point_map* current_ssids,
                                const access_point* after_change_ssid)

{
    int found = shgeti(current_ssids, after_change_ssid->ssid);
    if (found == AP_MAP_NOT_FOUND)
    {
        return (list_change){.change_type = NEW_SSID};
    }
    else if (current_ssids[found].value.SNR == after_change_ssid->SNR &&
             current_ssids[found].value.channel == after_change_ssid->channel)
    {
        return (list_change){.change_type = NO_CHANGE};
    }
    else
    {
        return (list_change){.change_type = PARAM_CHANGE, .changed_ssid_index = found};
    }
}

param_changes detect_param_changes(const access_point* lhs, const access_point* rhs)
{
    return (param_changes){.SNR_change = (lhs->SNR != rhs->SNR),
                           .channel_change = (lhs->channel != rhs->channel)};
}