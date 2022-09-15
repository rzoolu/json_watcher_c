#include "file_monitor.h"
#include <stdbool.h>
#include <sys/inotify.h>
#include <trace.h>
#include <unistd.h>

// waits for file modification events and calls callbacks until file exist
static int get_file_events(int inotify_fd, file_monitor_callbacks callbacks)
{

    bool file_exists = true;
    while (file_exists)
    {
        char event_buf[1024]
            __attribute__((aligned(__alignof__(struct inotify_event))));

        ssize_t length = read(inotify_fd, event_buf, sizeof(event_buf));
        if (length < 0)
        {
            TRACE_ERROR("Cannot read inotify descriptor.");
            return FILE_MONITOR_GENERIC_FAILURE;
        }

        const struct inotify_event* event = NULL;

        for (char* ptr = event_buf; ptr < event_buf + length;
             ptr += sizeof(struct inotify_event) + event->len)
        {
            event = (const struct inotify_event*)ptr;

            if (event->mask & IN_MODIFY)
            {
                TRACE_INFO("The file was modified.");

                // FILE* file = fopen("./access_points.json", "rb");
                // fseek(file, 0, SEEK_END);
                // const long file_size = ftell(file);
                // fseek(file, 0, SEEK_SET);

                // char* file_data = malloc(file_size + 1);
                // int number_of_bytes = fread(file_data, 1, file_size, file);
                // file_data[number_of_bytes] = 0;

                // TRACE_INFO("File content: %s", file_data);
                // free(file_data);

                if (callbacks.file_modified)
                {
                    callbacks.file_modified();
                }
            }
            else if (event->mask & IN_DELETE_SELF || event->mask & IN_MOVE_SELF)
            {
                TRACE_INFO("The file was deleted. Stop monitoring file.");

                if (callbacks.file_deleted)
                {
                    callbacks.file_deleted();
                }

                file_exists = false;
                break;
            }
            else
            {
                TRACE_DEBUG("Other event on monitored file: %u", event->mask);
            }
        }
    }

    return FILE_MONITOR_SUCCESS;
}

int monitor_file(const char* file_path, file_monitor_callbacks callbacks)
{

    int inotify_fd = inotify_init();

    if (inotify_fd < 0)
    {
        TRACE_ERROR("Cannot create inotify instance.");
        return FILE_MONITOR_GENERIC_FAILURE;
    }

    int file_watch_desc = inotify_add_watch(inotify_fd, file_path,
                                            IN_MODIFY | IN_DELETE_SELF | IN_MOVE_SELF);

    if (file_watch_desc < 0)
    {
        TRACE_ERROR("Cannot add inotify watch for %s", file_path);
        return FILE_MONITOR_GENERIC_FAILURE;
    }

    int result = get_file_events(inotify_fd, callbacks);

    inotify_rm_watch(inotify_fd, file_watch_desc);
    close(inotify_fd);

    return result;
}