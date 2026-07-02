#ifdef _MSC_VER

#pragma comment(linker, "/EXPORT:GetFileVersionInfoA=\\.\\GLOBALROOT\\SystemRoot\\System32\\version.dll.GetFileVersionInfoA")
#pragma comment(linker, "/EXPORT:GetFileVersionInfoByHandle=\\.\\GLOBALROOT\\SystemRoot\\System32\\version.dll.GetFileVersionInfoByHandle")
#pragma comment(linker, "/EXPORT:GetFileVersionInfoExA=\\.\\GLOBALROOT\\SystemRoot\\System32\\version.dll.GetFileVersionInfoExA")
#pragma comment(linker, "/EXPORT:GetFileVersionInfoExW=\\.\\GLOBALROOT\\SystemRoot\\System32\\version.dll.GetFileVersionInfoExW")
#pragma comment(linker, "/EXPORT:GetFileVersionInfoSizeA=\\.\\GLOBALROOT\\SystemRoot\\System32\\version.dll.GetFileVersionInfoSizeA")
#pragma comment(linker, "/EXPORT:GetFileVersionInfoSizeExA=\\.\\GLOBALROOT\\SystemRoot\\System32\\version.dll.GetFileVersionInfoSizeExA")
#pragma comment(linker, "/EXPORT:GetFileVersionInfoSizeExW=\\.\\GLOBALROOT\\SystemRoot\\System32\\version.dll.GetFileVersionInfoSizeExW")
#pragma comment(linker, "/EXPORT:GetFileVersionInfoSizeW=\\.\\GLOBALROOT\\SystemRoot\\System32\\version.dll.GetFileVersionInfoSizeW")
#pragma comment(linker, "/EXPORT:GetFileVersionInfoW=\\.\\GLOBALROOT\\SystemRoot\\System32\\version.dll.GetFileVersionInfoW")
#pragma comment(linker, "/EXPORT:VerFindFileA=\\.\\GLOBALROOT\\SystemRoot\\System32\\version.dll.VerFindFileA")
#pragma comment(linker, "/EXPORT:VerFindFileW=\\.\\GLOBALROOT\\SystemRoot\\System32\\version.dll.VerFindFileW")
#pragma comment(linker, "/EXPORT:VerInstallFileA=\\.\\GLOBALROOT\\SystemRoot\\System32\\version.dll.VerInstallFileA")
#pragma comment(linker, "/EXPORT:VerInstallFileW=\\.\\GLOBALROOT\\SystemRoot\\System32\\version.dll.VerInstallFileW")
#pragma comment(linker, "/EXPORT:VerLanguageNameA=\\.\\GLOBALROOT\\SystemRoot\\System32\\version.dll.VerLanguageNameA")
#pragma comment(linker, "/EXPORT:VerLanguageNameW=\\.\\GLOBALROOT\\SystemRoot\\System32\\version.dll.VerLanguageNameW")
#pragma comment(linker, "/EXPORT:VerQueryValueA=\\.\\GLOBALROOT\\SystemRoot\\System32\\version.dll.VerQueryValueA")
#pragma comment(linker, "/EXPORT:VerQueryValueW=\\.\\GLOBALROOT\\SystemRoot\\System32\\version.dll.VerQueryValueW")

#else

#define BE_FORWARD(name) \
    __attribute__((section(".drectve"))) static const char be_fwd_##name[] = \
        " /EXPORT:" #name "=\\\\.\\GLOBALROOT\\SystemRoot\\System32\\version.dll." #name

BE_FORWARD(GetFileVersionInfoA);
BE_FORWARD(GetFileVersionInfoByHandle);
BE_FORWARD(GetFileVersionInfoExA);
BE_FORWARD(GetFileVersionInfoExW);
BE_FORWARD(GetFileVersionInfoSizeA);
BE_FORWARD(GetFileVersionInfoSizeExA);
BE_FORWARD(GetFileVersionInfoSizeExW);
BE_FORWARD(GetFileVersionInfoSizeW);
BE_FORWARD(GetFileVersionInfoW);
BE_FORWARD(VerFindFileA);
BE_FORWARD(VerFindFileW);
BE_FORWARD(VerInstallFileA);
BE_FORWARD(VerInstallFileW);
BE_FORWARD(VerLanguageNameA);
BE_FORWARD(VerLanguageNameW);
BE_FORWARD(VerQueryValueA);
BE_FORWARD(VerQueryValueW);

#undef BE_FORWARD

#endif
