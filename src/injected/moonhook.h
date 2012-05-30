#ifndef MOONHOOK_H_
# define MOONHOOK_H_

# include <list>
# include <windows.h>
# include <stdio.h>
# include "error.h"
# include "types.h"
# include "log.h"

# define MOONHOOK "DllMoonHook.dll"

BOOL GetSeDebugPrivilege(void);
std::list<PLDR_MODULE> GetModuleList(void);
void RemoveModuleFromPEB(LPWSTR DllName);
void MoonHook(void);
void PatchImports(HANDLE HFile, PLDR_MODULE Module);

#endif /* !MOONHOOK_H_ */
