#include "Hardware.h"

BOOL HardwareBreakpoints()
{
	PCONTEXT ctx = PCONTEXT(VirtualAlloc(NULL, sizeof(ctx), MEM_COMMIT, PAGE_READWRITE));
	SecureZeroMemory(ctx, sizeof(CONTEXT));

	ctx->ContextFlags = CONTEXT_DEBUG_REGISTERS;

	if (!GetThreadContext(GetCurrentThread(), ctx))
		return -1;

	if (ctx->Dr0 != 0 || ctx->Dr1 != 0 || ctx->Dr2 != 0 || ctx->Dr3 != 0)
	{
		printf("Due to HardwareBreakpoints your debugger catched.\r\n");
		return TRUE;
	}
	else
		return FALSE;
}

// Interrupt-0x2d using Vectored Exception Handling
VOID __int2d() {
	_asm {
		int 2dh
		nop
		ret
	}
}

static BOOL SwallowedException_0x2d = TRUE;

static LONG CALLBACK VectorHandler_0x2d(PEXCEPTION_POINTERS ExceptionInfo) {
	SwallowedException_0x2d = FALSE;
	if (ExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_BREAKPOINT) {
		return EXCEPTION_CONTINUE_EXECUTION;
	}
	return EXCEPTION_CONTINUE_SEARCH;
}

BOOL Interrupt_0x2d() {

	PVOID Handle = AddVectoredExceptionHandler(1, VectorHandler_0x2d);
	SwallowedException_0x2d = TRUE;
	__int2d();
	RemoveVectoredExceptionHandler(Handle);
	return SwallowedException_0x2d;
}

// Interrupt 0x3 using Vectored Exception Handling
static BOOL SwallowedException_0x3 = TRUE;

static LONG CALLBACK VectorHandler_0x3(PEXCEPTION_POINTERS ExceptionInfo) {
	SwallowedException_0x3 = FALSE;
	if (ExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_BREAKPOINT) {

#ifdef _WIN64
		ExceptionInfo->ContextRecord->Rip++;
#else
		ExceptionInfo->ContextRecord->Eip++;
#endif
		return EXCEPTION_CONTINUE_EXECUTION;
	}
	return EXCEPTION_CONTINUE_SEARCH;
}

BOOL Inetrrupt_0x3() {
	PVOID Handle = AddVectoredExceptionHandler(1, VectorHandler_0x3);
	SwallowedException_0x3 = TRUE;
	__debugbreak();
	RemoveVectoredExceptionHandler(Handle);
	return SwallowedException_0x3;
}