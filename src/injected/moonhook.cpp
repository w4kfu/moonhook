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
	HANDLE HFile;

	// TODO Set LogFilename to ModuleFileName
	HFile = OpenLog("log.txt");

	// Debug :)
	//__asm jmp $

	ModuleList = GetModuleList();

	logMessage(HFile, "[+] GetModuleList");

	for (std::list<PLDR_MODULE>::iterator it = ModuleList.begin();
		it != ModuleList.end(); it++)
	{
		pLM = *it;
		wlogMessage(HFile, pLM->BaseDllName);
	}
	CloseHandle(HFile);
}

extern "C"
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{

	if (fdwReason == DLL_PROCESS_DETACH)
		return (TRUE);
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		 // Usefull or not ?
		//GetSeDebugPrivilege();
		MoonHook();
	}
	return TRUE;
}
