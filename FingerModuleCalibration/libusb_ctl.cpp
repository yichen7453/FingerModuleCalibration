#include "stdafx.h"

#include "libusb_ctl.h"
#include "sensor_init.h"


uint8_t		buf_finger[_301_FRAME_SIZE];
uint8_t		swipe_buf_finger[_301_SWIPE_FRAME_SIZE];

uint8_t		frame_toggle	= 0x02;
uint16_t	pixel_cnt		= 0;
int			transfer_cnt	= 0;;

struct libusb_device_handle *dev = NULL;

HRESULT LibUSB_OpenDevice(
	_Out_	P_LIBUSB_DEVICE_DATA DeviceData,
	_Out_	USHORT			vid, 
	_Out_	USHORT			pid)
{
	UINT res = -1;
	HRESULT hr = S_OK;

	printf("LibUsb_OpenDevice --> \n");

	const struct libusb_version* version;
	version = libusb_get_version();
	printf("  libusb version %d.%d.%d.%d\n\n", version->major, version->minor, version->micro, version->nano);

	if (!libusb_has_capability(LIBUSB_CAP_HAS_HOTPLUG)) {
		printf("  Hotplug capabilites are not supported on this platform\n");
		//libusb_exit(NULL);
	}
	else {
		printf("  Hotplug capabilites have supported on this platform\n");
	}

	res = libusb_init(NULL);
	if (res > 0) {
		printf("  libusb_init fali. (%d)\n", res);
		hr = S_FALSE;
	}
	else {
		printf("  libusb_init success. (%d)\n", res);

		DeviceData->LibusbHandle = libusb_open_device_with_vid_pid(NULL, vid, pid);

		if (DeviceData->LibusbHandle != NULL) {
			res = libusb_claim_interface(DeviceData->LibusbHandle, 0);
			if (res < 0) {
				printf("  libusb_claim_interface fail. (%d)\n", res);
				hr = S_FALSE;
			}
			else {
				printf("  libusb_claim_interface success. (%d)\n", res);
			}
		} else {
			hr = S_FALSE;
		} 
	}

	return hr;
}

VOID LibUSB_CloseDevice(
	_Inout_ P_LIBUSB_DEVICE_DATA DeviceData)
{
	if (DeviceData->LibusbHandle == NULL) {
		return;
	}

	libusb_release_interface(DeviceData->LibusbHandle, 0);
	CloseHandle(DeviceData->LibusbHandle);
	libusb_close(DeviceData->LibusbHandle);

	return;
}

BOOL LibUSB_StartStream(
	libusb_device_handle *hDeviceHandle,
	USHORT frameSize) 
{
	printf("LibUsb_StartStream ->\n");
	if (hDeviceHandle == NULL) {
		return FALSE;
	}

	BOOL bResult = TRUE;
	UINT res	 = -1;

	res = libusb_control_transfer(hDeviceHandle, CTRL_OUT, FPD_CMD_START_STREAM, 0, 0, (uint8_t *)&frameSize, 2, 0);

	if (res < 0) {
		printf("usb_ctl -> usb_sensor_start_stream fail. (%d)\n", res);
		return FALSE;
	}
	else {
		printf("usb_ctl -> usb_sensor_start_stream. (%d)\n", res);
	}

	return bResult;;
}

BOOL LibUSB_StopStream(
	libusb_device_handle *hDeviceHandle) 
{
	printf("LibUsb_StopStrea ->\n");

	if (hDeviceHandle == NULL) {
		return FALSE;
	}

	UINT	res = -1;
	USHORT	frameSize;
	BOOL	bResult = TRUE;

	frameSize = 0;
	res = libusb_control_transfer(hDeviceHandle, CTRL_OUT, FPD_CMD_STOP_STREAM, 0, 0, (uint8_t *)&frameSize, 2, 0);
	if (res < 0) {
		printf("usb_ctl -> usb_sensor_stop_stream fail. (%d)\n", res);
		return FALSE;
	}
	else {
		printf("usb_ctl -> usb_sensor_stop_stream. (%d)\n", res);
	}

	return bResult;
}

BOOL LibUSB_WriteRegister(
	libusb_device_handle *hDeviceHandle,
	UCHAR *dataTable, 
	USHORT dataTableLength)
{
	printf("Libusb_WriteRegister ->\n");

	if (hDeviceHandle == NULL) {
		return FALSE;
	}

	UINT res = -1;
	BOOL bResult = TRUE;

	res = libusb_control_transfer(hDeviceHandle, CTRL_OUT, FPD_CMD_WRITE_REG, 0, 0, dataTable, dataTableLength, 0);
	if (res < 0) {
		printf("usb_ctl -> usb_sensor_write_register error. (%d)\n", res);
		return FALSE;
	}
	else {
		printf("usb_ctl -> usb_sensor_write_register success. (%d)\n", res);
	}

	return bResult;
}

BOOL LibUSB_ReadRegister(
	libusb_device_handle *hDeviceHandle, 
	USHORT index, 
	UCHAR *data)
{
	printf("Libusb_ReadRegister ->\n");

	if (hDeviceHandle == NULL) {
		return FALSE;
	}

	UINT	res = -1;
	UCHAR	RegData;
	BOOL	bResult = TRUE;

	res = libusb_control_transfer(hDeviceHandle, CTRL_IN, FPD_CMD_READ_REG, 0, index, &RegData, 1, 0);
	*data = RegData;
	if (res < 0) {
		printf("control transfer IN error(%d)\n", res);
		return FALSE;
	}
	else {
		printf("control transfer IN ok, cmd(0x%.02X)\n", FPD_CMD_READ_REG);
		//printf("RegData(0x%.02X)\n", RegData);
		//printf("RegData(0x%.02X)\n", *data);
	}
	
	return bResult;
}

BOOL LibUSB_GetImage(
	libusb_device_handle *hDeviceHandle,
	UCHAR **img_data)
{
	printf("Libusb_GetImage ->\n");

	if (hDeviceHandle == NULL) {
		return FALSE;
	}

	BOOL bResult = TRUE;

	UINT res = -1;
	UINT hdr_idx = 0;
	UINT dat_idx = 1;
	UINT dat_len;

	UINT hdr_cnt = 0;

	UCHAR buf_trans[TRANSFER_SIZE];

	UINT len;

	res = libusb_bulk_transfer(hDeviceHandle, 0x81, buf_trans, TRANSFER_SIZE, (int*)&len, 0);
	if (res < 0) {
		printf("libusb_bulk_transfer error. (%d)\n", res);
		return FALSE;
	}
	else {
		printf("Packet length ----------------- packet index(%d) len(%d)\n", transfer_cnt++, len);
		
		while (hdr_idx < len) {
			if ((buf_trans[hdr_idx] & 0x02) != 0x02) {
				if ((buf_trans[hdr_idx] & 0x01) != frame_toggle) {
					printf("Total transfer ----------------- pixel(%d) total pocket(%d)\n", pixel_cnt, transfer_cnt);
					
					*img_data = buf_finger;	

					res = 1;
					pixel_cnt = 0;
					transfer_cnt = 0;

					frame_toggle = (buf_trans[hdr_idx] & 0x01);					
				}

				dat_len = len - dat_idx;
				dat_len = (dat_len > PAYLOAD_SIZE ? PAYLOAD_SIZE : dat_len);

				if ((pixel_cnt + dat_len) <= _301_FRAME_SIZE) {
					memcpy(&buf_finger[pixel_cnt], &buf_trans[dat_idx], dat_len);
					pixel_cnt += dat_len;
				}

				hdr_idx += 64;
				dat_idx += 64;
			}
			else {
				++hdr_idx;
				++dat_len;
			}
		}	
	}

	return bResult;
}

struct libusb_device_handle *get_dev_handle()
{
	return dev;
}