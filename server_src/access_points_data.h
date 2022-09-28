#pragma once

#include <stdbool.h>
#include <unistd.h>

#define SSID_MAX_LEN 32

#define AP_MAP_NOT_FOUND -1

// Single access points details
typedef struct access_point
{
    char ssid[SSID_MAX_LEN];
    unsigned int SNR;
    unsigned int channel;
} access_point;

// Hash map structure, where key is SSID as string
typedef struct access_point_map
{
    char* key; // ssid
    access_point value;
} access_point_map;

// Returns current access points.
access_point_map* get_current_access_points(void);

// Changes current map of access points. Deallocates previous one.
// @map new map
void set_current_access_points(access_point_map* map);

// Creates new access point map.
access_point_map* create_access_point_map(void);

// Deletes access point map.
void delete_access_point_map(access_point_map* map);

// Inserts new entry to the map or update value
// if given key (ssid) already exists.
// @map pointer to map, it's invalid after function call!
// @return pointer to possibly reallocated map.
access_point_map* add_to_map(access_point_map* map, const access_point* ap);

// Return number of elements in map.
size_t map_size(access_point_map* map);

////////////////////////////////////////////////////////
// List searching functions
////////////////////////////////////////////////////////

// Structure used as a result of detect_changed_ssid function
typedef struct list_change
{
    enum change_t
    {
        NO_CHANGE,
        NEW_SSID,
        PARAM_CHANGE
    } change_type;

    // valid only when change_type value is PARAM_CHANGE
    // and points to modifed entry in current_ssids map
    int changed_ssid_index;

} list_change;

// Structure used as a result of detect_param_changes function
typedef struct param_changes
{
    bool SNR_change;
    bool channel_change;
} param_changes;

// Checks whether given ssid exists in map
// @list map to search in
// @ssid ssid to search for
// @return index of found ssid entry, -1 if not found
int find_ssid_in_map(access_point_map* map, const char* ssid);

// Checks whether given ssid exists in map with the same parameters
// @current_ssids map to search in for change
// @after_change_ssid newly found AP

// @return NO_CHANGE when after_change_ssid exists with the same parameters.
//         NEW_SSID if after_change_ssid doesn't exist in current_ssids
//         PARAM_CHANGE if after_change_ssid exists with different parameters.
list_change detect_changed_ssid(access_point_map* current_ssids,
                                const access_point* after_change_ssid);

// Compares two access_points for parameter changes
// @return  param_changes structure represent difference,
//          each member is set to true when corresponding parameter values were different
param_changes detect_param_changes(const access_point* lhs, const access_point* rhs);