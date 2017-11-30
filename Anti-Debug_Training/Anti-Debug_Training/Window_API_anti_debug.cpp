#include"Windows_API_anti_dbg.h"

BOOL CheckRemoteDebuggerPresentAPI() {
	BOOL bIsDbgPresent = FALSE;
	CheckRemoteDebuggerPresent(GetCurrentProcess(), &bIsDbgPresent);
	if (bIsDbgPresent)
	{
		printf("[*] CheckRemoteDebuggerPresentAPI catch your debugger.\r\n");
		return bIsDbgPresent;
	}
	return bIsDbgPresent;
}