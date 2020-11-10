#include "sam.h"
#include "ECLIPSE_i2c.h"

void setupI2C(void) {


	// enable sercom peripheral bus mask
	PM->APBCMASK.reg |= ECLIPSE_SERCOM_APBCMASK;

	//turn on gclk for sercom
	GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | ECLIPSE_I2C_GCLK | ECLIPSE_I2C_GCLK_CLKCTRL_ID;

	//wait for gclk sync
	while (GCLK->STATUS.bit.SYNCBUSY) {};

	//reset sercom
	ECLIPSE_SERCOM->I2CM.CTRLA.bit.ENABLE = 0;
	while (ECLIPSE_SERCOM->I2CM.SYNCBUSY.bit.ENABLE) {};
	ECLIPSE_SERCOM->I2CM.CTRLA.bit.SWRST = 1;
	while (ECLIPSE_SERCOM->I2CM.SYNCBUSY.bit.SWRST || ECLIPSE_SERCOM->I2CM.SYNCBUSY.bit.ENABLE) {};

	//setup pin mux for i2c lines
	PORT->Group[0].PINCFG[14].reg |= PORT_PINCFG_PMUXEN;
	PORT->Group[0].PINCFG[15].reg |= PORT_PINCFG_PMUXEN;
	

	// set the mode for the I2C Pins
	PORT->Group[0].PMUX[14 >> 1].reg |= PORT_PMUX_PMUXE_C;
	PORT->Group[0].PMUX[15 >> 1].reg |= PORT_PMUX_PMUXO_C;


	//setup sercom - i2c stuff
	ECLIPSE_SERCOM->I2CM.CTRLA.bit.SWRST = 1;
	while (ECLIPSE_SERCOM->I2CM.SYNCBUSY.bit.SWRST || ECLIPSE_SERCOM->I2CM.CTRLA.bit.SWRST) {};

	ECLIPSE_SERCOM->I2CM.CTRLA.reg = SERCOM_I2CM_CTRLA_SPEED(0) |        // 0 is standard/fast mode 100 & 400kHz
	SERCOM_I2CM_CTRLA_SDAHOLD(0) |      // Hold SDA low for 300-600ns
	SERCOM_I2CM_CTRLA_MODE_I2C_MASTER;  // work as master

	//smart mode for autohandling of (n)ack and stuff
	ECLIPSE_SERCOM->I2CM.CTRLB.reg = SERCOM_I2CM_CTRLB_SMEN | SERCOM_I2CM_CTRLB_QCEN;
	while (ECLIPSE_SERCOM->I2CM.SYNCBUSY.bit.SYSOP) {};

	//set the baudrate with some clean code thx to arduino
	uint32_t baudrate = ECLIPSE_I2C_BAUD;      // Hz
	uint32_t clock_speed = ECLIPSE_I2C_GCLK_FREQ;  // Hz
	uint32_t rise_time = ECLIPSE_I2C_RISE_TIME;    // ns
	ECLIPSE_SERCOM->I2CM.BAUD.bit.BAUD =
	clock_speed / (2 * baudrate) - 5 - (((clock_speed / 1000000) * rise_time) / (2 * 1000));
	while (ECLIPSE_SERCOM->I2CM.SYNCBUSY.bit.SYSOP) {};

	//enable sercom
	ECLIPSE_SERCOM->I2CM.CTRLA.bit.ENABLE = 1;
	while (ECLIPSE_SERCOM->I2CM.SYNCBUSY.bit.ENABLE) {};

	//set bus to idle
	ECLIPSE_SERCOM->I2CM.STATUS.bit.BUSSTATE = BUSSTATE_IDLE;
	while (ECLIPSE_SERCOM->I2CM.SYNCBUSY.bit.SYSOP) {};
};

//create func for writing with i2c
enum ECLIPSE_i2c_res ECLIPSE_i2c_write(uint8_t address, uint8_t* data, size_t len) {
    /* Before trying to write, check to see if the bus is busy, if it is,
       bail.
    */
    if (ECLIPSE_SERCOM->I2CM.STATUS.bit.BUSSTATE == BUSSTATE_BUSY) {
        return false;
    }

    /* Address + write flag. */
    ECLIPSE_SERCOM->I2CM.ADDR.bit.ADDR = (address << 0x1ul) | 0;
    /* TODO: Consider a timeout here. */
    while (!ECLIPSE_SERCOM->I2CM.INTFLAG.bit.MB) {};

    /* Check for loss of bus or NACK - in either case we can't continue. */
    if (ECLIPSE_SERCOM->I2CM.STATUS.bit.BUSSTATE != BUSSTATE_OWNER) {
        return ECLIPSE_I2C_RES_ERR_BUSSTATE;
    }
    if (ECLIPSE_SERCOM->I2CM.STATUS.bit.RXNACK) {
        return ECLIPSE_I2C_RES_ERR_ADDR_NACK;
    }

    /* Send data bytes. */
    for (size_t i = 0; i < len; i++) {
        /* Send data and wait for TX complete. */
        ECLIPSE_SERCOM->I2CM.DATA.bit.DATA = data[i];

        while (!ECLIPSE_SERCOM->I2CM.INTFLAG.bit.MB) {
            /* Check for loss of arbitration or a bus error. We can't continue if those happen. */
            /* BUSERR is set in addition to ARBLOST if arbitration is lost, so just check that one. */
            if (ECLIPSE_SERCOM->I2CM.STATUS.bit.BUSERR) {
                return ECLIPSE_I2C_RES_ERR_BUSERR;
            }
        }

        /* If a nack is received we can not continue sending data. */
        if (ECLIPSE_SERCOM->I2CM.STATUS.bit.RXNACK) {
            return ECLIPSE_I2C_RES_ERR_DATA_NACK;
        }
    }

    /* Send STOP command. */
    ECLIPSE_SERCOM->I2CM.CTRLB.bit.CMD = 3;
    while (ECLIPSE_SERCOM->I2CM.SYNCBUSY.bit.SYSOP) {};

    return ECLIPSE_I2C_RES_SUCCESS;
};