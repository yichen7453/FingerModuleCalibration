#ifndef __SENSOR_CTL_H
#define __SENSOR_CTL_H

#include <string.h>
#include <stdint.h>

// ------+---------+---------+---------+---------+---------+---------+---------+
// export function return code
// ------+---------+---------+---------+---------+---------+---------+---------+

// Larry version
#if 1
#define _301_GAIN_INIT		0x02	// E0
#define _301_VRT_INIT		0x1A	// E1
#define _301_VRB_INIT		0x10	// E2
#define _301_DCOFFSET_INIT	0x12	// E6
#endif

// Sean version
#if 0
#define _301_GAIN_INIT		0x0F	// E0
#define _301_VRT_INIT		0x38	// E1
#define _301_VRB_INIT		0x00	// E2
#define _301_DCOFFSET_INIT	0x00	// E6
#endif

// Benjamin version
#if 0
#define _301_GAIN_INIT		0x08	// E0
#define _301_VRT_INIT		0x1D	// E1
#define _301_VRB_INIT		0x08	// E2
#define _301_DCOFFSET_INIT	0x11	// E6
#endif

// Phone calibration version
#if 0
#define _301_GAIN_INIT		0x00	// E0
#define _301_VRT_INIT		0x35	// E1
#define _301_VRB_INIT		0x08	// E2
#define _301_DCOFFSET_INIT	0x00	// E6
#endif

// Yi Test
#if 0
#define _301_GAIN_INIT		0x08	// E0
#define _301_VRT_INIT		0x2F	// E1
#define _301_VRB_INIT		0x0C	// E2
#define _301_DCOFFSET_INIT	0x13	// E6
#endif

// Brenda
#if 0
#define _301_GAIN_INIT		0x0E	// E0	0F
#define _301_VRT_INIT		0x1E	// E1
#define _301_VRB_INIT		0x00	// E2
#define _301_DCOFFSET_INIT	0x0D	// E6
#endif

#define FR_RDY_LIMIT		1000

#define SENSOR_GRID_LEN        9
#define SENSOR_HIST_LEN       16

#define GRID_STEP              3
#define GRID_WIDTH            42
#define GRID_HEIGHT           42

#define SENSOR_INTENSITY_WHITE         30


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


FLOAT calc_intensity2(unsigned char* image, int up, int down, int left, int right, unsigned char offset_width);

void calc_grid(uint8_t* fp_frame, int grid[]);
int check_grid(int grid[], int white_parts, int white_intensity);

#endif