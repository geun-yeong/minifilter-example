#ifndef __MINIFLTPORT_EXAMPLE_H__
#define __MINIFLTPORT_EXAMPLE_H__

#include <fltKernel.h>



extern PFLT_FILTER flt_handle; // in main.c
extern FLT_OPERATION_REGISTRATION operations[]; // was written in operation.c
extern FLT_REGISTRATION registration; // was written in registration.c

//
// were written in operation.c
//
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

//
// was written in filterport.c
//
NTSTATUS
MinifltExampleFilterUnloadRoutine(
	_In_ FLT_FILTER_UNLOAD_FLAGS flags
);

#endif