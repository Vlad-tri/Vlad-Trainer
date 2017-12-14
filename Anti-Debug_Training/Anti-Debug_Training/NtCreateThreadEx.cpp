#include "NtCreateThreadEx.h"
#define THREAD_CREATE_FLAGS_HIDE_FROM_DEBUGGER 0x00000004

void WINAPI ContinueExecution(LPVOID param) {
	BOOL check = FALSE;
	printf("This Thread is hidden from the debugger\n");

	pNtQueryInformationThread NtQueryInformationThread;
	NtQueryInformationThread = (pNtQueryInformationThread)GetProcAddress(GetModuleHandleA("ntdll.dll"),
		"NtQueryInformationThread");

	HANDLE currentThread = GetCurrentThread();
	const int ThreadHideFromDebugger = 0x11;
	if (NtQueryInformationThread(currentThread, ThreadHideFromDebugger, &check, sizeof(BOOLEAN), 0) >= 0) {
		if (!check) {
			printf("Anti-Anti-Debug Tool detected!\n");
		}
		else {
			printf("Everything Ok!\n");
		}
	}
	else {
		printf("Query threadhidefromdebugger not available\n");
	}
}


BOOL NtCreateThread_hide() {

	pNtCreateThreadEx NtCreateThreadEx;
	NtCreateThreadEx = (pNtCreateThreadEx)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtCreateThreadEx");

	HANDLE hThread = 0;
	HANDLE currentProcess = GetCurrentProcess();
	NTSTATUS ntStat = NtCreateThreadEx(
		&hThread, THREAD_ALL_ACCESS, 0, currentProcess, (LPTHREAD_START_ROUTINE)ContinueExecution,
		0, THREAD_CREATE_FLAGS_HIDE_FROM_DEBUGGER, 0, 0, 0, 0);

	if (ntStat >= 0) {
		WaitForSingleObject(hThread, INFINITE);
	}
	else {
		printf("NtCreateThreadEx failed!\n");
	}

	return TRUE;
}