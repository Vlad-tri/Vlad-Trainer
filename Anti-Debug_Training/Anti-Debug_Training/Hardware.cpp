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