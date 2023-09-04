#include "includes.hpp"

extern void NTAPI server_thread(void*);

extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT  driver_object, PUNICODE_STRING registry_path)
{
	UNREFERENCED_PARAMETER(driver_object);
	UNREFERENCED_PARAMETER(registry_path);


	HANDLE thread = nullptr;
	const auto status = PsCreateSystemThread(&thread, GENERIC_ALL, nullptr, nullptr, nullptr, server_thread, nullptr);

	if (!NT_SUCCESS(status))
	{
		return STATUS_UNSUCCESSFUL;
	}

	ZwClose(thread);
	return STATUS_SUCCESS;
}
