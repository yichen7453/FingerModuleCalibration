#include "stdafx.h"

#include "winusb_ctl.h"
#include "libusb_ctl.h"

#define countof(x) (sizeof(x) / sizeof(x[0]))

static WINUSB_DEVICE_DATA deviceData;


HRESULT RetrieveDevicePath(
	_Out_bytecap_(BufLen)	LPTSTR	DevicePath, 
	_In_					ULONG	BufLen, 
	_Out_opt_				PBOOL	FailureDeviceNotFound);



/*
 * Routine description:
 *		Open all needed handles to interact with the device.
 *		If the device has multiple USB interfaces, this function grants access to only the 
 *		first interface. If multiple devices have the same device interface GUID, there is 
 *		no guarantee of which one will be returned.
 *
 * Arguments:
 *		DeviceData - Struct filled in by this function. The caller should use the WinusbHandle
 *					 to interact with the device, and must pass the struct to CloseDevice when 
 *					 finished.
 *		FailureDeviceNotFound - TRUE when failure is returned due to no devices found with the 
 *								correct device interface (device not connected, driver not 
 *								installed, or device is disabled in Device Manager); FALSE
 *								otherwise.		
 *
 * Return value:
 *		HRESULT
*/
HRESULT WinUSB_OpenDevice(
	_Out_		P_WINUSB_DEVICE_DATA	DeviceData,
	_Out_opt_	PBOOL			FailureDeviceNotFound)
{
	printf("\nWinUSB_OpenDevice -->\n");

	HRESULT hr = S_OK;
	BOOL bResult;

	DeviceData->HandlesOpen = FALSE;

	hr = RetrieveDevicePath(DeviceData->DevicePath, sizeof(DeviceData->DevicePath), FailureDeviceNotFound);

	if (FAILED(hr)) {
		return hr;
	}

	printf("  DeviceName = (%ls)\n", DeviceData->DevicePath);

	DeviceData->DeviceHandle = CreateFile(DeviceData->DevicePath,
											GENERIC_WRITE | GENERIC_READ,
											FILE_SHARE_WRITE | FILE_SHARE_READ,
											NULL,
											OPEN_EXISTING,
											FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
											NULL);

	if (INVALID_HANDLE_VALUE == DeviceData->DeviceHandle) {
		hr = HRESULT_FROM_WIN32(GetLastError());
		printf("  Failed to open the device, error - %d\n", GetLastError());
		return hr;
	}

	bResult = WinUsb_Initialize(DeviceData->DeviceHandle, &DeviceData->WinusbHandle);

	if (FALSE == bResult) {
		hr = HRESULT_FROM_WIN32(GetLastError());
		CloseHandle(DeviceData->DeviceHandle);
		return hr;
	}
	else {
		hr = S_OK;
	}

	DeviceData->HandlesOpen = TRUE;

	return hr;
}

/*
 * Routine description:
 *		Perform required cleanup when the device is no longer needed.
 *		If OpenDevice failed, do nothing.
 *
 * Arguments:
 *		DeviceData - Struct filled in by OpenDevice
 *
 * Return value:
 *		None
*/
VOID WinUSB_CloseDevice(
	_Inout_ P_WINUSB_DEVICE_DATA DeviceData)
{
	if (FALSE == DeviceData->HandlesOpen) {
		// 
		// Called on an uninitialized DeviceData
		//
		return;
	}

	WinUsb_Free(DeviceData->WinusbHandle);
	CloseHandle(DeviceData->DeviceHandle);
	DeviceData->HandlesOpen = FALSE;

	return;
}

/*
 * Routine description:
 *		Retrieve the device path that can be used to open the WinUSB-based device.
 *		If multiple devices have the same device interface GUID, there is no 
 *		guarantee of which one will be returned.
 *
 * Arguments:
 *		DevicePath - On successful return, the path of the device (use with CreateFile).
 *		Buflen - The size of DevicePath's buffer, in bytes.
 *		FailureDeviceNotFound - TRUE when failure is returned due to no devices found with the correct device
 *								interface (device not connected, driver not installed, or device is disabled 
 *								in Device Manager); FALSE otherwise.
 *
 * Return value:
 *		HRESULT
*/
HRESULT RetrieveDevicePath(
	_Out_bytecap_(BufLen)	LPTSTR	DevicePath, 
	_In_					ULONG	BufLen, 
	_Out_opt_				PBOOL	FailureDeviceNotFound)		
{
	BOOL								bResult = FALSE;
	HDEVINFO							hardwareDeviceInfo;
	SP_DEVICE_INTERFACE_DATA			deviceInterfaceData;
	PSP_DEVICE_INTERFACE_DETAIL_DATA	deviceInterfaceDetailData = NULL;
	ULONG								length, requiredLength = 0;
	HRESULT								hr;
	
	
	GUID winUsb_guid = { 0xB6751000, 0x300B, 0x4428,{ 0xB3, 0x58, 0x17, 0xE6, 0xCA, 0x38, 0x34, 0xD4 } };

	if (NULL != FailureDeviceNotFound) {
		*FailureDeviceNotFound = FALSE;
	}

	//
	// Enumerate all devices exposing the interface
	// 
	hardwareDeviceInfo = SetupDiGetClassDevs(&winUsb_guid, NULL, NULL, (DIGCF_PRESENT | DIGCF_DEVICEINTERFACE));

	if (hardwareDeviceInfo == INVALID_HANDLE_VALUE) {
		hr = HRESULT_FROM_WIN32(GetLastError());
		printf("\n GetDevicePath() : SetupDiGetClassDevs failed!\n");
		return hr;
	}

	deviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

	// 
	// Get the first interface (index 0) in the result set
	// 
	bResult = SetupDiEnumDeviceInterfaces(hardwareDeviceInfo, NULL, &winUsb_guid, 0, &deviceInterfaceData);

	if (FALSE == bResult) {

		//
		// We would see this error if no devices were found
		//
		if (ERROR_NO_MORE_ITEMS == GetLastError() && NULL != FailureDeviceNotFound) {
			*FailureDeviceNotFound = TRUE;
		}
		hr = HRESULT_FROM_WIN32(GetLastError());
		printf("\n GetDevicePath() : SetupDiEnumInterfaces, error - %d\n", GetLastError());
		SetupDiDestroyDeviceInfoList(hardwareDeviceInfo);
		return hr;
	}

	//
	// Get the size of the path string
	// We expect to get a failure with insufficient buffer
	//
	bResult = SetupDiGetDeviceInterfaceDetail(hardwareDeviceInfo, &deviceInterfaceData, NULL, 0, &requiredLength, NULL);

	if (FALSE == bResult && ERROR_INSUFFICIENT_BUFFER != GetLastError()) {
		hr = HRESULT_FROM_WIN32(GetLastError());
		SetupDiDestroyDeviceInfoList(hardwareDeviceInfo);
		printf("\n GetDevicePath() : Get a failure with insufficient buffer.\n");
		return hr;
	}

	//
	// Allocate temporary space for SetupDi structure
	//
	deviceInterfaceDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA) LocalAlloc(LMEM_FIXED, requiredLength);

	if (NULL == deviceInterfaceDetailData) {
		hr = E_OUTOFMEMORY;
		SetupDiDestroyDeviceInfoList(hardwareDeviceInfo);
		return hr;
	}

	deviceInterfaceDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

	length = requiredLength;

	//
	// Get the interface's path string
	//
	bResult = SetupDiGetDeviceInterfaceDetail(hardwareDeviceInfo, &deviceInterfaceData, deviceInterfaceDetailData, length, &requiredLength, NULL);

	if (FALSE == bResult) {
		hr = HRESULT_FROM_WIN32(GetLastError());
		LocalFree(deviceInterfaceDetailData);
		SetupDiDestroyDeviceInfoList(hardwareDeviceInfo);
		return hr;
	}

	//
	// Give path to the caller. SetupDiGetDeviceInterfaceDatail ensured
	// DevicePath is NULL-terminated.
	//
	hr = StringCchCopy(DevicePath, BufLen, deviceInterfaceDetailData->DevicePath);
	
	LocalFree(deviceInterfaceDetailData);

	SetupDiDestroyDeviceInfoList(hardwareDeviceInfo);

	return hr;
}

BOOL GetUSBDeviceSpeed(
	WINUSB_INTERFACE_HANDLE hDeviceHandle,
	UCHAR* pDeviceSpeed)
{
	if (!pDeviceSpeed || hDeviceHandle == INVALID_HANDLE_VALUE) {
		return FALSE;
	}

	BOOL	bResult = TRUE;
	ULONG	length = sizeof(UCHAR);

	bResult = WinUsb_QueryDeviceInformation(hDeviceHandle, DEVICE_SPEED, &length, pDeviceSpeed);
	if (!bResult) {
		printf(" Error getting device speed: %d.\n", GetLastError());
		goto done;
	}

	if (*pDeviceSpeed == LowSpeed) {
		printf(" Device speed : %d (Low speed).\n", *pDeviceSpeed);
		goto done;
	}

	if (*pDeviceSpeed == FullSpeed) {
		printf(" Device speed : %d (Full speed).\n", *pDeviceSpeed);
		goto done;
	}

	if (*pDeviceSpeed == HighSpeed) {
		printf(" Device speed : %d (High speed).\n", *pDeviceSpeed);
		goto done;
	}

done:
	return bResult;
}

BOOL QueryDeviceEndpoints(
	WINUSB_INTERFACE_HANDLE hDeviceHandle,
	PIPE_ID* pipeid)
{
	if (hDeviceHandle == INVALID_HANDLE_VALUE) {
		return FALSE;
	}

	BOOL bResult = TRUE;

	USB_INTERFACE_DESCRIPTOR InterfaceDescriptor;
	WINUSB_PIPE_INFORMATION  Pipe;

	ZeroMemory(&InterfaceDescriptor, sizeof(USB_INTERFACE_DESCRIPTOR));
	ZeroMemory(&Pipe, sizeof(WINUSB_PIPE_INFORMATION));

	bResult = WinUsb_QueryInterfaceSettings(hDeviceHandle, 0, &InterfaceDescriptor);

	if (bResult) {
		//printf(" InterfaceDescriptor.bNumEndpoints: %d\n", InterfaceDescriptor.bNumEndpoints);
		for (int index = 0; index < InterfaceDescriptor.bNumEndpoints; index++) {
			bResult = WinUsb_QueryPipe(hDeviceHandle, 0, index, &Pipe);

			if (bResult) {
				if (Pipe.PipeType == UsbdPipeTypeControl) {
					printf(" Endpoint index: %d Pipe type: Control, Pipe ID: %d.\n", index, Pipe.PipeId);
				}
				if (Pipe.PipeType == UsbdPipeTypeIsochronous) {
					printf(" Endpoint index: %d Pipe type: Isochronous, Pipe ID: %d.\n", index, Pipe.PipeId);
				}
				if (Pipe.PipeType == UsbdPipeTypeBulk) {
					if (USB_ENDPOINT_DIRECTION_IN(Pipe.PipeId)) {
						printf(" Endpoint index: %d, Pipe type: Bulk In, Pipe ID: %c.\n", index, Pipe.PipeId);
						pipeid->PipeInId = Pipe.PipeId;
					}
					if (USB_ENDPOINT_DIRECTION_OUT(Pipe.PipeId)) {
						printf(" Endpoint index: %d, Pipe type: Bulk Out, Pipe ID: %c.\n", index, Pipe.PipeId);
						pipeid->PipeOutId = Pipe.PipeId;
					}
				}
				if (Pipe.PipeType == UsbdPipeTypeInterrupt) {
					printf(" Endpoint index: %d Pipe type: Interrupt, Pipe ID: %d.\n", index, Pipe.PipeId);
				}
			}
			else {
				continue;
			}
		}
	}

	return bResult;
}

BOOL WinUsb_ReadRegister(
	WINUSB_INTERFACE_HANDLE hDeviceHandle, 
	USHORT index,
	UCHAR *data)
{
	printf(" --> WinUsb_ReadRegister <-- \n");

	if (hDeviceHandle == INVALID_HANDLE_VALUE) {
		return FALSE;
	}

	BOOL bResult = TRUE;

	WINUSB_SETUP_PACKET SetupPacket;
	ZeroMemory(&SetupPacket, sizeof(WINUSB_SETUP_PACKET));

	ULONG cbSent = 0;
	UCHAR RegData;

	SetupPacket.RequestType = 0xC0;	// CTRL_IN
	SetupPacket.Request = FPD_CMD_READ_REG;
	SetupPacket.Value = 0;
	SetupPacket.Index = index;
	SetupPacket.Length = sizeof(UCHAR);

	bResult = WinUsb_ControlTransfer(hDeviceHandle, SetupPacket, &RegData, sizeof(RegData), &cbSent, 0);
	*data = RegData;
	
	if (!bResult) {
		printf("test.....\n");
		goto done;
	}

	printf(" RegData(0x%.02X)\n", RegData);
	printf(" Data sent : %zd\n Actual data transferred : %d.\n", sizeof(RegData), cbSent);

done:
	return bResult;
}

BOOL WinUSB_WriteRegister(
	WINUSB_INTERFACE_HANDLE hDeviceHandle,
	UCHAR *dataTable,
	USHORT dataTableLength)
{
	printf(" WinUSB_WriteRegister -> \n");

	if (hDeviceHandle == INVALID_HANDLE_VALUE) {
		return FALSE;
	}

	BOOL bResult = TRUE;

	WINUSB_SETUP_PACKET SetupPacket;
	ZeroMemory(&SetupPacket, sizeof(WINUSB_SETUP_PACKET));

	ULONG cbSent = 0;

	SetupPacket.RequestType = 0x40;	// CTRL_OUT
	SetupPacket.Request = FPD_CMD_WRITE_REG;
	SetupPacket.Value = 0;
	SetupPacket.Index = 0;
	SetupPacket.Length = dataTableLength;

	bResult = WinUsb_ControlTransfer(hDeviceHandle, SetupPacket, dataTable, dataTableLength, &cbSent, 0);

	if (!bResult) {
		goto done;
	}

	printf(" Data sent : %d\n Actual data transferred : %d.\n", dataTableLength, cbSent);

done:
	return bResult;
}

BOOL WinUSB_StartStream(
	WINUSB_INTERFACE_HANDLE hDeviceHandle,
	USHORT frameSize)
{
	printf(" WinUSB_StartStream -> \n");
	if (hDeviceHandle == INVALID_HANDLE_VALUE) {
		return FALSE;
	}

	BOOL bResult = TRUE;

	WINUSB_SETUP_PACKET SetupPacket;
	ZeroMemory(&SetupPacket, sizeof(WINUSB_SETUP_PACKET));

	ULONG cbSent = 0;

	SetupPacket.RequestType = 0x40;	// CTRL_OUT
	SetupPacket.Request = FPD_CMD_START_STREAM;
	SetupPacket.Value = 0;
	SetupPacket.Index = 0;
	SetupPacket.Length = 2;

	bResult = WinUsb_ControlTransfer(hDeviceHandle, SetupPacket, (UCHAR *)&frameSize, 2, &cbSent, 0);

	if (!bResult) {
		goto done;
	}

	printf(" Data sent : %d\n Actual data transferred : %d.\n", 2, cbSent);

done:
	return bResult;
}

BOOL WinUSB_StopStream(
	WINUSB_INTERFACE_HANDLE hDeviceHandle)
{
	printf(" WinUSB_StopStream -> \n");

	if (hDeviceHandle == INVALID_HANDLE_VALUE) {
		return FALSE;
	}

	BOOL bResult = TRUE;

	WINUSB_SETUP_PACKET SetupPacket;
	ZeroMemory(&SetupPacket, sizeof(WINUSB_SETUP_PACKET));

	ULONG cbSent = 0;
	USHORT frameSize = 0;

	SetupPacket.RequestType = 0x40;	// CTRL_OUT
	SetupPacket.Request = FPD_CMD_STOP_STREAM;
	SetupPacket.Value = 0;
	SetupPacket.Index = 0;
	SetupPacket.Length = 2;

	bResult = WinUsb_ControlTransfer(hDeviceHandle, SetupPacket, (UCHAR *)&frameSize, 2, &cbSent, 0);

	if (!bResult) {
		goto done;
	}

	printf(" Data sent : %d\n Actual data transferred : %d.\n", 2, cbSent);

done:
	return bResult;
}

BOOL WinUSB_GetImage(
	WINUSB_INTERFACE_HANDLE hDeviceHandle,
	UCHAR** readData,
	ULONG* readLength)
{
	printf(" WinUSB_ReadFromBulkEndpoint -> \n");

	if (hDeviceHandle == INVALID_HANDLE_VALUE) {
		return FALSE;
	}

	BOOL bResult = TRUE;

	//UCHAR* szBuffer = (UCHAR*)LocalAlloc(LPTR, sizeof(UCHAR)*cbSize);
	UCHAR szBuffer[4162];

	ULONG cbRead = 0;

	bResult = WinUsb_ReadPipe(hDeviceHandle, 0x81, szBuffer, 4162, &cbRead, 0);
	*readData = szBuffer;
	*readLength = cbRead;

	if (!bResult) {
		goto done;
	}
	
	printf(" Actual data read: %d.\n", cbRead);

done:
	LocalFree(szBuffer);
	return bResult;
}


