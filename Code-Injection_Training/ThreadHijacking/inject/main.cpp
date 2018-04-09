#include <Windows.h>

BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		MessageBox(NULL, "DLL loaded!", "MyDLL", MB_ICONINFORMATION);
		break;
	case DLL_PROCESS_DETACH:
		MessageBox(NULL, "DLL unloaded!", "MyDLL", MB_ICONINFORMATION);
		break;
	default:
		break;
	}

	return TRUE;
}