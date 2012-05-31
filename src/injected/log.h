#ifndef LOG_H_
# define LOG_H_

# include <windows.h>
# include <stdio.h>

HANDLE OpenLog(char *filename);
void logFunction(HANDLE HFile, PCHAR FuncName);
void logMessage(HANDLE hFile, char *msg);
void wlogMessage(HANDLE hFile, LPWSTR msg);

#endif /* !LOG_H_ */