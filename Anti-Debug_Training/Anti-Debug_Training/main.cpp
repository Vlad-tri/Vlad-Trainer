#include<Windows.h>
#include<stdio.h>
#include"Windows_API_anti_dbg.h"
#include"Hardware.h"

INT main(int argc, char* argv[]) {
	printf("[*] Testing for Anti-Debug-Techniques.\r\n");
	
	printf("[*] Check for Windows_API_anti_debug\r\n");
	BOOL res_CheckRemoteDebugger = CheckRemoteDebuggerPresentAPI();

	printf("[*] Check for Hardware_anti_debug\r\n");
	BOOL res_HardwareBreakpoints = HardwareBreakpoints();

	system("pause");
	return 0;
}