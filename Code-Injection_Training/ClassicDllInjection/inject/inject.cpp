#include<Windows.h>

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved) {
	
	MessageBox(0, L"Dll Injected Successfully", L"Injection", 0);
	
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		MessageBox(0, L"Dll Attached Successfully", L"Injection", 0);
		break;

	case DLL_PROCESS_DETACH:
		MessageBox(0, L"Dll Detached Successfully", L"Injection", 0);
		break;

	case DLL_THREAD_ATTACH:
		break;

	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}