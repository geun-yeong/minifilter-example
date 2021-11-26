#include "miniflt.h"

PFLT_FILTER flt_handle;

NTSTATUS
DriverEntry(
	_In_ PDRIVER_OBJECT driver_object,
	_In_ PUNICODE_STRING registry_path
)
{
	UNREFERENCED_PARAMETER(registry_path);

	NTSTATUS status = STATUS_SUCCESS;

	status = FltRegisterFilter(
		driver_object,
		&registration,
		&flt_handle
	);
	IF_ERROR(FltRegisterFilter, CLEANUP_DRIVER_ENTRY);

	status = FltStartFiltering(
		flt_handle
	);
	IF_ERROR(FltStartFiltering, CLEANUP_DRIVER_ENTRY);

	return STATUS_SUCCESS;

CLEANUP_DRIVER_ENTRY:

	if (flt_handle) {
		FltUnregisterFilter(flt_handle);
	}

	return status;
}