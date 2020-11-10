#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define BUSSTATE_UNKNOWN 0
#define BUSSTATE_IDLE 1
#define BUSSTATE_OWNER 2
#define BUSSTATE_BUSY 3

#define ECLIPSE_SERCOM SERCOM0
#define ECLIPSE_SERCOM_APBCMASK PM_APBCMASK_SERCOM0
#define ECLIPSE_I2C_GCLK GCLK_CLKCTRL_GEN_GCLK0
#define ECLIPSE_I2C_GCLK_CLKCTRL_ID GCLK_CLKCTRL_ID_SERCOM0_CORE
#define ECLIPSE_I2C_BAUD 400000								// bus speed (400k for fast)
#define ECLIPSE_I2C_GCLK_FREQ 8000000						// master clock, 8MHz
#define ECLIPSE_I2C_RISE_TIME 300

void setupI2C(void);

enum ECLIPSE_i2c_res {
	ECLIPSE_I2C_RES_SUCCESS = 0,
	ECLIPSE_I2C_RES_ERR_ADDR_NACK = -1,
	ECLIPSE_I2C_RES_ERR_BUSSTATE = -2,
	ECLIPSE_I2C_RES_ERR_BUSERR = -3,
	ECLIPSE_I2C_RES_ERR_DATA_NACK = -4,
};

enum ECLIPSE_i2c_res ECLIPSE_i2c_write(uint8_t address, uint8_t *data, size_t len);