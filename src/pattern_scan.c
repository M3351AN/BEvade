#include "pattern_scan.h"
#include "ptr_calc.h"
#include "log.h"
#include <stddef.h>

be_targets g_targets;

static int hex_digit(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

typedef struct {
    uint8_t bytes[64];
    bool    masks[64];
    int     len;
} sig_t;

static bool sig_parse(const char* str, sig_t* out) {
    out->len = 0;
    int i = 0;
    while (str[i]) {
        char c = str[i];
        if (c == ' ') { i++; continue; }
        if (c == '?') {
            out->bytes[out->len] = 0;
            out->masks[out->len] = false;
            out->len++;
            i++;
            if (str[i] == '?') i++;
            continue;
        }
        int hi = hex_digit(c);
        int lo = hex_digit(str[i + 1]);
        if (hi < 0 || lo < 0) return false;
        out->bytes[out->len] = (uint8_t)((hi << 4) | lo);
        out->masks[out->len] = true;
        out->len++;
        i += 2;
    }
    return out->len > 0;
}

static uint8_t* find_pattern(uint8_t* base, size_t size, const sig_t* sig) {
    if (size < (size_t)sig->len) return NULL;
    size_t end = size - (size_t)sig->len;
    for (size_t i = 0; i <= end; i++) {
        bool match = true;
        for (int j = 0; j < sig->len; j++) {
            if (sig->masks[j] && base[i + j] != sig->bytes[j]) {
                match = false;
                break;
            }
        }
        if (match) return base + i;
    }
    return NULL;
}

static uint8_t* scan_one(const char* name, const char* pattern) {
    sig_t sig;
    if (!sig_parse(pattern, &sig)) {
        BE_LOG("[be] bad sig\n");
        return NULL;
    }
    HMODULE mod = GetModuleHandleA("GTA5_Enhanced.exe");
    if (!mod) {
        BE_LOG("[be] module not found\n");
        return NULL;
    }
    uint8_t* base = (uint8_t*)mod;
    IMAGE_DOS_HEADER* dos = (IMAGE_DOS_HEADER*)base;
    IMAGE_NT_HEADERS* nt = (IMAGE_NT_HEADERS*)(base + dos->e_lfanew);
    DWORD size = nt->OptionalHeader.SizeOfCode;
    uint8_t* found = find_pattern(base, size, &sig);
    if (!found) {
        BE_LOG("[be] not found: ");
        BE_LOG(name);
        BE_LOG("\n");
    }
    return found;
}

bool be_scan(void) {
    uint8_t* p;

    p = scan_one("be_data", "48 C7 05 ? ? ? ? 00 00 00 00 E8 ? ? ? ? 48 89 C1 E8 ? ? ? ? E8 ? ? ? ? BD 0A 00 00 00");
    if (!p) return false;
    {
        uintptr_t rip_target = ptr_rip((uintptr_t)p + 3);
        uintptr_t base = rip_target + 12;
        g_targets.restart_code = (int*)(base);
        g_targets.need_restart  = (bool*)(base + 8);
        g_targets.banned        = (bool*)(base + 12);
    }

    p = scan_one("be_status_patch", "80 B9 92 0A 00 00 01");
    if (!p) return false;
    g_targets.patch_addr = p;

    p = scan_one("ac_hash", "89 9E C8 00 00 00 48 8B 0D ? ? ? ? 48 85 C9 74 46");
    if (!p) return false;
    g_targets.hash_store = (Obf32**)ptr_rip((uintptr_t)p + 9);
    g_targets.get_hash   = (void*)ptr_rip((uintptr_t)p + 0x13);

    p = scan_one("ac_hash2", "89 9E E8 00 00 00 89 C2 E8 ? ? ? ? 69");
    if (!p) return false;
    g_targets.get_hash2 = (void*)ptr_rip((uintptr_t)p + 9);

    p = scan_one("ac_ctx", "48 8D BB 70 0A 00 00 4C 8D 35 ? ? ? ? 66 90");
    if (!p) return false;
    g_targets.ctx = (void**)ptr_rip((uintptr_t)p - 0xF);

    p = scan_one("skel_update", "56 48 83 EC 20 48 8B 81 40 01 00 00 48 85 C0");
    if (!p) return false;
    g_targets.skeleton_update = p;

    p = scan_one("metric", "48 89 F9 FF 50 20 48 8D 15");
    if (!p) return false;
    g_targets.metric = (void*)((uintptr_t)p - 0x26);

    p = scan_one("http_req", "56 57 48 83 EC 28 48 89 CE 8B 81 ? ? ? ? FF C8 83 F8 04 0F 87");
    if (!p) return false;
    g_targets.http_req = p;

    p = scan_one("player_join", "48 89 10 48 89 48 10 89 F9");
    if (!p) return false;
    {
        void** arr = (void**)ptr_rip((uintptr_t)p - 4);
        g_targets.player_join = arr[1];
    }

    BE_LOG("[be] patterns resolved\n");
    return true;
}
