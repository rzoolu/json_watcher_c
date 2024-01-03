#include <stdio.h>

#ifdef DEBUG
#define TRACE_DEBUG(fmt, ...) \
    fprintf(stderr, "DBG %s:%d: " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define TRACE_DEBUG(...)
#endif

#define TRACE_INFO(fmt, ...) \
    fprintf(stderr, "INF %s:%d: " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#define TRACE_ERROR(fmt, ...) \
    fprintf(stderr, "ERR %s:%d: " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)
