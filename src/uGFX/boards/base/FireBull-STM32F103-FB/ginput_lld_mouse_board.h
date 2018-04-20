/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#ifndef _GINPUT_LLD_MOUSE_BOARD_H
#define _GINPUT_LLD_MOUSE_BOARD_H

static const SPIConfig spicfg = { 
    0,
	GPIOC, 
    6,
    /* SPI_CR1_BR_2 |*/ SPI_CR1_BR_1 | SPI_CR1_BR_0,
};

static inline void init_board(void)
{
	spiStart(&SPID1, &spicfg);
}

static inline bool_t getpin_pressed(void)
{
	return (!palReadPad(GPIOC, 4));
}

static inline void aquire_bus(void)
{
	spiAcquireBus(&SPID1);
    palClearPad(GPIOC, 6);
}

static inline void release_bus(void)
{
	palSetPad(GPIOC, 6);
	spiReleaseBus(&SPID1);
}

static inline uint16_t read_value(uint16_t port)
{
    static uint8_t txbuf[3] = {0};
    static uint8_t rxbuf[3] = {0};
    uint16_t ret;

    txbuf[0] = port;

    spiExchange(&SPID1, 3, txbuf, rxbuf);

    ret = (rxbuf[1] << 5) | (rxbuf[2] >> 3); 
    
	return ret;
}

#endif /* _GINPUT_LLD_MOUSE_BOARD_H */

