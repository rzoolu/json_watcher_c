#pragma once

#include "access_points_data.h"

typedef struct json_data_buffer
{
    const char* data;
    long size;
} json_data_buffer;

// Parse JSON file and returns access_point_map
// @file_path path to JSON file to be parsed
// @return populated access_point_map
access_point_map* parse_json_from_file(const char* file_path);

// Parse JSON file and returns access_point_map
// @json_data_buffer buffer with JSON to be parsed
// @return populated access_point_map
access_point_map* parse_json_from_buffer(const json_data_buffer json_buffer);
