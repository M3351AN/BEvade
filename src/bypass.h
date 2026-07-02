#ifndef BE_BYPASS_H
#define BE_BYPASS_H

#include <stdint.h>
#include <stdbool.h>
#include <windows.h>

extern volatile bool g_running;
extern bool g_be_active;
extern bool g_fsl;
extern bool g_fsl_bypass;

DWORD WINAPI be_thread(void* param);

#endif
