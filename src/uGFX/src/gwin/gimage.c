/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file	src/gwin/gimage.c
 * @brief	GWIN sub-system image code
 */

#include "gfx.h"

#if GFX_USE_GWIN && GWIN_NEED_IMAGE

#include "src/gwin/class_gwin.h"

#define widget(gh)	((GImageObject *)gh)

static void _destroy(GWindowObject *gh) {
	if (gdispImageIsOpen(&widget(gh)->image))
		gdispImageClose(&widget(gh)->image);
}

#if GWIN_NEED_IMAGE_ANIMATION
	static void _timer(void *param) {
		_gwinUpdate((GHandle)param);
	}
#endif

static void _redraw(GHandle gh) {
	coord_t		x, y, w, h, dx, dy;
	color_t		bg;
	#if GWIN_NEED_IMAGE_ANIMATION
		delaytime_t	delay;
	#endif

	// The default display area
	dx = 0;
	dy = 0;
	x = gh->x;
	y = gh->y;
	w = gh->width;
	h = gh->height;
	bg = gwinGetDefaultBgColor();

	// If the image isn't open just clear the area
	if (!gdispImageIsOpen(&widget(gh)->image)) {
		gdispGFillArea(gh->display, x, y, w, h, bg);
		return;
	}

	// Center horizontally if the area is larger than the image
	if (widget(gh)->image.width < w) {
		w = widget(gh)->image.width;
		dx = (gh->width-w)/2;
		x += dx;
		if (dx)
			gdispGFillArea(gh->display, gh->x, y, dx, h, bg);
		gdispGFillArea(gh->display, x+w, y, gh->width-dx-w, h, bg);
		dx = 0;
	}

	// Center image horizontally if the area is smaller than the image
	else if (widget(gh)->image.width > w) {
		dx = (widget(gh)->image.width - w)/2;
	}

	// Center vertically if the area is larger than the image
	if (widget(gh)->image.height < h) {
		h = widget(gh)->image.height;
		dy = (gh->height-h)/2;
		y += dy;
		if (dy)
			gdispGFillArea(gh->display, x, gh->y, w, dy, bg);
		gdispGFillArea(gh->display, x, y+h, w, gh->height-dy-h, bg);
		dy = 0;
	}

	// Center image vertically if the area is smaller than the image
	else if (widget(gh)->image.height > h) {
		dy = (widget(gh)->image.height - h)/2;
	}

	// Reset the background color in case it has changed
	gdispImageSetBgColor(&widget(gh)->image, bg);

	// Display the image
	gdispGImageDraw(gh->display, &widget(gh)->image, x, y, w, h, dx, dy);

	#if GWIN_NEED_IMAGE_ANIMATION
		// read the delay for the next frame
		delay = gdispImageNext(&widget((GHandle)gh)->image);

		// Wait for that delay if required
		switch(delay) {
		case TIME_INFINITE:
			// Everything is done
			break;
		case TIME_IMMEDIATE:
			// We can't allow a continuous loop here as it would lock the system up so we delay for the minimum period
			delay = 1;
			// Fall through
		default:
			// Start the timer to draw the next frame of the animation
			gtimerStart(&widget((GHandle)gh)->timer, _timer, (void*)gh, FALSE, delay);
			break;
		}
	#endif
}

static const gwinVMT imageVMT = {
	"Image",					// The class name
	sizeof(GImageObject),		// The object size
	_destroy,					// The destroy routine
	_redraw,					// The redraw routine
	0,							// The after-clear routine
};

GHandle gwinGImageCreate(GDisplay *g, GImageObject *gobj, GWindowInit *pInit) {
	if (!(gobj = (GImageObject *)_gwindowCreate(g, &gobj->g, pInit, &imageVMT, 0)))
		return 0;

	// Ensure the gdispImageIsOpen() gives valid results
	gdispImageInit(&gobj->image);

	// Initialise the timer
	#if GWIN_NEED_IMAGE_ANIMATION
		gtimerInit(&gobj->timer);
	#endif
	
	gwinSetVisible((GHandle)gobj, pInit->show);

	return (GHandle)gobj;
}

bool_t gwinImageOpenGFile(GHandle gh, GFILE *f) {
	// is it a valid handle?
	if (gh->vmt != (gwinVMT *)&imageVMT)
		return FALSE;

	if (gdispImageIsOpen(&widget(gh)->image))
		gdispImageClose(&widget(gh)->image);

	if ((gdispImageOpenGFile(&widget(gh)->image, f) & GDISP_IMAGE_ERR_UNRECOVERABLE))
		return FALSE;

	_gwinUpdate(gh);

	return TRUE;
}

gdispImageError gwinImageCache(GHandle gh) {
	// is it a valid handle?
	if (gh->vmt != (gwinVMT *)&imageVMT)
		return GDISP_IMAGE_ERR_BADFORMAT;

	return gdispImageCache(&widget(gh)->image);
}

#endif // GFX_USE_GWIN && GWIN_NEED_IMAGE
