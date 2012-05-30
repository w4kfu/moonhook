#include "moonhook.h"

inline PTEB getCurrentTeb(void)
{
	PTEB retval;

	__asm
	{
		mov eax, fs:[0018h]
		mov retval, eax
	}
	return retval;
}

std::list<PLDR_MODULE> GetModuleList(void)
{
    PTEB pTEB;
    PPEB_LDR_DATA pLDR;
    PLIST_ENTRY pMark, pEntry;
    PLDR_MODULE pLM;
    std::list<PLDR_MODULE> ModuleList(0);

    pTEB = getCurrentTeb();

	pLDR = pTEB->Peb->LoaderData;

    pMark = &(pLDR->InMemoryOrderModuleList);

    for(pEntry = pMark->Flink; pEntry != pMark; pEntry = pEntry->Flink)
    {
        pLM = CONTAINING_RECORD(pEntry, LDR_MODULE, InMemoryOrderModuleList);
        ModuleList.push_back(pLM);
    }
    return ModuleList;
}

void RemoveModuleFromPEB(LPWSTR DllName)
{ 
    PTEB pTEB; 
    PPEB_LDR_DATA pLDR; 
    PLIST_ENTRY pMark, pEntry; 
    PLDR_MODULE pLM;
	LPWSTR lptr;

    pTEB = getCurrentTeb();

    pLDR = pTEB->Peb->LoaderData;

    pMark = &(pLDR->InMemoryOrderModuleList); 

    for(pEntry = pMark->Flink; pEntry != pMark; pEntry = pEntry->Flink) 
    { 
        pLM = CONTAINING_RECORD(pEntry, LDR_MODULE, InMemoryOrderModuleList);
		lptr = wcsrchr(pLM->BaseDllName, '\\');
		if (lptr)
		{
			if (!wcscmp(DllName, lptr + 1))
			{
				pEntry->Blink->Flink = pEntry->Flink;
				pEntry->Flink->Blink = pEntry->Blink;
			}
		}
    } 

    pMark = &(pLDR->InLoadOrderModuleList); 

    for(pEntry = pMark->Flink; pEntry != pMark; pEntry = pEntry->Flink) 
    { 
        pLM = CONTAINING_RECORD(pEntry, LDR_MODULE, InLoadOrderModuleList);
		lptr = wcsrchr(pLM->BaseDllName, '\\');
		if (lptr)
		{
			if (!wcscmp(DllName, lptr + 1))
			{
				pEntry->Blink->Flink = pEntry->Flink; 
				pEntry->Flink->Blink = pEntry->Blink;
			}
        } 
    } 

    pMark = &(pLDR->InInitializationOrderModuleList); 

    for(pEntry = pMark->Flink; pEntry != pMark; pEntry = pEntry->Flink) 
    { 
        pLM = CONTAINING_RECORD(pEntry, LDR_MODULE, InInitializationOrderModuleList); 
		lptr = wcsrchr(pLM->BaseDllName, '\\');
		if (lptr)
		{
			if (!wcscmp(DllName, lptr + 1))
			{
				pEntry->Blink->Flink = pEntry->Flink; 
				pEntry->Flink->Blink = pEntry->Blink;
			}
        } 
    } 
}  