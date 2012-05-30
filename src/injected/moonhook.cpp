#include "moonhook.h"

BOOL GetSeDebugPrivilege(void)
{
    HANDLE htok;
    TOKEN_PRIVILEGES tp;
    LUID luidp;
    BOOL retval;

    if (!OpenProcessToken(GetCurrentProcess(),
	  TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &htok))
      return 0;

    if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luidp))
    {
      CloseHandle(htok);
      return 0;
    }

    tp.PrivilegeCount = 1;
    tp.Privileges[0].Luid = luidp;
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    retval = AdjustTokenPrivileges(htok, FALSE, &tp, 0, NULL, NULL);

    CloseHandle(htok);
    return retval;
}

extern "C"
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
  // Usefull or not ?
  GetSeDebugPrivilege();
  return 0;
}
