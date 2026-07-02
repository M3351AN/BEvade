cmake_minimum_required(VERSION 3.20)
project(BEvade LANGUAGES C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(BEVADE_SOURCES
    src/dllmain.c
    src/crt_stubs.c
    src/pattern_scan.c
    src/hooks.c
    src/bypass.c
)

set(MINHOOK_SOURCES
    minhook/src/buffer.c
    minhook/src/hook.c
    minhook/src/trampoline.c
    minhook/src/hde/hde64.c
)

add_library(BEvade SHARED ${BEVADE_SOURCES} ${MINHOOK_SOURCES})

target_include_directories(BEvade PRIVATE
    minhook/include
    minhook/src
    minhook/src/hde
    src
)

if(MSVC)
    target_compile_options(BEvade PRIVATE /Zl /GS- /O2)
    target_link_options(BEvade PRIVATE
        /NODEFAULTLIB
        /ENTRY:DllMain
        /SUBSYSTEM:WINDOWS
    )
else()
    target_compile_options(BEvade PRIVATE -O2 -ffreestanding -fno-stack-protector)
    target_link_options(BEvade PRIVATE
        -nostdlib
        -nodefaultlibs
        -Wl,-e,DllMain
        -Wl,--subsystem,windows
    )
endif()

target_link_libraries(BEvade PRIVATE kernel32)
