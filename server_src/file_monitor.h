#pragma once

#define FILE_MONITOR_GENERIC_FAILURE -1
#define FILE_MONITOR_SUCCESS 0

typedef void (*file_modified_t)(void);
typedef void (*file_deleted_t)(void);

typedef struct file_monitor_callbacks
{
    file_modified_t file_modified; //> called when file is modifed
    file_deleted_t file_deleted;   //> called when file is deleted
} file_monitor_callbacks;

// Starts monitoring file for changes. Currently function is blocking until file is deleted.
// @file_path path to file
// @callbacks called when file events occur
// @return negative on failure, FILE_MONITOR_SUCCESS otherwise
int monitor_file(const char* file_path, file_monitor_callbacks callbacks);