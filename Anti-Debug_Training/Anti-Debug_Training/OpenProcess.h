#include <Windows.h>
#include <stdio.h>

BOOL OpenProcessCsrss();
typedef DWORD(NTAPI* pCsrGetId)(VOID);