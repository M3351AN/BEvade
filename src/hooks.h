#ifndef BE_HOOKS_H
#define BE_HOOKS_H

#include <stdint.h>
#include <stdbool.h>
#include <windows.h>

typedef uint32_t (*get_hash_t)(void);
typedef uint32_t (*get_hash2_t)(void*, uint32_t);
typedef void     (*skel_update_t)(void*, int);
typedef bool     (*metric_t)(void*, void*, void*, void*);
typedef void     (*http_req_t)(void*);
typedef BOOL     (WINAPI *get_thread_ctx_t)(HANDLE, LPCONTEXT);
typedef bool     (*player_join_t)(void*, void*);

extern get_hash2_t    g_orig_hash2;
extern http_req_t     g_orig_http;
extern get_thread_ctx_t g_orig_thread_ctx;
extern player_join_t  g_orig_join;

bool be_hooks_install(void);
void be_hooks_remove(void);

#endif
