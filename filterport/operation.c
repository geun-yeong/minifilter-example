#include "filterport.h"
#include "port.h"
#include "error.h"
#include "../common/common.h"
#include <windef.h>
#include <ntifs.h>

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
	_Inout_ PFLT_CALLBACK_DATA callback_data,
	_In_    PCFLT_RELATED_OBJECTS flt_object,
	_Out_   PVOID* completion_context
)
{
	UNREFERENCED_PARAMETER(callback_data);
	UNREFERENCED_PARAMETER(flt_object);
	UNREFERENCED_PARAMETER(completion_context);

	return FLT_PREOP_SUCCESS_WITH_CALLBACK;
}



FLT_POSTOP_CALLBACK_STATUS
MinifltExampleCreatePostRoutine(
	_Inout_      PFLT_CALLBACK_DATA callback_data,
	_In_         PCFLT_RELATED_OBJECTS flt_object,
	_In_opt_     PVOID completion_context,
	_In_         FLT_POST_OPERATION_FLAGS flags
)
{
	UNREFERENCED_PARAMETER(callback_data);
	UNREFERENCED_PARAMETER(completion_context);
	UNREFERENCED_PARAMETER(flags);

	PFLT_FILE_NAME_INFORMATION name_info = NULL;
	UNICODE_STRING test_txt_pattern, file_path;

	if (flt_object && flt_object->FileObject) {
		NTSTATUS status;

		// get a file path
		status = FltGetFileNameInformation(
			callback_data, 
			FLT_FILE_NAME_NORMALIZED | FLT_FILE_NAME_QUERY_DEFAULT,
			&name_info
		);
		if (!NT_SUCCESS(status)) {
			return FLT_POSTOP_FINISHED_PROCESSING;
		}

		status = FltParseFileNameInformation(name_info);
		IF_ERROR(FltParseFileNameInformation, EXIT_OF_CREATE_POST_OPERATION);

		RtlInitUnicodeString(&test_txt_pattern, L"*TEST.TXT");
		RtlInitUnicodeString(&file_path, name_info->Name.Buffer);
		
		if (!FsRtlIsNameInExpression(&test_txt_pattern, &file_path, TRUE, NULL)) {
			goto EXIT_OF_CREATE_POST_OPERATION;
		}

		// get the file name from the full path
		LPWSTR file_name = wcsrchr(name_info->Name.Buffer, L'\\');
		if (!file_name) {
			file_name = name_info->Name.Buffer;
		}

		FLT_TO_USER sent;        RtlZeroMemory(&sent, sizeof(sent));
		FLT_TO_USER_REPLY reply; RtlZeroMemory(&reply, sizeof(reply));
		ULONG returned_bytes = 0;

		// send the file path to client
		// if client reply to block accessing the file,
		// the minifilter return STATUS_ACCESS_DENIED to filter manager
		wcscpy_s(sent.path, ARRAYSIZE(sent.path), name_info->Name.Buffer);
		status = MinifltPortSendMessage(&sent, sizeof(sent), &reply, sizeof(reply), &returned_bytes);
		if (NT_SUCCESS(status) && returned_bytes > 0 && reply.block) {
			callback_data->IoStatus.Status = STATUS_ACCESS_DENIED;
		}

		DbgPrint(
			"[filterport] " __FUNCTION__ " [%u] Complete to creat/open a file (%ws)\n",
			PtrToUint(PsGetCurrentProcessId()),
			file_name
		);
	}

EXIT_OF_CREATE_POST_OPERATION:

	if (name_info) {
		FltReleaseFileNameInformation(name_info);
	}

	return FLT_POSTOP_FINISHED_PROCESSING;
}