#include "OpenProcess.h"

DWORD GetCsrssProcessId() {
	pCsrGetId CsrGetProcessId = (pCsrGetId)GetProcAddress(GetModuleHandleA("ntdll.dll"), "CsrGetProcessId");
	if (CsrGetProcessId) {
		return CsrGetProcessId();
	}
	else {
		return 0;
	}
}

BOOL OpenProcessCsrss() {

	HANDLE csrss = OpenProcess(PROCESS_ALL_ACCESS, FALSE, GetCsrssProcessId());
	if (csrss != NULL) {
		printf("You are doing debugging\r\n");
		return FALSE;
	}
	else {
		printf("You are not doing debugging yet\r\n");
		return TRUE;
	}
}