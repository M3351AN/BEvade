#ifndef BE_LOG_H
#define BE_LOG_H

#ifndef NDEBUG
#include <windows.h>
#define BE_LOG(msg) OutputDebugStringA(msg)
#else
#define BE_LOG(msg) ((void)0)
#endif

#endif
