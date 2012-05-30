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
