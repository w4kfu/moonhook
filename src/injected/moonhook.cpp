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
		PatchExports(HFile, pLM);
	}
	CloseHandle(HFile);
}

void PatchExports(HANDLE HFile, PLDR_MODULE Module)
{
    PIMAGE_DOS_HEADER           pIDH;
    PIMAGE_NT_HEADERS           pINTH;
	PIMAGE_EXPORT_DIRECTORY		pIED;
    DWORD                       dwExportTableOffset;
	PSTR						*pNames;
	PSTR						pName;
    wchar_t						pwszModule[MAX_PATH];
	DWORD i;
    unsigned long				*pchName;
	unsigned long				*pchAddr;
	PUSHORT						pchOrdinal  = NULL;
	DWORD						Addr;
	char						sAddr[0x10];
	DWORD						OldProtect;

	pIDH = (PIMAGE_DOS_HEADER) Module->BaseAddress;

    if (IsBadReadPtr(Module->BaseAddress, sizeof(IMAGE_DOS_HEADER)))
        return;

    pINTH = (PIMAGE_NT_HEADERS)((BYTE*)Module->BaseAddress + pIDH->e_lfanew);

    dwExportTableOffset = pINTH->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;

    if (dwExportTableOffset == 0)
        return;

	pIED = (PIMAGE_EXPORT_DIRECTORY)((BYTE*)Module->BaseAddress + dwExportTableOffset);
	pNames = (PSTR*)((BYTE*)Module->BaseAddress + pIED->AddressOfNames);

	
    pchName = (unsigned long *)((BYTE*)Module->BaseAddress + pIED->AddressOfNames);
	pchAddr = (unsigned long *)((BYTE*)Module->BaseAddress + pIED->AddressOfFunctions);
	pchOrdinal = (PUSHORT)((BYTE*)Module->BaseAddress + pIED->AddressOfNameOrdinals);

    for (i = 0; i < pIED->NumberOfNames; ++i)
    {
			ULONG ord = pchOrdinal[i];
			pName = (char *)((BYTE*)Module->BaseAddress + pchName[i]);
			logMessage(HFile, pName);
			Addr = (DWORD)((BYTE*)Module->BaseAddress + pchAddr[ord]);
			sprintf(sAddr, "%08X", Addr);
			logMessage(HFile, sAddr);
			if (!strcmp(pName, "MessageBoxA"))
			{
				MessageBoxA(NULL, "TEST", "TEST", 0);
				__asm jmp $
				VirtualProtect(&pchAddr[ord], sizeof (DWORD), PAGE_READWRITE, &OldProtect);
				memset(&pchAddr[ord], 0, sizeof (DWORD));
				VirtualProtect(&pchAddr[ord], sizeof (DWORD), OldProtect, &OldProtect);
			}
	}
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
		RemoveModuleFromPEB(TEXT(MOONHOOK));
		MoonHook();
	}
	return TRUE;
}
