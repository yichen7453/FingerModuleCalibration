#include "stdafx.h"

#include "sensor_ctl_301.h"
#include "libusb_ctl.h"
#include "winusb_ctl.h"
#include "sensor_init.h"


static uint8_t GAIN		= _301_GAIN_INIT;
static uint8_t VRT		= _301_VRT_INIT;
static uint8_t VRB		= _301_VRB_INIT;
static uint8_t DCOFFSET = _301_DCOFFSET_INIT;


static UCHAR _301_dvr_table[] = {
	0xE0, _301_GAIN_INIT,
	0xE1, _301_VRT_INIT,
	0xE2, _301_VRB_INIT,
	0xE6, _301_DCOFFSET_INIT
};

static UCHAR _301_init_table[] = {
	0xD2, 0x10,
	0xE0, _301_GAIN_INIT,
	0xE1, _301_VRT_INIT,
	0xE2, _301_VRB_INIT,
	0xE6, _301_DCOFFSET_INIT,
	0xF0, 0x01,
	0xF1, 0x06,
	0xD3, 0x01,
	0xD2, 0x13
};


#define HIST_SIZE            256

static uint32_t m_pHistVal[HIST_SIZE];
static uint32_t m_pHistCnt[HIST_SIZE];

BOOL Sensor_ReadId(
	BOOL usbType,
	UCHAR *chip_id);

BOOL Sensor_CheckModule(
	BOOL usbType,
	UINT *chipType);

BOOL Sensor_CheckFrameReady(
	BOOL usbType);

BOOL Sensor_Set_SwipeRegister(
	BOOL usbType);

BOOL Sensor_Set_AreaRegister(
	BOOL usbType);

BOOL Sensor_DropFrame(
	BOOL usbType);

BOOL Sensor_Calibration(
	BOOL usbType);


WINUSB_DEVICE_DATA	winusbDeviceData;
LIBUSB_DEVICE_DATA	libusbDeviceData;

BOOL Sensor_ReadId(
	BOOL usbType,
	UCHAR *chipId)
{
	printf("\nSensor_ReadId --> \n");

	BOOL bResult = TRUE;

	if (usbType) {
		return WinUsb_ReadRegister(winusbDeviceData.WinusbHandle, 0xFF, chipId);
	}
	else {
		return LibUSB_ReadRegister(libusbDeviceData.LibusbHandle, 0xFF, chipId);
	}
	return bResult;
}

BOOL Sensor_CheckModule(
	BOOL usbType,
	INT *chipType)
{
	printf("\n Sensor_CheckModule --> \n");

	UCHAR chipId;
	//UCHAR chipVersion;

	Sensor_ReadId(usbType, &chipId);
	printf("  chip id is %x\n", chipId);

	if (chipId == _301_ID || chipId == _301_ID2 || chipId == _301_ID3 || chipId == _301_ID4 || chipId == _301_ID5) {
		*chipType = 301;
		//usb_read_register(0xFD, &chip_version);
		//printf("chip version is %d\n", chip_version);

		//usb_write_register(_301_init_table, ARRAYSIZE(_301_init_table));
	}
	else if (chipId == _305_ID || chipId == _305_ID2 || chipId == _305_ID3) {
		*chipType = 305;
	}

	// Software reset
	if (usbType) {
		return WinUSB_WriteRegister(winusbDeviceData.WinusbHandle, reset_table, ARRAYSIZE(reset_table));
	}
	else {
		return LibUSB_WriteRegister(libusbDeviceData.LibusbHandle, reset_table, ARRAYSIZE(reset_table));
	}
}

BOOL Sensor_CheckFrameReady(
	BOOL usbType)
{
	UCHAR _d3;
	UINT count = 0;
	BOOL bResult = TRUE;

	while (1) {
		if (usbType) {
			bResult = WinUsb_ReadRegister(winusbDeviceData.WinusbHandle, 0xD3, &_d3);
		}
		else {
			bResult = LibUSB_ReadRegister(libusbDeviceData.LibusbHandle, 0xD3, &_d3);
		}

		if (bResult) {
			if (_d3 & 0x04) {
				break;
			}
			else {
				return FALSE;
			}
		}

		if (++count > FR_RDY_LIMIT) break;
	}

	return bResult;
}

BOOL Sensor_Set_SwipeRegister(
	BOOL usbType)
{
	BOOL bResult = TRUE;

	do {
		if (usbType) {
			bResult = WinUSB_WriteRegister(winusbDeviceData.WinusbHandle, _301_dvr_table, ARRAYSIZE(_301_dvr_table));
			bResult = WinUSB_WriteRegister(winusbDeviceData.WinusbHandle, _301_swipe_init_table, ARRAYSIZE(_301_swipe_init_table));
		}
		else {
			bResult = LibUSB_WriteRegister(libusbDeviceData.LibusbHandle, _301_dvr_table, ARRAYSIZE(_301_dvr_table));
			bResult = LibUSB_WriteRegister(libusbDeviceData.LibusbHandle, _301_swipe_init_table, ARRAYSIZE(_301_swipe_init_table));
		}
	} while (0);

	return bResult;
}

BOOL Sensor_Set_AreaRegister(
	BOOL usbType)
{
	BOOL bResult = TRUE;

	do {
		//ret = usb_write_register(_301_dvr_table, ARRAYSIZE(_301_dvr_table));
		if (usbType) {
			bResult = WinUSB_WriteRegister(winusbDeviceData.WinusbHandle, _301_touch_init_table, ARRAYSIZE(_301_touch_init_table));
		}
		else {
			bResult = LibUSB_WriteRegister(libusbDeviceData.LibusbHandle, _301_touch_init_table, ARRAYSIZE(_301_touch_init_table));
		}
	} while (0);

	return bResult;
}

BOOL Sensor_DropFrame(
	BOOL usbType)
{
	BOOL bResult = TRUE;

	do {
		bResult = Sensor_CheckFrameReady(usbType);
		if (!bResult) break;


		UCHAR d3[] = { 0xD3, 0x00 };
		if (usbType) {
			bResult = WinUSB_WriteRegister(winusbDeviceData.WinusbHandle, d3, ARRAYSIZE(d3));
		}
		else {
			bResult = LibUSB_WriteRegister(libusbDeviceData.LibusbHandle, d3, ARRAYSIZE(d3));
		}
		if (!bResult) break;
	} while (0);

	return bResult;
}

BOOL Sensor_Calibration(
	BOOL usbType)
{
	BOOL bResult = TRUE;

	UCHAR *img_data;
	ULONG transferred = 0;

	UINT max, min;

	UCHAR _301_cali_table[] = {
		0xE0, GAIN,
		0xE1, VRT,
		0xE2, VRB,
		0xE6, DCOFFSET
	};

	printf("  Cali Register -> %02x, %02x, %02x, %02x\n", GAIN, VRT, VRB, DCOFFSET);

	if (usbType) {
		bResult = WinUSB_WriteRegister(winusbDeviceData.WinusbHandle, _301_cali_table, ARRAYSIZE(_301_cali_table));
	}
	else {
		bResult = LibUSB_WriteRegister(libusbDeviceData.LibusbHandle, _301_cali_table, ARRAYSIZE(_301_cali_table));
	}

	Sleep(100);

	if (usbType) {
		bResult = WinUSB_GetImage(winusbDeviceData.WinusbHandle, &img_data, &transferred);
	}
	else {
		bResult = LibUSB_GetImage(libusbDeviceData.LibusbHandle, &img_data);
	}

	if (bResult) {
		for (UINT idx = 0; idx < _301_SWIPE_FRAME_SIZE; idx++) {
			img_data[idx] = 255 - img_data[idx];
		}

		max = min = img_data[0];

		for (UINT i = 0; i < _301_SWIPE_FRAME_SIZE; i++) {
			if (max < img_data[i]) {
				max = img_data[i];
			}
			else if (min > img_data[i]) {
				min = img_data[i];
			}
		}

		printf("  Max: %d, Min: %d\n", max, min);

		if ((180 < (max & 0xFF)) && (230 > (max & 0xFF)) && (20 < (min & 0xFF)) && (100 > (min & 0xFF))) {
			//ret = sensor_calibration_2();
			bResult = FALSE;
			printf("Final Register -> %02x, %02x, %02x, %02x\n", GAIN, VRT, VRB, DCOFFSET);
		}
		else {
			if (DCOFFSET == 0x14) {
				if (GAIN == 0x00) {
					bResult = FALSE;
					printf("Error Register -> %02x, %02x, %02x, %02x\n", GAIN, VRT, VRB, DCOFFSET);
				}
				else {
					GAIN--;
					DCOFFSET = 0x00;
				}
			}
			else {
				DCOFFSET++;
			}
		}
	}

	return bResult;
}


FLOAT calc_intensity2(
	UCHAR *image, 
	INT up, 
	INT down, 
	INT left, 
	INT right, 
	UCHAR offset_width)
{
	INT pixel_idx;
	INT pixel_cnt = 0;
	INT pixel_sum = 0;
	INT row, col;
	FLOAT per = 0.0;

	pixel_idx = up * offset_width + left;

	for (row = up; row <= down; row++) {
		for (col = left; col <= right; col++) {
			pixel_sum += image[pixel_idx++];
			++pixel_cnt;
			//printf("pixel_idx = %d ", pixel_idx);
		}
		pixel_idx += (offset_width - (right - left + 1));
		//printf("pixel_idx2 = %d ", pixel_idx);
	}
	per = (FLOAT)(pixel_sum / pixel_cnt);
	//printf(" per = %f\n", per);

	return per;
}


int calc_intensity(uint8_t * fp_frame, int up, int down, int left, int right)
{
	int row;
	int col;

	int pixel_idx;
	int pixel_num = 0;
	int pixel_sum = 0;

	// todo: parameter boundary check

	pixel_idx = up * _301_FRAME_SIZE + left;

	for (row = up; row <= down; row++) {
		for (col = left; col <= right; col++) {
			pixel_sum += fp_frame[pixel_idx++];
			++pixel_num;
		}
		pixel_idx += (_301_FRAME_WIDTH - (right - left + 1));
	}

	return (pixel_sum / pixel_num);
}

void calc_grid(uint8_t * fp_frame, int grid[])
{
	int grid_idx;
	int grid_cnt = 0;
	int axis_x = 1;
	int axis_y = 1;

	// check grid capacity

	for (grid_idx = 0; grid_idx < SENSOR_GRID_LEN; grid_idx++) {
		grid[grid_idx] = calc_intensity(fp_frame, axis_y, axis_y + GRID_HEIGHT, axis_x, axis_x + GRID_WIDTH);

		if (++grid_cnt < GRID_STEP) {
			axis_x += GRID_WIDTH;

		}
		else {
			grid_cnt = 0;
			axis_x = 1;
			axis_y += GRID_HEIGHT;
		}
	}
}

int check_grid(int grid[], int white_parts, int white_intensity)
{
	int ret = 0;
	int grid_idx = 0;
	int white_block = 0;

	for (grid_idx = 0; grid_idx <= SENSOR_GRID_LEN; grid_idx++) {
		if (grid[grid_idx] <= white_intensity) {
			++white_block;
		}
	}

	if (white_block >= white_parts) {
		ret = -1;
	}

	return (ret);
}
