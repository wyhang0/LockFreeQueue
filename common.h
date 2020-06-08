#ifndef COMMON_H
#define COMMON_H

template
<typename T>
char IsPtr(T* ); // match pointer

int IsPtr(...);  // match non-pointer
#define ISPTR(p) (sizeof(IsPtr(p)) == sizeof(char))

#define LOCKFREEQUEUE LockFreeQueue



#endif // COMMON_H
