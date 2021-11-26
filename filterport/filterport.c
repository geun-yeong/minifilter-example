#include "filterport.h"

NTSTATUS
MinifltExampleFilterUnloadRoutine(
	_In_ FLT_FILTER_UNLOAD_FLAGS flags
)
{
	UNREFERENCED_PARAMETER(flags);

	if (flt_handle) {
		FltUnregisterFilter(flt_handle);
	}

	return STATUS_SUCCESS;
}