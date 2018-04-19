/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */


#include "framebuffer.h"

//#define GDISP_LLD_PIXELFORMAT					GDISP_PIXELFORMAT_RGB888
#define GDISP_LLD_PIXELFORMAT					GDISP_PIXELFORMAT_RGB565

// Set this to your frame buffer pixel format.
#ifndef GDISP_LLD_PIXELFORMAT
	#define GDISP_LLD_PIXELFORMAT		GDISP_PIXELFORMAT_BGR888
#endif

// Uncomment this if your frame buffer device requires flushing
//#define GDISP_HARDWARE_FLUSH		TRUE

#ifdef GDISP_DRIVER_VMT

static void board_init(GDisplay *g, fbInfo *fbi) {
	// Initialize your frame buffer device here

	if(fb_init(480, 272, 16) == 0)
	{
		while(1){}
	}

/*	unsigned short int *ptr;
	ptr = FrameBufferInfo.pointer;
	
	unsigned int k, l;
	for(k = 0; k < 600; k++)
	{
		for(l = 0; l < 800; l++)
		 {
		   *ptr = 0x07E0;
		   ptr++;
		 }
	}*/

		// TODO: Set the details of the frame buffer
		g->g.Width = 480;
		g->g.Height = 272;
		g->g.Backlight = 100;
		g->g.Contrast = 50;
		fbi->linelen = g->g.Width * sizeof(LLDCOLOR_TYPE);	// bytes per row
		fbi->pixels = FrameBufferInfo.pointer;				// pointer to the memory frame buffer
	}

	#if GDISP_HARDWARE_FLUSH
		static void board_flush(GDisplay *g) {
			// TODO: Can be an empty function if your hardware doesn't support this
			(void) g;
		}
	#endif

	#if GDISP_NEED_CONTROL
		static void board_backlight(GDisplay *g, uint8_t percent) {
			// TODO: Can be an empty function if your hardware doesn't support this
			(void) g;
			(void) percent;
		}

		static void board_contrast(GDisplay *g, uint8_t percent) {
			// TODO: Can be an empty function if your hardware doesn't support this
			(void) g;
			(void) percent;
		}

		static void board_power(GDisplay *g, powermode_t pwr) {
			// TODO: Can be an empty function if your hardware doesn't support this
			(void) g;
			(void) pwr;
		}
	#endif

#endif /* GDISP_LLD_BOARD_IMPLEMENTATION */
