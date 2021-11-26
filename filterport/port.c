#include "port.h"
#include "error.h"
#include "../common/common.h"

static PFLT_FILTER flt_handle;
static PFLT_PORT flt_port;
static PFLT_PORT client_port;

NTSTATUS
MinifltPortInitialize(
	_In_ PFLT_FILTER miniflt_handle
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
		miniflt_handle,
		&flt_port,
		&oa,
		NULL,
		MinifltPortNotifyRoutine,
		MinifltPortDisconnectRoutine,
		MinifltPortMessageRoutine,
		1 // max connection
	);
	IF_ERROR(FltCreateCommunicationPort, CLEANUP_PORT_INITIALIZE);

	flt_handle = miniflt_handle;
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
	_In_ PFLT_PORT connected_client_port,
	_In_ PVOID server_cookie,
	_In_ PVOID connection_context,
	_In_ ULONG connection_context_size,
	_Out_ PVOID* connection_port_cookie
)
{
	UNREFERENCED_PARAMETER(server_cookie);
	UNREFERENCED_PARAMETER(connection_context);
	UNREFERENCED_PARAMETER(connection_context_size);
	UNREFERENCED_PARAMETER(connection_port_cookie);

	DbgPrint(
		"[filterport] " __FUNCTION__ " User-mode application(%u) connect to this filter\n",
		PtrToUint(PsGetCurrentProcessId())
	);

	client_port = connected_client_port;

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



//
// This function send data to user-mode application
//
NTSTATUS
MinifltPortSendMessage(
	_In_ PVOID send_data,
	_In_ ULONG send_data_size,
	_Out_opt_ PVOID recv_buffer,
	_In_ ULONG recv_buffer_size,
	_Out_ PULONG written_bytes_to_recv_buffer
)
{
	NTSTATUS status = STATUS_SUCCESS;

	if (recv_buffer) {
		status = FltSendMessage( // receive a reply about sent data
			flt_handle,
			&client_port,
			send_data,
			send_data_size,
			recv_buffer,
			&recv_buffer_size,
			NULL
		);

		*written_bytes_to_recv_buffer = recv_buffer_size;
	}
	else {
		//
		// timeout 값은
		//   - 양수면 1601년 1월 1일을 기준으로 한 절대 시각이 될 때 까지 대기
		//   - 음수면 현재 시간 기준으로 해당 시간 만큼 대기
		//   - NULL이면 무한정 대기
		// 
		// 100 나노초 단위이므로
		//   - 10 = 1 micro second
		//   - 10'000 = 1 milli second
		//   - 10'000'000 = 1 second
		// 
		LARGE_INTEGER timeout;
		timeout.QuadPart = 0;

		status = FltSendMessage( // just sending
			flt_handle,
			&client_port,
			send_data,
			send_data_size,
			NULL,
			NULL,
			&timeout
		);

		*written_bytes_to_recv_buffer = 0;
	}

	return status;
}