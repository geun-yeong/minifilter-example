#include <Windows.h>
#include <fltUser.h>
#include "../common/common.h"

#include <iostream>
#include <exception>
using namespace std;

int main() {
	char exception_msg[128];

	HANDLE port_handle;
	HRESULT h_result;

	USER_TO_FLT sent;        ZeroMemory(&sent, sizeof(sent));
	USER_TO_FLT_REPLY reply; ZeroMemory(&reply, sizeof(reply));
	DWORD returned_bytes = 0;

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
		
		

		// send a message and receive a reply
		wcscpy_s(sent.msg, ARRAYSIZE(sent.msg), L"Hello, MiniFilter");
		h_result = FilterSendMessage(
			port_handle,
			&sent,
			sizeof(sent),
			&reply,
			sizeof(reply),
			&returned_bytes
		);

		if (IS_ERROR(h_result)) {
			sprintf_s(
				exception_msg, 128,
				"FilterSendMessage failed (HRESULT = 0x%x)", h_result
			);
			throw exception(exception_msg);
		}

		// if minifilter send the reply
		if (returned_bytes > 0) {
			wcout << reply.msg << endl;
		}
		else {
			cout << "No reply" << endl;
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