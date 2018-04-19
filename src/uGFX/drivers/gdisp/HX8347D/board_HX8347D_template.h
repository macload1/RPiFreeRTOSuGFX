/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#ifndef _GDISP_LLD_BOARD_H
#define _GDISP_LLD_BOARD_H

static inline void init_board(GDisplay *g) {
	(void) g;
}

static inline void post_init_board(GDisplay *g) {
	(void) g;
}

static inline void setpin_reset(GDisplay *g, bool_t state) {
	(void) g;
	(void) state;
}

static inline void set_backlight(GDisplay *g, uint8_t percent) {
	(void) g;
	(void) percent;
}

static inline void acquire_bus(GDisplay *g) {
	(void) g;
}

static inline void release_bus(GDisplay *g) {
	(void) g;
}

static inline void busmode16(GDisplay *g) {
	(void) g;
}

static inline void busmode8(GDisplay *g) {
	(void) g;
}

static inline void write_index(GDisplay *g, uint8_t index) {
	(void) g;
	(void) index;
}

static inline void write_data(GDisplay *g, uint8_t data) {
	(void) g;
	(void) data;
}

static inline void write_ram16(GDisplay *g, uint16_t data) {
	(void) g;
	(void) data;
}

#endif /* _GDISP_LLD_BOARD_H */
