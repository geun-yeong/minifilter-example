#include "miniflt.h"
#include <windef.h>

FLT_OPERATION_REGISTRATION operations[] = {
	{
		IRP_MJ_CREATE,
		0,
		MinifltExampleCreatePreRoutine,
		MinifltExampleCreatePostRoutine,
		NULL
	},
	{
		IRP_MJ_OPERATION_END
	}
};



FLT_PREOP_CALLBACK_STATUS 
MinifltExampleCreatePreRoutine(
	_Inout_ PFLT_CALLBACK_DATA data,
	_In_    PCFLT_RELATED_OBJECTS flt_object,
	_Out_   PVOID* completion_context
)
{
	UNREFERENCED_PARAMETER(data);
	UNREFERENCED_PARAMETER(completion_context);

	if (flt_object && flt_object->FileObject && flt_object->FileObject->FileName.Buffer) {
		DbgPrint(
			"[miniflt] " __FUNCTION__ "  [%u] Start    to creat/open a file (%wZ)\n",
			PtrToUint(PsGetCurrentProcessId()),
			&(flt_object->FileObject->FileName)
		);
	}
	
	return FLT_PREOP_SUCCESS_WITH_CALLBACK;
}



FLT_POSTOP_CALLBACK_STATUS 
MinifltExampleCreatePostRoutine(
	_Inout_      PFLT_CALLBACK_DATA data,
	_In_         PCFLT_RELATED_OBJECTS flt_object,
	_In_opt_     PVOID completion_context,
	_In_         FLT_POST_OPERATION_FLAGS flags
)
{
	UNREFERENCED_PARAMETER(data);
	UNREFERENCED_PARAMETER(completion_context);
	UNREFERENCED_PARAMETER(flags);

	if (flt_object && flt_object->FileObject && flt_object->FileObject->FileName.Buffer) {
		DbgPrint(
			"[miniflt] " __FUNCTION__ " [%u] Complete to creat/open a file (%wZ)\n",
			PtrToUint(PsGetCurrentProcessId()),
			&(flt_object->FileObject->FileName)
		);
	}

	return FLT_POSTOP_FINISHED_PROCESSING;
}