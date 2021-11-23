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
	IF_ERROR(FltRegisterFilter, EXIT_OF_DRIVER_ENTRY);

	status = FltStartFiltering(
		flt_handle
	);
	IF_ERROR(FltStartFiltering, EXIT_OF_DRIVER_ENTRY);

EXIT_OF_DRIVER_ENTRY:

	return status;
}