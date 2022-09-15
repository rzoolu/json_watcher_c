#pragma once

#include "access_points_data.h"

// Parse JSON file and returns access_point_map
// @file_path path to JSON file to be parsed
// @return populated access_point_map
access_point_map* parse_json(const char* file_path);
