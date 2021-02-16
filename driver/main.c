#include "common.h"

void start(void* context)
{
	log("Thread started.\n");

	initialize_server(55123ui16);

	return;
}

NTSTATUS DriverEntry(PDRIVER_OBJECT driver_object, PUNICODE_STRING registry_path)
{
	HANDLE thread_handle;
	auto result = PsCreateSystemThread(&thread_handle, THREAD_ALL_ACCESS, 0, 0, 0, start, 0);

	return result;
}