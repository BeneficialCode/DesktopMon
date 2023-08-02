#include <ntddk.h>

void DriverUnload(PDRIVER_OBJECT);
OB_PREOP_CALLBACK_STATUS OnPreOpenDesktop(PVOID RegistrationContext,
	POB_PRE_OPERATION_INFORMATION Info);

PVOID g_RegHandle = nullptr;

extern "C"
NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING) {
	KdPrint(("Driver Entry entered\n"));

	OB_OPERATION_REGISTRATION operation = {
		ExDesktopObjectType,// object type
		OB_OPERATION_HANDLE_CREATE | OB_OPERATION_HANDLE_DUPLICATE,
		OnPreOpenDesktop,nullptr // pre,post
	};
	OB_CALLBACK_REGISTRATION reg = {
		OB_FLT_REGISTRATION_VERSION,
		1,// operation count
		RTL_CONSTANT_STRING(L"4567123.6789"), // altitude
		nullptr,
		&operation
	};

	NTSTATUS status = STATUS_SUCCESS;

	status = ObRegisterCallbacks(&reg, &g_RegHandle);
	if (!NT_SUCCESS(status)) {
		KdPrint(("failed to register callbacks (0x%08X)\n", status));
		return status;
	}

	DriverObject->DriverUnload = DriverUnload;

	KdPrint(("DriverEntry completed successfully!\n"));

	return status;
}

void DriverUnload(PDRIVER_OBJECT DriverObject) {
	UNREFERENCED_PARAMETER(DriverObject);
	ObUnRegisterCallbacks(g_RegHandle);
}


OB_PREOP_CALLBACK_STATUS OnPreOpenDesktop(PVOID RegistrationContext,
	POB_PRE_OPERATION_INFORMATION Info) {
	UNREFERENCED_PARAMETER(RegistrationContext);

	KdPrint(("Object: 0x%p\n", Info->Object));
	KdPrint(("Flag: %d,Desired Access: %d \
		Original Desired Access: %d\n", Info->Flags, Info->Parameters->CreateHandleInformation.DesiredAccess,
		Info->Parameters->CreateHandleInformation.OriginalDesiredAccess));

	
	return OB_PREOP_SUCCESS;
}