#include <stdio.h>

#ifdef DEBUG
#define TRACE_DEBUG(fmt, ...)                             \
    do                                                    \
    {                                                     \
        fprintf(stderr, "DBG %s:%d: " fmt "\n", __FILE__, \
                __LINE__, ##__VA_ARGS__);                 \
    } while (0)
#else
#define TRACE_DEBUG(...)
#endif

#define TRACE_INFO(fmt, ...)                              \
    do                                                    \
    {                                                     \
        fprintf(stderr, "INF %s:%d: " fmt "\n", __FILE__, \
                __LINE__, ##__VA_ARGS__);                 \
    } while (0)

#define TRACE_ERROR(fmt, ...)                             \
    do                                                    \
    {                                                     \
        fprintf(stderr, "ERR %s:%d: " fmt "\n", __FILE__, \
                __LINE__, ##__VA_ARGS__);                 \
    } while (0)
