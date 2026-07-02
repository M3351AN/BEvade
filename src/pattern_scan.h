#ifndef BE_SCAN_H
#define BE_SCAN_H

#include <stdint.h>
#include <stdbool.h>
#include <windows.h>
#include "types.h"

typedef struct be_targets {
    int*    restart_code;
    bool*   need_restart;
    bool*   banned;

    void*   patch_addr;

    Obf32** hash_store;
    void*   get_hash;
    void*   get_hash2;

    void**  ctx;

    void*   skeleton_update;
    void*   metric;
    void*   http_req;
    void*   player_join;
} be_targets;

extern be_targets g_targets;

bool be_scan(void);

#endif
