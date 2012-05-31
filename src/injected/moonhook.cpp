#include "moonhook.h"

// FIXME clean this shit !!!
HANDLE HFilee;
char ordString[32];

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
	wchar_t ModuleName[MAX_PATH];
	HANDLE HFile;

	// TODO Set LogFilename to ModuleFileName
	HFile = OpenLog("log.txt");

	HFilee = HFile;

	// Debug :)
	//__asm jmp $

	//RemoveModuleFromPEB(TEXT(MOONHOOK));

	ModuleList = GetModuleList();

	GetModuleFileNameW(NULL, ModuleName, MAX_PATH);
	
	logMessage(HFile, "[+] GetModuleList");

	for (std::list<PLDR_MODULE>::iterator it = ModuleList.begin();
		it != ModuleList.end(); it++)
	{
		pLM = *it;
		wlogMessage(HFile, pLM->BaseDllName);
		//PatchExports(HFile, pLM);
		if (!wcscmp(ModuleName, pLM->BaseDllName))
		{
			PatchImports(HFile, pLM);
		}
	}
	logMessage(HFile, "\r\n");
}

void PatchImports(HANDLE HFile, PLDR_MODULE Module)
{
    PIMAGE_DOS_HEADER           pIDH;
    PIMAGE_NT_HEADERS           pINTH;
    PIMAGE_IMPORT_DESCRIPTOR    pIID;
    DWORD                       dwTemp;
    DWORD                       dwImportTableOffset;
    DWORD                       dwOldProtect;

	pIDH = (PIMAGE_DOS_HEADER) Module->BaseAddress;

    if (IsBadReadPtr(Module->BaseAddress, sizeof(IMAGE_DOS_HEADER)))
        return;

    pINTH = (PIMAGE_NT_HEADERS)((BYTE*)Module->BaseAddress + pIDH->e_lfanew);

	dwImportTableOffset = pINTH->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;

    if (dwImportTableOffset == 0)
        return;

	pIID = (PIMAGE_IMPORT_DESCRIPTOR)((BYTE*)Module->BaseAddress + dwImportTableOffset);

    while (TRUE)
    {
        LPSTR                   pszModule   = NULL;
        PIMAGE_THUNK_DATA       pITDA       = NULL;
        PIMAGE_THUNK_DATA       pIINA       = NULL;
        wchar_t pwszModule[MAX_PATH];
        
        if (pIID->FirstThunk == 0 || pIID->OriginalFirstThunk == 0)
        {
            break;
        }

        pszModule = (LPSTR)((BYTE*)Module->BaseAddress + pIID->Name);
        swprintf(pwszModule, L"%S", pszModule);

        pITDA = (PIMAGE_THUNK_DATA)((BYTE*)Module->BaseAddress + (DWORD)pIID->FirstThunk);

        pIINA = (PIMAGE_THUNK_DATA)((BYTE*)Module->BaseAddress + (DWORD)pIID->OriginalFirstThunk);

        while (pITDA->u1.Ordinal != 0)
        {
			PVOID   pfnOld;
            PVOID   pfnNew;

            pfnOld = (PVOID)pITDA->u1.Function;

            if (pIINA)
            {
					LPSTR fnName = NULL;
                    bool exportedByOrdinal = false;

                    if (!IMAGE_SNAP_BY_ORDINAL(pIINA->u1.Ordinal))
                    {
                        PIMAGE_IMPORT_BY_NAME pIIN = (PIMAGE_IMPORT_BY_NAME)((BYTE*)Module->BaseAddress + pIINA->u1.AddressOfData);
                        fnName = (LPSTR)pIIN->Name;
                    }
                    else
                    {
                        sprintf(ordString, "Ord%x", pIINA->u1.Ordinal);
                        fnName = ordString;
                        exportedByOrdinal = true;
                    }
					if (fnName[0] != '_')
					{

						if (VirtualProtect(&pITDA->u1.Function, sizeof(DWORD), PAGE_READWRITE, &dwOldProtect))
						{
							//CHAR wut[100];
							//sprintf(wut, "%08X", pITDA->u1.Function);
							//logMessage(HFile, wut);
							pITDA->u1.Function = (DWORD)SetupTrampo(HFile, fnName, (PVOID)pITDA->u1.Function);
							VirtualProtect(&pITDA->u1.Function, sizeof(DWORD), dwOldProtect, &dwOldProtect);
						}
						logMessage(HFile, fnName);
					}
					
			}
            pITDA++;
            pIINA++;
        }

        pIID++;
    }
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

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{

	if (fdwReason == DLL_PROCESS_DETACH)
	{
		//CloseHandle(HFilee);
		return TRUE;
	}
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		//DisableThreadLibraryCalls(GetModuleHandleA(MOONHOOK));
		 // Usefull or not ?
		//GetSeDebugPrivilege();
		MoonHook();
	}
	return TRUE;
}
