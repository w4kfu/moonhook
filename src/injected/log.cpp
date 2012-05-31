#include "log.h"

HANDLE OpenLog(char *filename)
{
	HANDLE Hfile;

	Hfile = CreateFileA(filename, GENERIC_WRITE, 0, 0, CREATE_ALWAYS , 
		FILE_ATTRIBUTE_NORMAL, NULL);
	//TODO Add messagebox Error ?
	if (Hfile == INVALID_HANDLE_VALUE)
		exit(EXIT_FAILURE);
	return Hfile;
}

void logFunction(HANDLE HFile, PCHAR FuncName)
{
	char msg[10000];

    //if (IsBadReadPtr(FuncName, 1))
	//	sprintf(msg, "[PID : %X][TID : %X], %s()", GetCurrentProcessId(), GetCurrentThreadId(), "Ordinal ??");
	//else
	sprintf(msg, "[PID : %X][TID : %X], %s()", GetCurrentProcessId(), GetCurrentThreadId(), FuncName);
	logMessage(HFile, msg);

}

void logMessage(HANDLE hFile, char *msg)
{
	DWORD written;

	WriteFile(hFile, msg, strlen(msg), &written, 0);
	WriteFile(hFile, "\r\n", strlen("\r\n"), &written, 0);
}

void wlogMessage(HANDLE hFile, LPWSTR wmsg)
{
	char msg[1000];
	DWORD written;

	wcstombs(msg, wmsg, 1000);
	WriteFile(hFile, msg, strlen(msg), &written, 0);
	WriteFile(hFile, "\r\n", strlen("\r\n"), &written, 0);
}