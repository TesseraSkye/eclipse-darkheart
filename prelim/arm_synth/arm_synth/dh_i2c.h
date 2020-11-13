#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define BUSSTATE_UNKNOWN 0
#define BUSSTATE_IDLE 1
#define BUSSTATE_OWNER 2
#define BUSSTATE_BUSY 3

#define DH_SERCOM SERCOM0
#define DH_SERCOM_APBCMASK PM_APBCMASK_SERCOM0
#define DH_I2C_GCLK GCLK_CLKCTRL_GEN_GCLK0
#define DH_I2C_GCLK_CLKCTRL_ID GCLK_CLKCTRL_ID_SERCOM0_CORE
#define DH_I2C_BAUD 400000								// bus speed (400k for fast)
#define DH_I2C_GCLK_FREQ 8000000						// master clock, 8MHz
#define DH_I2C_RISE_TIME 300

void setupI2C(void);

enum DH_i2c_res {
	DH_I2C_RES_SUCCESS = 0,
	DH_I2C_RES_ERR_ADDR_NACK = -1,
	DH_I2C_RES_ERR_BUSSTATE = -2,
	DH_I2C_RES_ERR_BUSERR = -3,
	DH_I2C_RES_ERR_DATA_NACK = -4,
};

enum DH_i2c_res DH_i2c_write(uint8_t address, uint8_t *data, size_t len);