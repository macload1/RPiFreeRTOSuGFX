/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file	src/gwin/list.c
 * @brief	GWIN list widget header file
 */

#include "gfx.h"

#if GFX_USE_GWIN && GWIN_NEED_LIST

#include "src/gwin/class_gwin.h"
#include <string.h>
#include <stdlib.h>

// user for the default drawing routine
#define SCROLLWIDTH			16	// the border from the scroll buttons to the frame
#define ARROW				10	// arrow side length
#define HORIZONTAL_PADDING	5	// extra horizontal padding for text
#define VERTICAL_PADDING	2	// extra vertical padding for text

// Macro's to assist in data type conversions
#define gh2obj		((GListObject *)gh)
#define gw2obj		((GListObject *)gw)
#define qi2li		((ListItem *)qi)
#define qix2li		((ListItem *)qix)
#define ple			((GEventGWinList *)pe)

// Flags for the GListObject
#define GLIST_FLG_MULTISELECT		(GWIN_FIRST_CONTROL_FLAG << 0)
#define GLIST_FLG_HASIMAGES			(GWIN_FIRST_CONTROL_FLAG << 1)
#define GLIST_FLG_SCROLLALWAYS		(GWIN_FIRST_CONTROL_FLAG << 2)
#define GLIST_FLG_SCROLLSMOOTH      (GWIN_FIRST_CONTROL_FLAG << 3)
#define GLIST_FLG_ENABLERENDER      (GWIN_FIRST_CONTROL_FLAG << 4)

// Flags on a ListItem.
#define GLIST_FLG_SELECTED			0x0001

typedef struct ListItem {
	gfxQueueASyncItem	q_item;		// This must be the first member in the struct

	uint16_t			flags;
	uint16_t			param;		// A parameter the user can specify himself
	const char*			text;
	#if GWIN_NEED_LIST_IMAGES
		gdispImage*		pimg;
	#endif
} ListItem;

static void sendListEvent(GWidgetObject *gw, int item) {
	GSourceListener*	psl;
	GEvent*				pe;

	// Trigger a GWIN list event
	psl = 0;

	while ((psl = geventGetSourceListener(GWIDGET_SOURCE, psl))) {
		if (!(pe = geventGetEventBuffer(psl)))
			continue;

		ple->type = GEVENT_GWIN_LIST;
		ple->list = (GHandle)gw;
		ple->item = item;

		geventSendEvent(psl);
	}
}

static void gwinListDefaultDraw(GWidgetObject* gw, void* param);

#if GINPUT_NEED_MOUSE
    static void MouseSelect(GWidgetObject* gw, coord_t x, coord_t y) {
        const gfxQueueASyncItem*    qi;
        int                         item, i;
        coord_t                     iheight;
        (void)						x;

        iheight = gdispGetFontMetric(gw->g.font, fontHeight) + VERTICAL_PADDING;

        // Handle click over the list area
        item = (gw2obj->top + y) / iheight;

        if (item < 0 || item >= gw2obj->cnt)
            return;

        for(qi = gfxQueueASyncPeek(&gw2obj->list_head), i = 0; qi; qi = gfxQueueASyncNext(qi), i++) {
            if ((gw->g.flags & GLIST_FLG_MULTISELECT)) {
                if (item == i) {
                    qi2li->flags ^= GLIST_FLG_SELECTED;
                    break;
                }
            } else {
                if (item == i)
                    qi2li->flags |= GLIST_FLG_SELECTED;
                else
                    qi2li->flags &=~ GLIST_FLG_SELECTED;
            }
        }

        _gwinUpdate(&gw->g);
        sendListEvent(gw, item);

    }

	// a mouse down has occurred over the list area
	static void MouseDown(GWidgetObject* gw, coord_t x, coord_t y) {
		coord_t		iheight, pgsz;

		// Save our mouse start position
        gw2obj->start_mouse_x = x;
        gw2obj->start_mouse_y = y;
		gw2obj->last_mouse_y = y;

		// For smooth scrolling, scrolling is done in the MouseMove and selection is done on MouseUp
		if (gw->g.flags & GLIST_FLG_SCROLLSMOOTH)
		    return;

		// Some initial stuff
		iheight = gdispGetFontMetric(gw->g.font, fontHeight) + VERTICAL_PADDING;
		pgsz = gw->g.height-2;

		// Handle click over the scroll bar
		if (x >= gw->g.width-(SCROLLWIDTH+2) && (gw2obj->cnt > pgsz/iheight || (gw->g.flags & GLIST_FLG_SCROLLALWAYS))) {
			if (y < 2*ARROW) {
				if (gw2obj->top > 0) {
					gw2obj->top -= iheight;
					if (gw2obj->top < 0)
					    gw2obj->top = 0;
					_gwinUpdate(&gw->g);
				}
			} else if (y >= gw->g.height - 2*ARROW) {
				if (gw2obj->top < gw2obj->cnt * iheight - pgsz) {
				    gw2obj->top += iheight;
				    if (gw2obj->top > gw2obj->cnt * iheight - pgsz)
				        gw2obj->top = gw2obj->cnt * iheight - pgsz;
				    _gwinUpdate(&gw->g);
				}
			} else if (y < gw->g.height/2) {
				if (gw2obj->top > 0) {
					if (gw2obj->top > pgsz)
						gw2obj->top -= pgsz;
					else
						gw2obj->top = 0;
					_gwinUpdate(&gw->g);
				}
			} else {
				if (gw2obj->top < gw2obj->cnt * iheight - pgsz) {
					if (gw2obj->top < gw2obj->cnt * iheight - 2*pgsz)
						gw2obj->top += pgsz;
					else
						gw2obj->top = gw2obj->cnt * iheight - pgsz;
					_gwinUpdate(&gw->g);
				}
			}
			return;
		}

        MouseSelect(gw, x, y);
	}

    static void MouseUp(GWidgetObject* gw, coord_t x, coord_t y) {
        // Only act when we are a smooth scrolling list
        if (!(gw->g.flags & GLIST_FLG_SCROLLSMOOTH))
            return;

        // Only allow selection when we did not scroll
        if (abs(gw2obj->start_mouse_x - x) > 4 || abs(gw2obj->start_mouse_y - y) > 4)
            return;

        MouseSelect(gw, x, y);
    }

	static void MouseMove(GWidgetObject* gw, coord_t x, coord_t y) {
        int iheight, oldtop;
        (void) x;

        if (!(gw->g.flags & GLIST_FLG_SCROLLSMOOTH)) return;

        if (gw2obj->last_mouse_y != y) {
            oldtop = gw2obj->top;
            iheight = gdispGetFontMetric(gw->g.font, fontHeight) + VERTICAL_PADDING;

            gw2obj->top -= y - gw2obj->last_mouse_y;
            if (gw2obj->top >= gw2obj->cnt * iheight - (gw->g.height-2))
                gw2obj->top = gw2obj->cnt * iheight - (gw->g.height-2) - 1;
            if (gw2obj->top < 0)
                gw2obj->top = 0;
            gw2obj->last_mouse_y = y;
            if (oldtop != gw2obj->top)
            	_gwinUpdate(&gw->g);
        }
	}
#endif

#if GINPUT_NEED_TOGGLE
	// a toggle-on has occurred
	static void ToggleOn(GWidgetObject *gw, uint16_t role) {
		const gfxQueueASyncItem	*	qi;
		const gfxQueueASyncItem	*	qix;
		int							i;

		switch (role) {
			// select down
			case 0:
				for (i = 0, qi = gfxQueueASyncPeek(&gw2obj->list_head); qi; qi = gfxQueueASyncNext(qi), i++) {
					if ((qi2li->flags & GLIST_FLG_SELECTED)) {
						qix = gfxQueueASyncNext(qi);
						if (qix) {
							qi2li->flags &=~ GLIST_FLG_SELECTED;
							qix2li->flags |= GLIST_FLG_SELECTED;
							_gwinUpdate(&gw->g);
						}
						break;
					}
				}
				break;

			// select up
			case 1:
				qi = gfxQueueASyncPeek(&gw2obj->list_head);
				qix = 0;

				for (i = 0; qi; qix = qi, qi = gfxQueueASyncNext(qi), i++) {
					if ((qi2li->flags & GLIST_FLG_SELECTED))
						if (qix) {
							qi2li->flags &=~ GLIST_FLG_SELECTED;
							qix2li->flags |= GLIST_FLG_SELECTED;
							_gwinUpdate(&gw->g);
						}
						break;
					}
				}
				break;
		}
	}

	static void ToggleAssign(GWidgetObject *gw, uint16_t role, uint16_t instance) {
		if (role)
			gw2obj->t_up = instance;
		else
			gw2obj->t_dn = instance;
	}

	static uint16_t ToggleGet(GWidgetObject *gw, uint16_t role) {
		return role ? gw2obj->t_up : gw2obj->t_dn;
	}
#endif

static void _destroy(GHandle gh) {
	const gfxQueueASyncItem* qi;

	while((qi = gfxQueueASyncGet(&gh2obj->list_head)))
		gfxFree((void *)qi);

	_gwidgetDestroy(gh);
}

static const gwidgetVMT listVMT = {
	{
		"List",					// The class name
		sizeof(GListObject),	// The object size
		_destroy,				// The destroy routine
		_gwidgetRedraw,			// The redraw routine
		0,						// The after-clear routine
	},
	gwinListDefaultDraw,		// default drawing routine
	#if GINPUT_NEED_MOUSE
		{
			MouseDown,
			MouseUp,
			MouseMove,
		},
	#endif
	#if GINPUT_NEED_TOGGLE
		{
			2,					// two toggle roles
			ToggleAssign,		// Assign toggles
			ToggleGet,			// get toggles
			0,
			ToggleOn,			// process toggle on event
		},
	#endif
	#if GINPUT_NEED_DIAL
		{
			0,
			0,
			0,
			0,
		},
	#endif
};

GHandle gwinGListCreate(GDisplay *g, GListObject* gobj, GWidgetInit* pInit, bool_t multiselect) {
	if (!(gobj = (GListObject *)_gwidgetCreate(g, &gobj->w, pInit, &listVMT)))
		return 0;

	// initialize the item queue
	gfxQueueASyncInit(&gobj->list_head);
	gobj->cnt = 0;
	gobj->top = 0;
	if (multiselect)
		gobj->w.g.flags |= GLIST_FLG_MULTISELECT;
	gobj->w.g.flags |= GLIST_FLG_SCROLLALWAYS;
	gobj->w.g.flags |= GLIST_FLG_ENABLERENDER;

	gwinSetVisible(&gobj->w.g, pInit->g.show);

	return (GHandle)gobj;
}

void gwinListEnableRender(GHandle gh, bool_t ena) {
	// is it a valid handle?
	if (gh->vmt != (gwinVMT *)&listVMT)
		return;

	if (ena) {
		gh->flags |= GLIST_FLG_ENABLERENDER;
		gwinRedraw(gh);
	} else {
		gh->flags &=~ GLIST_FLG_ENABLERENDER;
	}
}

void gwinListSetScroll(GHandle gh, scroll_t flag) {
	// is it a valid handle?
	if (gh->vmt != (gwinVMT *)&listVMT)
		return;

    ((GListObject*)gh)->w.g.flags &=~(GLIST_FLG_SCROLLSMOOTH | GLIST_FLG_SCROLLALWAYS);
	switch (flag) {
		case scrollAlways:
			((GListObject*)gh)->w.g.flags |= GLIST_FLG_SCROLLALWAYS;
			break;

		case scrollAuto:
			break;

		case scrollSmooth:
		    ((GListObject*)gh)->w.g.flags |= GLIST_FLG_SCROLLSMOOTH;
		    break;
	}
}

int gwinListAddItem(GHandle gh, const char* item_name, bool_t useAlloc) {
	ListItem	*newItem;

	// is it a valid handle?
	if (gh->vmt != (gwinVMT *)&listVMT)
		return -1;

	if (useAlloc) {
		size_t len = strlen(item_name)+1;
		if (!(newItem = gfxAlloc(sizeof(ListItem) + len)))
			return -1;

		memcpy((char *)(newItem+1), item_name, len);
		item_name = (const char *)(newItem+1);
	} else {
		if (!(newItem = gfxAlloc(sizeof(ListItem))))
			return -1;
	}

	// the item is not selected when added
	newItem->flags = 0;
	newItem->param = 0;
	newItem->text = item_name;
	#if GWIN_NEED_LIST_IMAGES
		newItem->pimg = 0;
	#endif

	// select the item if it's the first in the list
	if (gh2obj->cnt == 0 && !(gh->flags & GLIST_FLG_MULTISELECT))
		newItem->flags |= GLIST_FLG_SELECTED;

	// add the new item to the list
	gfxQueueASyncPut(&gh2obj->list_head, &newItem->q_item);

	// increment the total amount of entries in the list widget
	gh2obj->cnt++;

	_gwinUpdate(gh);

	// return the position in the list (-1 because we start with index 0)
	return gh2obj->cnt-1;
}

const char* gwinListItemGetText(GHandle gh, int item) {
	const gfxQueueASyncItem*	qi;
	int							i;

	// is it a valid handle?
	if (gh->vmt != (gwinVMT *)&listVMT)
		return 0;

	// watch out for an invalid item
	if (item < 0 || item >= gh2obj->cnt)
		return 0;

	for(qi = gfxQueueASyncPeek(&gh2obj->list_head), i = 0; qi; qi = gfxQueueASyncNext(qi), i++) {
		if (i == item)
			return qi2li->text;
	}
	return 0;
}

int gwinListFindText(GHandle gh, const char* text) {
	const gfxQueueASyncItem*	qi;
	int							i;

	// is it a valid handle?
	if (gh->vmt != (gwinVMT *)&listVMT)
		return -1;

	// watch out for NULL pointers
	if (!text)
		return -1;

	for(qi = gfxQueueASyncPeek(&gh2obj->list_head), i = 0; qi; qi = gfxQueueASyncNext(qi), i++) {
		if (strcmp(((ListItem *)qi)->text, text) == 0)
			return i;	
	}

	return -1;
}

int gwinListGetSelected(GHandle gh) {
	const gfxQueueASyncItem	*	qi;
	int							i;

	// is it a valid handle?
	if (gh->vmt != (gwinVMT *)&listVMT)
		return -1;

	// Multi-select always returns -1. Use gwinListItemIsSelected() instead
	if ((gh->flags & GLIST_FLG_MULTISELECT))
		return -1;

	for(qi = gfxQueueASyncPeek(&gh2obj->list_head), i = 0; qi; qi = gfxQueueASyncNext(qi), i++) {
		if (qi2li->flags & GLIST_FLG_SELECTED)
			return i;
	}

	return -1;
}

void gwinListItemSetParam(GHandle gh, int item, uint16_t param) {
	const gfxQueueASyncItem	*	qi;
	int							i;

	// is it a valid handle?
	if (gh->vmt != (gwinVMT *)&listVMT)
		return;

	// watch out for an invalid item
	if (item < 0 || item > (gh2obj->cnt) - 1)
		return;

	for(qi = gfxQueueASyncPeek(&gh2obj->list_head), i = 0; qi; qi = gfxQueueASyncNext(qi), i++) {
		if (i == item) {
			qi2li->param = param;
			break;
		}
	}
}

void gwinListDeleteAll(GHandle gh) {
	gfxQueueASyncItem* qi;

	// is it a valid handle?
	if (gh->vmt != (gwinVMT *)&listVMT)
		return;

	while((qi = gfxQueueASyncGet(&gh2obj->list_head)))
		gfxFree(qi);

	gh->flags &= ~GLIST_FLG_HASIMAGES;
	gh2obj->cnt = 0;
	gh2obj->top = 0;
	_gwinUpdate(gh);
}

void gwinListItemDelete(GHandle gh, int item) {
	const gfxQueueASyncItem	*	qi;
	int							i;

	// is it a valid handle?
	if (gh->vmt != (gwinVMT *)&listVMT)
		return;

	// watch out for an invalid item
	if (item < 0 || item >= gh2obj->cnt)
		return;

	for(qi = gfxQueueASyncPeek(&gh2obj->list_head), i = 0; qi; qi = gfxQueueASyncNext(qi), i++) {
		if (i == item) {
			gfxQueueASyncRemove(&gh2obj->list_head, (gfxQueueASyncItem*)qi);
			gfxFree((void *)qi);
			if (gh2obj->top >= item && gh2obj->top)
				gh2obj->top--;
			_gwinUpdate(gh);
			break;
		}
	}
}

uint16_t gwinListItemGetParam(GHandle gh, int item) {
	const gfxQueueASyncItem	*	qi;
	int							i;

	// is it a valid handle?
	if (gh->vmt != (gwinVMT *)&listVMT)
		return 0;

	// watch out for an invalid item
	if (item < 0 || item > (gh2obj->cnt) - 1)
		return 0;

	for(qi = gfxQueueASyncPeek(&gh2obj->list_head), i = 0; qi; qi = gfxQueueASyncNext(qi), i++) {
		if (i == item)
			return qi2li->param;
	}
	return 0;
}

bool_t gwinListItemIsSelected(GHandle gh, int item) {
	const gfxQueueASyncItem	*	qi;
	int							i;

	// is it a valid handle?
	if (gh->vmt != (gwinVMT *)&listVMT)
		return FALSE;

	// watch out for an invalid item
	if (item < 0 || item > (gh2obj->cnt) - 1)
		return FALSE;

	for(qi = gfxQueueASyncPeek(&gh2obj->list_head), i = 0; qi; qi = gfxQueueASyncNext(qi), i++) {
		if (i == item)
			return (qi2li->flags &  GLIST_FLG_SELECTED) ? TRUE : FALSE;
	}
	return FALSE;
}

int gwinListItemCount(GHandle gh) {
	// is it a valid handle?
	if (gh->vmt != (gwinVMT *)&listVMT)
		return 0;

	return gh2obj->cnt;
}

const char* gwinListGetSelectedText(GHandle gh) {
	// is it a valid handle?
	if (gh->vmt != (gwinVMT *)&listVMT)
		return 0;

	// return NULL if nothing is selected (or multi-select)
	if (gwinListGetSelected(gh) < 0)
		return 0;

	return gwinListItemGetText(gh, gwinListGetSelected(gh));
}

#if GWIN_NEED_LIST_IMAGES
	void gwinListItemSetImage(GHandle gh, int item, gdispImage *pimg) {
		const gfxQueueASyncItem	*	qi;
		int							i;

		// is it a valid handle?
		if (gh->vmt != (gwinVMT *)&listVMT)
			return;

		// watch out for an invalid item
		if (item < 0 || item > (gh2obj->cnt) - 1)
			return;

		for(qi = gfxQueueASyncPeek(&gh2obj->list_head), i = 0; qi; qi = gfxQueueASyncNext(qi), i++) {
			if (i == item) {
				qi2li->pimg = pimg;
				if (pimg)
					gh->flags |= GLIST_FLG_HASIMAGES;
				break;
			}
		}
	}
#endif

static void gwinListDefaultDraw(GWidgetObject* gw, void* param) {
	(void)param;

	#if GDISP_NEED_CONVEX_POLYGON
		static const point upArrow[] = { {0, ARROW}, {ARROW, ARROW}, {ARROW/2, 0} };
		static const point downArrow[] = { {0, 0}, {ARROW, 0}, {ARROW/2, ARROW} };
	#endif

	const gfxQueueASyncItem*	qi;
	int							i;
	coord_t						x, y, iheight, iwidth;
	color_t						fill;
	const GColorSet *			ps;
	#if GWIN_NEED_LIST_IMAGES
		coord_t					sy;
	#endif

	// is it a valid handle?
	if (gw->g.vmt != (gwinVMT *)&listVMT)
		return;

	// don't render if render has been disabled
	if (!(gw->g.flags & GLIST_FLG_ENABLERENDER))
		return;

	ps = (gw->g.flags & GWIN_FLG_SYSENABLED) ? &gw->pstyle->enabled : &gw->pstyle->disabled;
	iheight = gdispGetFontMetric(gw->g.font, fontHeight) + VERTICAL_PADDING;
	x = 1;

	// the scroll area
	if (gw->g.flags & GLIST_FLG_SCROLLSMOOTH) {
		iwidth = gw->g.width - 2 - 4;
		if (gw2obj->cnt > 0) {
			int max_scroll_value = gw2obj->cnt * iheight - gw->g.height-2;
			if (max_scroll_value > 0) {
				int bar_height = (gw->g.height-2) * (gw->g.height-2) / (gw2obj->cnt * iheight);
				gdispGFillArea(gw->g.display, gw->g.x + gw->g.width-4, gw->g.y + 1, 2, gw->g.height-1, gw->pstyle->background);
				gdispGFillArea(gw->g.display, gw->g.x + gw->g.width-4, gw->g.y + gw2obj->top * ((gw->g.height-2)-bar_height) / max_scroll_value, 2, bar_height, ps->edge);
			}
		}
	} else if ((gw2obj->cnt > (gw->g.height-2) / iheight) || (gw->g.flags & GLIST_FLG_SCROLLALWAYS)) {
		iwidth = gw->g.width - (SCROLLWIDTH+3);
		gdispGFillArea(gw->g.display, gw->g.x+iwidth+2, gw->g.y+1, SCROLLWIDTH, gw->g.height-2, gdispBlendColor(ps->fill, gw->pstyle->background, 128));
		gdispGDrawLine(gw->g.display, gw->g.x+iwidth+1, gw->g.y+1, gw->g.x+iwidth+1, gw->g.y+gw->g.height-2, ps->edge);
		#if GDISP_NEED_CONVEX_POLYGON
			gdispGFillConvexPoly(gw->g.display, gw->g.x+iwidth+((SCROLLWIDTH-ARROW)/2+2), gw->g.y+(ARROW/2+1), upArrow, 3, ps->fill);
			gdispGFillConvexPoly(gw->g.display, gw->g.x+iwidth+((SCROLLWIDTH-ARROW)/2+2), gw->g.y+gw->g.height-(ARROW+ARROW/2+1), downArrow, 3, ps->fill);
		#else
			#warning "GWIN: Lists display better when GDISP_NEED_CONVEX_POLYGON is turned on"
			gdispGFillArea(gw->g.display, gw->g.x+iwidth+((SCROLLWIDTH-ARROW)/2+2), gw->g.y+(ARROW/2+1), ARROW, ARROW, ps->fill);
			gdispGFillArea(gw->g.display, gw->g.x+iwidth+((SCROLLWIDTH-ARROW)/2+2), gw->g.y+gw->g.height-(ARROW+ARROW/2+1), ARROW, ARROW, ps->fill);
		#endif
	} else
		iwidth = gw->g.width - 2;

	#if GWIN_NEED_LIST_IMAGES
		if ((gw->g.flags & GLIST_FLG_HASIMAGES)) {
			x += iheight;
			iwidth -= iheight;
		}
	#endif


	// Find the top item
	for (qi = gfxQueueASyncPeek(&gw2obj->list_head), i = iheight - 1; i < gw2obj->top && qi; qi = gfxQueueASyncNext(qi), i+=iheight);

	// the list frame
	gdispGDrawBox(gw->g.display, gw->g.x, gw->g.y, gw->g.width, gw->g.height, ps->edge);

	// Set the clipping region so we do not override the frame.
	gdispGSetClip(gw->g.display, gw->g.x+1, gw->g.y+1, gw->g.width-2, gw->g.height-2);

	// Draw until we run out of room or items
	for (y = 1-(gw2obj->top%iheight); y < gw->g.height-2 && qi; qi = gfxQueueASyncNext(qi), y += iheight) {
		fill = (qi2li->flags & GLIST_FLG_SELECTED) ? ps->fill : gw->pstyle->background;
		gdispGFillArea(gw->g.display, gw->g.x+1, gw->g.y+y, iwidth, iheight, fill);
		#if GWIN_NEED_LIST_IMAGES
			if ((gw->g.flags & GLIST_FLG_HASIMAGES)) {
				// Clear the image area
				if (qi2li->pimg && gdispImageIsOpen(qi2li->pimg)) {
					// Calculate which image
					sy = (qi2li->flags & GLIST_FLG_SELECTED) ? 0 : (iheight-VERTICAL_PADDING);
					if (!(gw->g.flags & GWIN_FLG_SYSENABLED))
						sy += 2*(iheight-VERTICAL_PADDING);
					while (sy > qi2li->pimg->height)
						sy -= iheight-VERTICAL_PADDING;
					// Draw the image
					gdispImageSetBgColor(qi2li->pimg, fill);
					gdispGImageDraw(gw->g.display, qi2li->pimg, gw->g.x+1, gw->g.y+y, iheight-VERTICAL_PADDING, iheight-VERTICAL_PADDING, 0, sy);
				}
			}
		#endif
		gdispGFillStringBox(gw->g.display, gw->g.x+x+HORIZONTAL_PADDING, gw->g.y+y, iwidth-HORIZONTAL_PADDING, iheight, qi2li->text, gw->g.font, ps->text, fill, justifyLeft);
	}

	// Fill any remaining item space
	if (y < gw->g.height-1)
		gdispGFillArea(gw->g.display, gw->g.x+1, gw->g.y+y, iwidth, gw->g.height-1-y, gw->pstyle->background);
}

#endif // GFX_USE_GWIN && GWIN_NEED_LIST
