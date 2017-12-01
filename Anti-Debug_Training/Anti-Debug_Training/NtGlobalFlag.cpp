#include"NtGlobalFlag.h"


BOOL IsWow64() {
	typedef BOOL(WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
	LPFN_ISWOW64PROCESS fnIsWow64Process;

	BOOL bIsWow64 = FALSE;
	fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(GetModuleHandleA("kernel32.dll"), "IsWow64Process");

	if (fnIsWow64Process != NULL)
	{
		if (!fnIsWow64Process(GetCurrentProcess(), &bIsWow64))
		{
			// handle error
		}
	}

	return bIsWow64;
}

BOOL NtGlobalFlag() {

	PDWORD pNtGlobalFlag = NULL, pNtGlobalFlagWoW64 = NULL;

#if defined (ENV64BIT)
	BYTE *_teb64 = (BYTE*) __readgsqword(0x30);
	DWORD64 _peb64 = *(DWORD64*)(_teb + 0x60);
	pNtGlobalFlag = (PDWORD)(_readgsqword(0x60) + 0xBC);

#elif defined (ENV32BIT)
	BYTE *_teb32 = (BYTE*)__readfsdword(0x18);
	DWORD _peb32 = *(DWORD*)(_teb + 0x30);
	pNtGlobalFlag = (PDWORD)(_peb32 + 0x68);

	if (IsWoW64()) {

		BYTE *_teb64 = (BYTE*)__readfsdword(0x18) - 0x2000;
		DWORD64 _peb64 = *(DWORD64*)(_teb64 + 0x60);
		pNtGlobalFlagWoW64 = (PDWORD)(_peb64 + 0xBC);
	}
#endif

	BOOL normalDetected = pNtGlobalFlag && *pNtGlobalFlag & 0x00000070;
	BOOL wow64Detected = pNtGlobalFlagWoW64 && *pNtGlobalFlagWoW64 & 0x00000070;

	if (normalDetected || wow64Detected) {
		printf("Debugger caught by NtGlobalFlag.\r\n");
		return TRUE;
	}
	else
	{
		return FALSE;
	}

}