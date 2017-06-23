#include "stdafx.h"

typedef struct _WINUSB_DEVICE_DATA {

	BOOL					HandlesOpen;
	WINUSB_INTERFACE_HANDLE	WinusbHandle;
	HANDLE					DeviceHandle;
	TCHAR					DevicePath[MAX_PATH];

} WINUSB_DEVICE_DATA, *P_WINUSB_DEVICE_DATA;

struct PIPE_ID {
	UCHAR PipeInId;
	UCHAR PipeOutId;
};

HRESULT WinUSB_OpenDevice(
	_Out_	P_WINUSB_DEVICE_DATA DeviceData,
	_Out_opt_ PBOOL			FailureDeviceNotFound);

VOID WinUSB_CloseDevice(
	_Inout_ P_WINUSB_DEVICE_DATA DeviceData);

BOOL GetUSBDeviceSpeed(
	WINUSB_INTERFACE_HANDLE hDeviceHandle,
	UCHAR* pDeviceSpeed);

BOOL QueryDeviceEndpoints(
	WINUSB_INTERFACE_HANDLE hDeviceHandle,
	PIPE_ID* pipeid);

BOOL WinUsb_ReadRegister(
	WINUSB_INTERFACE_HANDLE hDeviceHandle,
	USHORT index,
	UCHAR *data);

BOOL WinUSB_WriteRegister(
	WINUSB_INTERFACE_HANDLE hDeviceHandle,
	UCHAR *dataTable,
	USHORT dataTableLength);

BOOL WinUSB_StartStream(
	WINUSB_INTERFACE_HANDLE hDeviceHandle,
	USHORT frameSize);

BOOL WinUSB_StopStream(
	WINUSB_INTERFACE_HANDLE hDeviceHandle);

BOOL WinUSB_GetImage(
	WINUSB_INTERFACE_HANDLE hDeviceHandle,
	UCHAR** readData,
	ULONG* readLength);
