#include "port.h"
#include "error.h"
#include "../common/common.h"

static PFLT_PORT flt_port;

NTSTATUS
MinifltPortInitialize(
	_In_ PFLT_FILTER flt_handle
)
{
	NTSTATUS status         = STATUS_SUCCESS;
	UNICODE_STRING port_name; RtlZeroMemory(&port_name, sizeof(port_name));
	PSECURITY_DESCRIPTOR sd;  RtlZeroMemory(&sd, sizeof(sd));
	OBJECT_ATTRIBUTES oa;     RtlZeroMemory(&oa, sizeof(oa));
	
	status = FltBuildDefaultSecurityDescriptor(
		&sd,
		FLT_PORT_ALL_ACCESS
	);
	IF_ERROR(FltBuildDefaultSecurityDescriptor, CLEANUP_PORT_INITIALIZE);

	RtlInitUnicodeString(&port_name, TEXT(MINIFLT_EXAMPLE_PORT_NAME));
	InitializeObjectAttributes(
		&oa, &port_name, OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE, NULL, sd
	);

	status = FltCreateCommunicationPort(
		flt_handle,
		&flt_port,
		&oa,
		NULL,
		MinifltPortNotifyRoutine,
		MinifltPortDisconnectRoutine,
		MinifltPortMessageRoutine,
		1 // max connection
	);
	IF_ERROR(FltCreateCommunicationPort, CLEANUP_PORT_INITIALIZE);

	return STATUS_SUCCESS;

CLEANUP_PORT_INITIALIZE:

	MinifltPortFinalize();

	return status;
}

VOID
MinifltPortFinalize()
{
	if (flt_port) {
		FltCloseCommunicationPort(flt_port);
	}
}

//
// This function will be call when
// user mode application calls FilterConnectCommunicationPort
//
NTSTATUS
MinifltPortNotifyRoutine(
	_In_ PFLT_PORT client_port,
	_In_ PVOID server_cookie,
	_In_ PVOID connection_context,
	_In_ ULONG connection_context_size,
	_Out_ PVOID* connection_port_cookie
)
{
	UNREFERENCED_PARAMETER(client_port);
	UNREFERENCED_PARAMETER(server_cookie);
	UNREFERENCED_PARAMETER(connection_context);
	UNREFERENCED_PARAMETER(connection_context_size);
	UNREFERENCED_PARAMETER(connection_port_cookie);

	DbgPrint(
		"[filterport] " __FUNCTION__ " User-mode application(%u) connect to this filter\n",
		PtrToUint(PsGetCurrentProcessId())
	);

	return STATUS_SUCCESS;
}

//
// This function will be call when
// user mode handle count for the client port reaches zero or
// when the minifilter driver is to be unloaded
//
VOID
MinifltPortDisconnectRoutine(
	_In_ PVOID connection_cookie
)
{
	UNREFERENCED_PARAMETER(connection_cookie);

	DbgPrint(
		"[filterport] " __FUNCTION__ " User-mode application(%u) disconnect with this filter\n",
		PtrToUint(PsGetCurrentProcessId())
	);
}

//
// This function will be call when
// user mode application calls FilterSendMessage
//
NTSTATUS
MinifltPortMessageRoutine(
	_In_ PVOID port_cookie,
	_In_opt_ PVOID input_buffer,
	_In_ ULONG input_buffer_size,
	_Out_opt_ PVOID output_buffer,
	_In_ ULONG output_buffer_size,
	_Out_ PULONG return_output_buffer_length
)
{
	UNREFERENCED_PARAMETER(port_cookie);

	DbgPrint(
		"[filterport] " __FUNCTION__ " User-mode application(%u) send data to this filter\n",
		PtrToUint(PsGetCurrentProcessId())
	);

	if (input_buffer && input_buffer_size == sizeof(USER_TO_FLT)) {
		PUSER_TO_FLT sent = (PUSER_TO_FLT)input_buffer;
		DbgPrint(
			"[filterport] " __FUNCTION__ " Data: %ws\n",
			sent->msg
		);
	}
	
	if (output_buffer && output_buffer_size == sizeof(USER_TO_FLT_REPLY)) {
		PUSER_TO_FLT_REPLY reply = (PUSER_TO_FLT_REPLY)output_buffer;
		wcscpy_s(reply->msg, ARRAYSIZE(reply->msg), L"Hello, User");
		*return_output_buffer_length = (ULONG)(wcslen(L"Hello, User") * sizeof(wchar_t));
	}
	
	return STATUS_SUCCESS;
}