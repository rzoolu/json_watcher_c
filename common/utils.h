#include <stdlib.h>
#include <trace.h>

#if defined(__GNUC__) || defined(__clang__)
#define LIKELY(expr) (__builtin_expect((expr), 1))
#define UNLIKELY(expr) (__builtin_expect((expr), 0))
#else
#define LIKELY(expr) (expr)
#define UNLIKELY(expr) (expr)
#endif

static inline void* safe_malloc(size_t size, const char* file, int line)
{
    void* ptr = calloc(1, size);
    if (UNLIKELY(ptr == NULL))
    {
        TRACE_ERROR("malloc failure at %s:%d", file, line);
        exit(EXIT_FAILURE);
    }

    return ptr;
}

// little bit strict (requires that free is not called on NULL), but helps to detect double free problem
static inline void safe_free(const void* ptr, const char* file, int line)
{
    if (UNLIKELY(ptr == NULL))
    {
        TRACE_ERROR("Redundant free or double free problem at: %s:%d", file, line);
        exit(EXIT_FAILURE);
    }

    free((void*)ptr);
}

// To improve safety and hide allocation failure handling code
// below macros can be used by user code instead of malloc/free
#define SAFE_MALLOC(size) safe_malloc(size, __FILE__, __LINE__)
#define SAFE_FREE(ptr)                  \
    safe_free(ptr, __FILE__, __LINE__); \
    ptr = NULL;
