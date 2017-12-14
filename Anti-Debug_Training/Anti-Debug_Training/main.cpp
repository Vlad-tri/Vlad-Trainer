#include<Windows.h>
#include<stdio.h>
#include"Windows_API_anti_dbg.h"
#include"Hardware.h"
#include"NtCreateThreadEx.h"
#include "NtCloseHandle.h"
#include "OpenProcess.h"

INT main(int argc, char* argv[]) {

	printf("[*] Testing for Anti-Debug-Techniques.\r\n");
	
	printf("[*] Check for Windows_API_anti_debug\r\n");
	BOOL res_CheckRemoteDebugger = CheckRemoteDebuggerPresentAPI();

	printf("[*] Check for Hardware_anti_debug\r\n");
	BOOL res_HardwareBreakpoints = HardwareBreakpoints();
	//BOOL res_Interrupt_0x2d = Interrupt_0x2d();

	printf("[*] Check for NtCreateThreadEx\r\n");
	NtCreateThread_hide();

	printf("[*] Check for NtCloseHandle\r\n");
	NtCloseHandle();

	// Not found any anti-debug yet
	printf("[*] Check for OpenProcess csrss.exe\r\n");
	OpenProcessCsrss();


	system("pause");
	return 0;
}