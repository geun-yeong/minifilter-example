#ifndef __PORT_H__
#define __PORT_H__

#include <fltKernel.h>

NTSTATUS
MinifltPortInitialize(
	_In_ PFLT_FILTER flt_handle
);

VOID
MinifltPortFinalize();

NTSTATUS
MinifltPortNotifyRoutine(
	_In_ PFLT_PORT client_port,
	_In_ PVOID server_cookie,
	_In_ PVOID connection_context,
	_In_ ULONG connection_context_size,
	_Out_ PVOID* connection_port_cookie
);

VOID
MinifltPortDisconnectRoutine(
	_In_ PVOID connection_cookie
);

NTSTATUS
MinifltPortMessageRoutine(
	_In_ PVOID port_cookie,
	_In_opt_ PVOID input_buffer,
	_In_ ULONG input_buffer_size,
	_Out_opt_ PVOID output_buffer,
	_In_ ULONG output_buffer_size,
	_Out_ PULONG return_output_buffer_length
);

NTSTATUS
MinifltPortSendMessage(
	_In_ PVOID send_data,
	_In_ ULONG send_data_size,
	_Out_opt_ PVOID recv_buffer,
	_In_ ULONG recv_buffer_size,
	_Out_ PULONG written_bytes_to_recv_buffer
);

#endif