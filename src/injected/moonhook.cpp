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

void MoonHook(void)
{
	std::list<PLDR_MODULE> ModuleList(0);
	PLDR_MODULE pLM;
	FILE *fp;

	// TODO Set LogFilename to ModuleFileName
	fp = fopen("log.txt", "w");

	ModuleList = GetModuleList();

	fwrite("[+] GetModuleList\n", strlen("[+] GetModuleList\n"), 1, fp);

	for (std::list<PLDR_MODULE>::iterator it = ModuleList.begin();
		it != ModuleList.end(); it++)
	{
		pLM = *it;
		fwrite(pLM->BaseDllName, wcslen(pLM->BaseDllName), 1, fp);
		fwrite("\n", 1, 1, fp);
	}
	fclose(fp);
}

extern "C"
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
  // Usefull or not ?
  GetSeDebugPrivilege();
  MoonHook();
  return 0;
}
