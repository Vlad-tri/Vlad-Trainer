#include <Windows.h>
#include <TlHelp32.h>
#include <tchar.h>

int main(int argc, char* argv[]) {

	char currentDirectory[MAX_PATH];
	SIZE_T bytesWritten;

	if (argc != 2) {
		wprintf_s(L"[!] Please specify the target PID\n");
		return 0;
	}
	
	// Opening the process having the particular pid
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, (DWORD)argv[1]);
			
	if (hProcess == INVALID_HANDLE_VALUE) {
		_tprintf(_T("[X] Error Opening Process\n"));
		return 0;
	}

	// Allocating the memory in target process address space
	LPVOID baddr = VirtualAllocEx(hProcess, NULL, 4096, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	if (baddr == NULL) {
		_tprintf(_T("[X] Error Allocating Address Space\n"));
		CloseHandle(hProcess);
		return 0;
	}

	// Getting LoadLibraryA address
	LPVOID LoadAddr = (LPVOID)GetProcAddress(GetModuleHandle(L"kernel32.dll"), "LoadLibraryA");
	if (LoadAddr == NULL) {
		_tprintf(_T("[X] Error Allocating Address Space\n"));
		CloseHandle(hProcess);
		return 0;
	}

	GetCurrentDirectoryA(MAX_PATH, currentDirectory);
	strncat_s(currentDirectory, "\\inject.dll", MAX_PATH);

	_tprintf(_T("[*] Injecting the %hs DLL\n"),currentDirectory);

	// Writing our DLL to the target process memory
	if (WriteProcessMemory(hProcess, baddr, currentDirectory, strlen(currentDirectory)+1, &bytesWritten)) {
		_tprintf(_T("[X] WriteProcessMemory failed\n"));
		CloseHandle(hProcess);
		return 0;
	}

	_tprintf(_T("[*] Written %d bytes\n"), bytesWritten);
	
	// Create the remote thread for running the DLL on target process
	if (CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)LoadAddr, baddr, NULL, NULL) == NULL) {
		_tprintf(_T("[X] Error creating remote thread [%d] :(\n"),GetLastError());
		CloseHandle(hProcess);
		return 0;
	}

	_tprintf(_T("[*] Remote Thread created successfully\n"));

	system("pause");
	return 1;
}