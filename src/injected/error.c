#include "error.h"

void ErrorMsg(char *FuncName, char *ApiName)
{

  printf("Error inside Function : %s because of Api : %s, ErrorCode : %X\n",
      FuncName, ApiName, GetLastError());
  exit(42);
}
