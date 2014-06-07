/*
  CheckVersion.c

  cl CheckVersion.c
  (link) advapi32.lib
*/

#include "CheckVersion.h"

#pragma comment(lib, "advapi32.lib")

BOOL checkVersion(DWORD major, DWORD minor, DWORD spmajor)
{
  OSVERSIONINFO osvi;
  ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
  osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
  GetVersionEx(&osvi);
  if(osvi.dwMajorVersion > major) return TRUE;
  if(osvi.dwMajorVersion < major) return FALSE;
  if(osvi.dwMinorVersion > minor) return TRUE;
  if(osvi.dwMinorVersion < minor) return FALSE;
  if(spmajor && osvi.dwPlatformId == VER_PLATFORM_WIN32_NT){
    BOOL spCheck = FALSE;
    HKEY hkey;
    if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,
      "SYSTEM\\CurrentControlSet\\Control\\Windows",
      0, KEY_QUERY_VALUE, &hkey) == ERROR_SUCCESS){
      DWORD CSDVersion, sz = sizeof(DWORD); // sizeof(CSDVersion)
      if(RegQueryValueEx(hkey, "CSDVersion", NULL, NULL,
        (BYTE *)&CSDVersion, &sz) == ERROR_SUCCESS)
        spCheck = LOWORD(CSDVersion) >= spmajor;
      RegCloseKey(hkey);
    }
    return spCheck;
  }
  return TRUE;
}
