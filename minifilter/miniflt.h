#ifndef __MINIFLT_EXAMPLE_H__
#define __MINIFLT_EXAMPLE_H__

#include <fltKernel.h>

#define IF_ERROR(func_name, to) \
			if(!NT_SUCCESS(status)) { \
				DbgPrint("[miniflt] " __FUNCTION__ " - " #func_name " failed (status: 0x%x)", status); \
				goto to; \
			}



extern PFLT_FILTER flt_handle;
extern FLT_OPERATION_REGISTRATION operations[];
extern FLT_REGISTRATION registration;

FLT_PREOP_CALLBACK_STATUS 
MinifltExampleCreatePreRoutine(
	_Inout_ PFLT_CALLBACK_DATA Data,
	_In_    PCFLT_RELATED_OBJECTS FltObjects,
	_Out_   PVOID* CompletionContext
);

FLT_POSTOP_CALLBACK_STATUS 
MinifltExampleCreatePostRoutine(
	_Inout_      PFLT_CALLBACK_DATA Data,
	_In_         PCFLT_RELATED_OBJECTS FltObjects,
	_In_opt_     PVOID CompletionContext,
	_In_         FLT_POST_OPERATION_FLAGS Flags
);

NTSTATUS
MinifltExampleFilterUnloadRoutine(
	_In_ FLT_FILTER_UNLOAD_FLAGS flags
);

#endif