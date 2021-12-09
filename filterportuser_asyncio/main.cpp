﻿#include <Windows.h>
#include <fltUser.h>
#include "../common/common.h"

#include <iostream>
#include <exception>
using namespace std;

typedef struct _FLT_TO_USER_WRAPPER {

	FILTER_MESSAGE_HEADER hdr;
	FLT_TO_USER data;

} FLT_TO_USER_WRAPPER, * PFLT_TO_USER_WRAPPER;

typedef struct _FLT_TO_USER_REPLY_WRAPPER {

	FILTER_REPLY_HEADER hdr;
	FLT_TO_USER_REPLY data;

} FLT_TO_USER_REPLY_WRAPPER, * PFLT_TO_USER_REPLY_WRAPPER;



int main() {
	char exception_msg[128];

	HANDLE port_handle;
	HRESULT h_result;

	USER_TO_FLT sent;        ZeroMemory(&sent, sizeof(sent));
	USER_TO_FLT_REPLY reply; ZeroMemory(&reply, sizeof(reply));
	OVERLAPPED overlapped;   ZeroMemory(&overlapped, sizeof(overlapped));

	FLT_TO_USER_WRAPPER recv;			  ZeroMemory(&recv, sizeof(recv));
	FLT_TO_USER_REPLY_WRAPPER recv_reply; ZeroMemory(&recv_reply, sizeof(recv_reply));

	DWORD returned_bytes = 0, wait_count = 0;

	overlapped.hEvent = CreateEvent(NULL, FALSE, FALSE, L"Local\\FilterGetMessage_IsSuccess");
	if (!overlapped.hEvent) {
		fprintf(stderr, "CreateEvent failed (error = %u)\n", GetLastError());
		return 1;
	}

	try {
		// connect to minifilter communication port
		h_result = FilterConnectCommunicationPort(
			TEXT(MINIFLT_EXAMPLE_PORT_NAME),
			0,
			nullptr,
			0,
			nullptr,
			&port_handle
		);

		if (IS_ERROR(h_result)) {
			sprintf_s(
				exception_msg, 128,
				"FilterConnectCommunicationPort failed (HRESULT = 0x%x)", h_result
			);
			throw exception(exception_msg);
		}



		// get message from minifilter with async io
		h_result = FilterGetMessage(port_handle, &recv.hdr, sizeof(recv), &overlapped);
		if (h_result != 0x800703e5 && IS_ERROR(h_result)) {
			sprintf_s(
				exception_msg, 128,
				"FilterGetMessage failed (HRESULT = 0x%x)", h_result
			);
			throw exception(exception_msg);
		}



		while (WaitForSingleObject(overlapped.hEvent, 1000 /* 1 second */) == WAIT_TIMEOUT) {
			printf("wait for %u times...\n", ++wait_count);
		}

		DWORD transferred = 0;
		if (GetOverlappedResult(port_handle, &overlapped, &transferred, FALSE)) {

			printf("FilterGetMessage was finished!!! (%ws)\n", recv.data.path);

			// if there is "test.txt" in path, block it
			ZeroMemory(&recv_reply, sizeof(recv_reply));
			recv_reply.data.block = TRUE;
			recv_reply.hdr.MessageId = recv.hdr.MessageId;

			// send the reply
			h_result = FilterReplyMessage(
				port_handle,
				&recv_reply.hdr,
				sizeof(recv_reply.hdr) + sizeof(recv_reply.data)
			);

			if (IS_ERROR(h_result)) {
				sprintf_s(
					exception_msg, 128,
					"FilterReplyMessage failed (HRESULT = 0x%x)", h_result
				);
				throw exception(exception_msg);
			}
		}
	}
	catch (const exception& e) {
		cerr << e.what() << endl;
	}



	if (port_handle) {
		FilterClose(port_handle);
	}

	return 0;
}