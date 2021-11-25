#include "filterport.h"
#include "port.h"
#include "error.h"
#include "../common/common.h"

VOID DriverUnload(
	_In_ PDRIVER_OBJECT driver_object
);

PFLT_FILTER flt_handle;

NTSTATUS
DriverEntry(
	_In_ PDRIVER_OBJECT driver_object,
	_In_ PUNICODE_STRING registry_path
)
{
	UNREFERENCED_PARAMETER(registry_path);

	NTSTATUS status = STATUS_SUCCESS;

	// register the minifilter to filter manager
	status = FltRegisterFilter(
		driver_object,
		&registration,
		&flt_handle
	);
	IF_ERROR(FltRegisterFilter, CLEANUP_DRIVER_ENTRY);
	
	// initialize the filter port
	status = MinifltPortInitialize(flt_handle);
	IF_ERROR(MinifltPortInitialize, CLEANUP_DRIVER_ENTRY);

	// start my minifilter filtering
	status = FltStartFiltering(
		flt_handle
	);
	IF_ERROR(FltStartFiltering, CLEANUP_DRIVER_ENTRY);

	// set a driver unload routine
	driver_object->DriverUnload = DriverUnload;

	return STATUS_SUCCESS;

CLEANUP_DRIVER_ENTRY:

	MinifltPortFinalize();

	if (flt_handle) {
		FltUnregisterFilter(flt_handle);
	}

	return status;
}



VOID DriverUnload(
	_In_ PDRIVER_OBJECT driver_object
)
{
	UNREFERENCED_PARAMETER(driver_object);

	MinifltPortFinalize();

	if (flt_handle) {
		FltUnregisterFilter(flt_handle);
	}
}