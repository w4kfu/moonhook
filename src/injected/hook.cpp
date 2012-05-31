#include "moonhook.h"

BYTE *SetupTrampo(HANDLE HFile, PCHAR NameFunc, PVOID Addr)
{
	BYTE *Trampo;
	DWORD Dest;

	Trampo = (BYTE*)VirtualAlloc(0, 0x50, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

	// PUSHA
	*Trampo = 0x60;
	// PUSH
	*(Trampo + 1) = 0x68;						// 0x1
	memcpy(Trampo + 2, &NameFunc, 4);			// 0x1
	*(Trampo + 6) = 0x68;						// 0x5
	memcpy(Trampo + 7, &HFile, 4);				// 0x6

	// CALL $ + 5
	*(Trampo + 11) = 0xE8;						// 0xA
	memset(Trampo + 12, 0, 4);					// 0xB

	// POP EAX
	*(Trampo + 16) = 0x58;						// 0xF

	// ADD EAX, 0xA								// 0x10
	*(Trampo + 17) = 0x83;
	*(Trampo + 18) = 0xC0;
	*(Trampo + 19) = 0x0A;

	// PUSH EAX									// 0x13
	*(Trampo + 20) = 0x50;

	// JMP 
	*(Trampo + 21) = 0xE9;
	Dest = (DWORD)((BYTE*)logFunction - (Trampo + 21) - 5);
	memcpy(Trampo + 22, &Dest, 4);

	// POP EAX * 2
	*(Trampo + 26) = 0x58;
	*(Trampo + 27) = 0x58;

	// POPA
	*(Trampo + 28) = 0x61;

	if (!strcmp(NameFunc, "__CreateWindowExW"))
	{
		*(Trampo + 29) = 0xEB;
		*(Trampo + 30) = 0xFE;
		*(Trampo + 31) = 0xE9;
		Dest = (DWORD)((BYTE*)Addr - (Trampo + 31) - 5);
		memcpy(Trampo + 32, &Dest, 4);
	}
	else
	{
		*(Trampo + 29) = 0xE9;
		Dest = (DWORD)((BYTE*)Addr - (Trampo + 29) - 5);
		memcpy(Trampo + 30, &Dest, 4);
	}

	return Trampo;
}