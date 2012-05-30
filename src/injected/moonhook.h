#ifndef MOONHOOK_H_
# define MOONHOOK_H_

# include <list>
# include <windows.h>
# include <stdio.h>
# include "error.h"
# include "types.h"
# include "log.h"

BOOL GetSeDebugPrivilege(void);
std::list<PLDR_MODULE> GetModuleList(void);

#endif /* !MOONHOOK_H_ */
