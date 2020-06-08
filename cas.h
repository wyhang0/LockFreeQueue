#ifndef CAS_H
#define CAS_H

#ifdef __GNUC__
    #define ATOMIC_ADD(x, y) __sync_fetch_and_add(x, y)
    #define ATOMIC_SUB(x, y) __sync_fetch_and_sub(x, y)
    #define CAS(ptr, old, new) __sync_bool_compare_and_swap(ptr, old, new)
#elif defined _WIN32

#elif defined _WIN64

#endif

#endif // CAS_H
