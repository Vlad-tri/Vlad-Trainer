#include <Windows.h>
#include <stdio.h>

EXCEPTION_DISPOSITION ExceptionRoutine(
	PEXCEPTION_RECORD ExceptionRecord,
	PVOID             EstablisherFrame,
	PCONTEXT          ContextRecord,
	PVOID             DispatcherContext) {

	if (EXCEPTION_INVALID_HANDLE == ExceptionRecord->ExceptionCode) {
		printf("You are debugging the program\r\n");
		exit(-1);
	}
	return ExceptionContinueExecution;
}

BOOL NtCloseHandle() {
	
	__asm {
			push ExceptionRoutine
			push dword ptr fs : [0]
			mov  dword ptr fs : [0], esp
	}

	CloseHandle((HANDLE)0xDEADBEEF);

	__asm {
			mov eax, [esp]
			mov dword ptr fs : [0], eax
			add esp, 8
	}

	printf("You are not debugging the program\r\n");
	return TRUE;
}