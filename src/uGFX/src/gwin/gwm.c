/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#include "gfx.h"

#if GFX_USE_GWIN && !GWIN_NEED_WINDOWMANAGER
	/**
	 * A really nasty default implementation for the simplest of systems
	 */


	#include "src/gwin/class_gwin.h"

	// Needed if there is no window manager
	#define MIN_WIN_WIDTH	1
	#define MIN_WIN_HEIGHT	1

	static gfxMutex		gmutex;

	void _gwmInit(void)	{
		gfxMutexInit(&gmutex);
	}

	void _gwmDeinit(void) {
		gfxMutexDestroy(&gmutex);
	}

	bool_t _gwinWMAdd(GHandle gh, const GWindowInit *pInit) {
		gh->x = gh->y = gh->width = gh->height = 0;
		gwinMove(gh, pInit->x, pInit->y);
		gwinResize(gh, pInit->width, pInit->height);
		return TRUE;
	}

	void _gwinFlushRedraws(GRedrawMethod how) {
		(void) how;

		// We are always flushed
	}


	#if GDISP_NEED_CLIP
		static void getLock(GHandle gh) {
			gfxMutexEnter(&gmutex);
			gdispGSetClip(gh->display, gh->x, gh->y, gh->width, gh->height);
		}
		static void exitLock(GHandle gh) {
			gdispGUnsetClip(gh->display);
			gfxMutexExit(&gmutex);
		}
	#else
		#define getLock(gh)		gfxMutexEnter(&gmutex)
		#define exitLock(gh)	gfxMutexExit(&gmutex)
	#endif

	void _gwinUpdate(GHandle gh) {
		if ((gh->flags & GWIN_FLG_SYSVISIBLE)) {
			if (gh->vmt->Redraw) {
				getLock(gh);
				gh->vmt->Redraw(gh);
				exitLock(gh);
			} else if ((gh->flags & GWIN_FLG_BGREDRAW)) {
				getLock(gh);
				gdispGFillArea(gh->display, gh->x, gh->y, gh->width, gh->height, gh->bgcolor);
				exitLock(gh);
				if (gh->vmt->AfterClear)
					gh->vmt->AfterClear(gh);
			}
		} else if ((gh->flags & GWIN_FLG_BGREDRAW)) {
			getLock(gh);
			gdispGFillArea(gh->display, gh->x, gh->y, gh->width, gh->height, gwinGetDefaultBgColor());
			exitLock(gh);
		}
		gh->flags &= ~(GWIN_FLG_NEEDREDRAW|GWIN_FLG_BGREDRAW);
	}

	bool_t _gwinDrawStart(GHandle gh) {
		if (!(gh->flags & GWIN_FLG_SYSVISIBLE))
			return FALSE;

		getLock(gh);
		return TRUE;
	}

	void _gwinDrawEnd(GHandle gh) {
		(void)	gh;
		exitLock(gh);
	}

	void gwinSetVisible(GHandle gh, bool_t visible) {
		if (visible) {
			if (!(gh->flags & GWIN_FLG_VISIBLE)) {
				gh->flags |= (GWIN_FLG_VISIBLE|GWIN_FLG_SYSVISIBLE|GWIN_FLG_BGREDRAW);
				_gwinUpdate(gh);
			}
		} else {
			if ((gh->flags & GWIN_FLG_VISIBLE)) {
				gh->flags &= ~(GWIN_FLG_VISIBLE|GWIN_FLG_SYSVISIBLE);
				gh->flags |= GWIN_FLG_BGREDRAW;
				_gwinUpdate(gh);
			}
		}
	}

	void gwinSetEnabled(GHandle gh, bool_t enabled) {
		if (enabled) {
			if (!(gh->flags & GWIN_FLG_ENABLED)) {
				gh->flags |= (GWIN_FLG_ENABLED|GWIN_FLG_SYSENABLED);
				_gwinUpdate(gh);
			}
		} else {
			if ((gh->flags & GWIN_FLG_ENABLED)) {
				gh->flags &= ~(GWIN_FLG_ENABLED|GWIN_FLG_SYSENABLED);
				_gwinUpdate(gh);
			}
		}
	}

	void gwinMove(GHandle gh, coord_t x, coord_t y) {
		gh->x = x; gh->y = y;
		if (gh->x < 0) gh->x = 0;
		if (gh->y < 0) gh->y = 0;
		if (gh->x > gdispGGetWidth(gh->display)-MIN_WIN_WIDTH)		gh->x = gdispGGetWidth(gh->display)-MIN_WIN_WIDTH;
		if (gh->y > gdispGGetHeight(gh->display)-MIN_WIN_HEIGHT)	gh->y = gdispGGetHeight(gh->display)-MIN_WIN_HEIGHT;
		if (gh->x+gh->width > gdispGGetWidth(gh->display)) 			gh->width = gdispGGetWidth(gh->display) - gh->x;
		if (gh->y+gh->height > gdispGGetHeight(gh->display)) 		gh->height = gdispGGetHeight(gh->display) - gh->y;
		_gwinUpdate(gh);
	}

	void gwinResize(GHandle gh, coord_t width, coord_t height) {
		gh->width = width; gh->height = height;
		if (gh->width < MIN_WIN_WIDTH) { gh->width = MIN_WIN_WIDTH; }
		if (gh->height < MIN_WIN_HEIGHT) { gh->height = MIN_WIN_HEIGHT; }
		if (gh->x+gh->width > gdispGGetWidth(gh->display))		gh->width = gdispGGetWidth(gh->display) - gh->x;
		if (gh->y+gh->height > gdispGGetHeight(gh->display))	gh->height = gdispGGetHeight(gh->display) - gh->y;
		_gwinUpdate(gh);
	}

	void gwinRedraw(GHandle gh) {
		_gwinUpdate(gh);
	}
#endif

#if GFX_USE_GWIN && GWIN_NEED_WINDOWMANAGER

#include "src/gwin/class_gwin.h"

/*-----------------------------------------------
 * Data
 *-----------------------------------------------*/

// The default window manager
extern const GWindowManager	GNullWindowManager;
GWindowManager *			_GWINwm;

static gfxSem				gwinsem;
static gfxQueueASync		_GWINList;
#if !GWIN_REDRAW_IMMEDIATE
	static GTimer			RedrawTimer;
	static void				RedrawTimerFn(void *param);
#endif
static volatile uint8_t		RedrawPending;
	#define DOREDRAW_INVISIBLES		0x01
	#define DOREDRAW_VISIBLES		0x02


/*-----------------------------------------------
 * Window Routines
 *-----------------------------------------------*/

void _gwmInit(void)
{
	gfxSemInit(&gwinsem, 1, 1);
	gfxQueueASyncInit(&_GWINList);
	#if !GWIN_REDRAW_IMMEDIATE
		gtimerInit(&RedrawTimer);
		gtimerStart(&RedrawTimer, RedrawTimerFn, 0, TRUE, TIME_INFINITE);
	#endif
	_GWINwm = (GWindowManager *)&GNullWindowManager;
	_GWINwm->vmt->Init();
}

void _gwmDeinit(void)
{
	GHandle		gh;

	while((gh = gwinGetNextWindow(0)))
		gwinDestroy(gh);

	_GWINwm->vmt->DeInit();
	#if !GWIN_REDRAW_IMMEDIATE
		gtimerDeinit(&RedrawTimer);
	#endif
	gfxQueueASyncDeinit(&_GWINList);
	gfxSemDestroy(&gwinsem);
}

#if GWIN_REDRAW_IMMEDIATE
	#define TriggerRedraw(void) _gwinFlushRedraws(REDRAW_NOWAIT);
#else
	#define TriggerRedraw()		gtimerJab(&RedrawTimer);

	static void RedrawTimerFn(void *param) {
		(void)		param;
		_gwinFlushRedraws(REDRAW_NOWAIT);
	}
#endif

void _gwinFlushRedraws(GRedrawMethod how) {
	GHandle		gh;

	// Do we really need to do anything?
	if (!RedrawPending)
		return;

	// Obtain the drawing lock
	if (how == REDRAW_WAIT)
		gfxSemWait(&gwinsem, TIME_INFINITE);
	else if (how == REDRAW_NOWAIT && !gfxSemWait(&gwinsem, TIME_IMMEDIATE))
		// Someone is drawing - They will do the redraw when they are finished
		return;

	// Do loss of visibility first
	while ((RedrawPending & DOREDRAW_INVISIBLES)) {
		RedrawPending &= ~DOREDRAW_INVISIBLES;				// Catch new requests

		for(gh = gwinGetNextWindow(0); gh; gh = gwinGetNextWindow(gh)) {
			if ((gh->flags & (GWIN_FLG_NEEDREDRAW|GWIN_FLG_SYSVISIBLE)) != GWIN_FLG_NEEDREDRAW)
				continue;

			// Do the redraw
			#if GDISP_NEED_CLIP
				gdispGSetClip(gh->display, gh->x, gh->y, gh->width, gh->height);
				_GWINwm->vmt->Redraw(gh);
				gdispGUnsetClip(gh->display);
			#else
				_GWINwm->vmt->Redraw(gh);
			#endif

			// Postpone further redraws
			#if !GWIN_REDRAW_IMMEDIATE && !GWIN_REDRAW_SINGLEOP
				if (how == REDRAW_NOWAIT) {
					RedrawPending |= DOREDRAW_INVISIBLES;
					TriggerRedraw();
					goto releaselock;
				}
			#endif
		}
	}

	// Do the visible windows next
	while ((RedrawPending & DOREDRAW_VISIBLES)) {
		RedrawPending &= ~DOREDRAW_VISIBLES;				// Catch new requests

		for(gh = gwinGetNextWindow(0); gh; gh = gwinGetNextWindow(gh)) {
			if ((gh->flags & (GWIN_FLG_NEEDREDRAW|GWIN_FLG_SYSVISIBLE)) != (GWIN_FLG_NEEDREDRAW|GWIN_FLG_SYSVISIBLE))
				continue;

			// Do the redraw
			#if GDISP_NEED_CLIP
				gdispGSetClip(gh->display, gh->x, gh->y, gh->width, gh->height);
				_GWINwm->vmt->Redraw(gh);
				gdispGUnsetClip(gh->display);
			#else
				_GWINwm->vmt->Redraw(gh);
			#endif

			// Postpone further redraws (if there are any and the options are set right)
			#if !GWIN_REDRAW_IMMEDIATE && !GWIN_REDRAW_SINGLEOP
				if (how == REDRAW_NOWAIT) {
					while((gh = gwinGetNextWindow(gh))) {
						if ((gh->flags & (GWIN_FLG_NEEDREDRAW|GWIN_FLG_SYSVISIBLE)) == (GWIN_FLG_NEEDREDRAW|GWIN_FLG_SYSVISIBLE)) {
							RedrawPending |= DOREDRAW_VISIBLES;
							TriggerRedraw();
							break;
						}
					}
					goto releaselock;
				}
			#endif
		}
	}

	#if !GWIN_REDRAW_IMMEDIATE && !GWIN_REDRAW_SINGLEOP
		releaselock:
	#endif

	// Release the lock
	if (how == REDRAW_WAIT || how == REDRAW_NOWAIT)
		gfxSemSignal(&gwinsem);
}

void _gwinUpdate(GHandle gh) {
	// Only redraw if visible
	if (!(gh->flags & GWIN_FLG_SYSVISIBLE))
		return;

	// Mark for redraw
	gh->flags |= GWIN_FLG_NEEDREDRAW;
	RedrawPending |= DOREDRAW_VISIBLES;

	// Asynchronous redraw
	TriggerRedraw();
}

bool_t _gwinDrawStart(GHandle gh) {
	// This test should occur inside the lock. We do this
	//	here as well as an early out (more efficient).
	if (!(gh->flags & GWIN_FLG_SYSVISIBLE))
		return FALSE;

	// Obtain the drawing lock
	gfxSemWait(&gwinsem, TIME_INFINITE);

	// Re-test visibility as we may have waited a while
	if (!(gh->flags & GWIN_FLG_SYSVISIBLE)) {
		_gwinDrawEnd(gh);
		return FALSE;
	}

	// OK - we are ready to draw.
	#if GDISP_NEED_CLIP
		gdispGSetClip(gh->display, gh->x, gh->y, gh->width, gh->height);
	#endif
	return TRUE;
}

void _gwinDrawEnd(GHandle gh) {
	// Ensure there is no clip set
	#if GDISP_NEED_CLIP
		gdispGUnsetClip(gh->display);
	#endif

	// Look for something to redraw
	_gwinFlushRedraws(REDRAW_INSESSION);

	// Release the lock
	gfxSemSignal(&gwinsem);
}

bool_t _gwinWMAdd(GHandle gh, const GWindowInit *pInit) {
	#if GWIN_NEED_CONTAINERS
		// Save the parent
		gh->parent = pInit->parent;

		// Ensure the display is consistent with any parents
		if (gh->parent && (!(gh->parent->flags & GWIN_FLG_CONTAINER) || gh->display != gh->parent->display))
			return FALSE;
	#endif

	// Add to the window manager
	if (!_GWINwm->vmt->Add(gh, pInit))
		return FALSE;

	#if GWIN_NEED_CONTAINERS
		// Notify the parent it has been added
		if (gh->parent && ((gcontainerVMT *)gh->parent->vmt)->NotifyAdd)
			((gcontainerVMT *)gh->parent->vmt)->NotifyAdd(gh->parent, gh);
	#endif

	return TRUE;
}

void gwinSetWindowManager(struct GWindowManager *gwm) {
	if (!gwm)
		gwm = (GWindowManager *)&GNullWindowManager;
	if (_GWINwm != gwm) {
		_GWINwm->vmt->DeInit();
		_GWINwm = gwm;
		_GWINwm->vmt->Init();
	}
}

void gwinRedraw(GHandle gh) {
	// Only redraw if visible
	if (!(gh->flags & GWIN_FLG_SYSVISIBLE))
		return;

	// Mark for redraw
	gh->flags |= GWIN_FLG_NEEDREDRAW;
	RedrawPending |= DOREDRAW_VISIBLES;

	// Synchronous redraw
	_gwinFlushRedraws(REDRAW_WAIT);
}

#if GWIN_NEED_CONTAINERS
	void gwinSetVisible(GHandle gh, bool_t visible) {
		if (visible) {
			// Mark us as visible
			gh->flags |= GWIN_FLG_VISIBLE;

			// Do we want to be added to the display
			if (!(gh->flags & GWIN_FLG_SYSVISIBLE) && (!gh->parent || (gh->parent->flags & GWIN_FLG_SYSVISIBLE))) {
				// Check each window's visibility is consistent with its parents
				for(gh = gwinGetNextWindow(0); gh; gh = gwinGetNextWindow(gh)) {
					if ((gh->flags & (GWIN_FLG_SYSVISIBLE|GWIN_FLG_VISIBLE)) == GWIN_FLG_VISIBLE && (!gh->parent || (gh->parent->flags & GWIN_FLG_SYSVISIBLE)))
						gh->flags |= (GWIN_FLG_SYSVISIBLE|GWIN_FLG_NEEDREDRAW|GWIN_FLG_BGREDRAW);		// Fix it and mark for redraw
				}

				// Mark for redraw
				RedrawPending |= DOREDRAW_VISIBLES;
				TriggerRedraw();
			}
		} else {
			// Mark us as not visible
			gh->flags &= ~GWIN_FLG_VISIBLE;

			// Do we need to be removed from the display
			if ((gh->flags & GWIN_FLG_SYSVISIBLE)) {
				gh->flags |= (GWIN_FLG_NEEDREDRAW|GWIN_FLG_BGREDRAW);

				// Check each window's visibility is consistent with its parents
				for(gh = gwinGetNextWindow(0); gh; gh = gwinGetNextWindow(gh)) {
					if ((gh->flags & GWIN_FLG_SYSVISIBLE) && (!(gh->flags & GWIN_FLG_VISIBLE) || (gh->parent && !(gh->parent->flags & GWIN_FLG_SYSVISIBLE))))
						gh->flags &= ~GWIN_FLG_SYSVISIBLE;				// Fix it
				}

				// Mark for redraw - no need to redraw children
				RedrawPending |= DOREDRAW_INVISIBLES;
				TriggerRedraw();
			}
		}
	}
#else
	void gwinSetVisible(GHandle gh, bool_t visible) {
		if (visible) {
			if (!(gh->flags & GWIN_FLG_VISIBLE)) {
				gh->flags |= (GWIN_FLG_VISIBLE|GWIN_FLG_SYSVISIBLE|GWIN_FLG_NEEDREDRAW|GWIN_FLG_BGREDRAW);
				RedrawPending |= DOREDRAW_VISIBLES;
				TriggerRedraw();
			}
		} else {
			if ((gh->flags & GWIN_FLG_VISIBLE)) {
				gh->flags &= ~(GWIN_FLG_VISIBLE|GWIN_FLG_SYSVISIBLE);
				gh->flags |= (GWIN_FLG_NEEDREDRAW|GWIN_FLG_BGREDRAW);
				RedrawPending |= DOREDRAW_INVISIBLES;
				TriggerRedraw();
			}
		}
	}
#endif

#if GWIN_NEED_CONTAINERS
	// These two sub-functions set/clear system enable recursively.
	void gwinSetEnabled(GHandle gh, bool_t enabled) {
		if (enabled) {
			// Mark us as enabled
			gh->flags |= GWIN_FLG_ENABLED;

			// Do we change our real enabled state
			if (!(gh->flags & GWIN_FLG_SYSENABLED) && (!gh->parent || (gh->parent->flags & GWIN_FLG_SYSENABLED))) {
				// Check each window's enabled state is consistent with its parents
				for(gh = gwinGetNextWindow(0); gh; gh = gwinGetNextWindow(gh)) {
					if ((gh->flags & (GWIN_FLG_SYSENABLED|GWIN_FLG_ENABLED)) == GWIN_FLG_ENABLED && (!gh->parent || (gh->parent->flags & GWIN_FLG_SYSENABLED))) {
						gh->flags |= GWIN_FLG_SYSENABLED;							// Fix it
						_gwinUpdate(gh);
					}
				}
			}
		} else {
			gh->flags &= ~GWIN_FLG_ENABLED;

			// Do we need to change our real enabled state
			if ((gh->flags & GWIN_FLG_SYSENABLED)) {
				// Check each window's visibility is consistent with its parents
				for(gh = gwinGetNextWindow(0); gh; gh = gwinGetNextWindow(gh)) {
					if ((gh->flags & GWIN_FLG_SYSENABLED) && (!(gh->flags & GWIN_FLG_ENABLED) || (gh->parent && !(gh->parent->flags & GWIN_FLG_SYSENABLED)))) {
						gh->flags &= ~GWIN_FLG_SYSENABLED;			// Fix it
						_gwinUpdate(gh);
					}
				}
			}
		}
	}
#else
	void gwinSetEnabled(GHandle gh, bool_t enabled) {
		if (enabled) {
			if (!(gh->flags & GWIN_FLG_ENABLED)) {
				gh->flags |= (GWIN_FLG_ENABLED|GWIN_FLG_SYSENABLED);
				_gwinUpdate(gh);
			}
		} else {
			if ((gh->flags & GWIN_FLG_ENABLED)) {
				gh->flags &= ~(GWIN_FLG_ENABLED|GWIN_FLG_SYSENABLED);
				_gwinUpdate(gh);
			}
		}
	}
#endif

void gwinMove(GHandle gh, coord_t x, coord_t y) {
	_GWINwm->vmt->Move(gh, x, y);
}

void gwinResize(GHandle gh, coord_t width, coord_t height) {
	_GWINwm->vmt->Size(gh, width, height);
}

void gwinSetMinMax(GHandle gh, GWindowMinMax minmax) {
	_GWINwm->vmt->MinMax(gh, minmax);
}

void gwinRaise(GHandle gh) {
	_GWINwm->vmt->Raise(gh);
}

GWindowMinMax gwinGetMinMax(GHandle gh) {
	if (gh->flags & GWIN_FLG_MINIMIZED)
		return GWIN_MINIMIZE;
	if (gh->flags & GWIN_FLG_MAXIMIZED)
		return GWIN_MAXIMIZE;
	return GWIN_NORMAL;
}

void gwinRedrawDisplay(GDisplay *g, bool_t preserve) {
	GHandle	gh;

	for(gh = gwinGetNextWindow(0); gh; gh = gwinGetNextWindow(gh)) {

		// Skip if it is for a different display
		if (g && gh->display != g)
			continue;

		#if GWIN_NEED_CONTAINERS
			// Skip if it is not a top level window (parents internally take care of their children)
			if (gh->parent)
				continue;
		#endif

		// Only visible windows are to be redrawn
		if (!(gh->flags & GWIN_FLG_SYSVISIBLE))
			continue;

		if (!preserve)
			gh->flags |= GWIN_FLG_BGREDRAW;

		_gwinUpdate(gh);
	}
}

GHandle gwinGetNextWindow(GHandle gh) {
	return gh ? (GHandle)gfxQueueASyncNext(&gh->wmq) : (GHandle)gfxQueueASyncPeek(&_GWINList);
}

/*-----------------------------------------------
 * "Null" Window Manager Routines
 *-----------------------------------------------*/

// This is a parent reveal operation
#define GWIN_FLG_PARENTREVEAL		(GWIN_FIRST_WM_FLAG << 0)

// Minimum dimensions
#define MIN_WIN_WIDTH	3
#define MIN_WIN_HEIGHT	3


static void WM_Init(void);
static void WM_DeInit(void);
static bool_t WM_Add(GHandle gh, const GWindowInit *pInit);
static void WM_Delete(GHandle gh);
static void WM_Redraw(GHandle gh);
static void WM_Size(GHandle gh, coord_t w, coord_t h);
static void WM_Move(GHandle gh, coord_t x, coord_t y);
static void WM_Raise(GHandle gh);
static void WM_MinMax(GHandle gh, GWindowMinMax minmax);

static const gwmVMT GNullWindowManagerVMT = {
	WM_Init,
	WM_DeInit,
	WM_Add,
	WM_Delete,
	WM_Redraw,
	WM_Size,
	WM_Move,
	WM_Raise,
	WM_MinMax,
};

const GWindowManager	GNullWindowManager = {
	&GNullWindowManagerVMT,
};

static void WM_Init(void) {
	// We don't need to do anything here.
	// A full window manager would move the windows around, add borders etc

	// clear the screen
	// cycle through the windows already defined displaying them
	// or cut all the window areas out of the screen and clear the remainder
}

static void WM_DeInit(void) {
	// We don't need to do anything here.
	// A full window manager would remove any borders etc
}

static bool_t WM_Add(GHandle gh, const GWindowInit *pInit) {
	// Note the window will not currently be marked as visible

	// Put it on the end of the queue
	gfxQueueASyncPut(&_GWINList, &gh->wmq);

	// Make sure the size/position is valid - prefer position over size.
	gh->width = MIN_WIN_WIDTH; gh->height = MIN_WIN_HEIGHT;
	gh->x = gh->y = 0;
	WM_Move(gh, pInit->x, pInit->y);
	WM_Size(gh, pInit->width, pInit->height);
	return TRUE;
}

static void WM_Delete(GHandle gh) {
	// Remove it from the window list
	gfxQueueASyncRemove(&_GWINList, &gh->wmq);
}

static void WM_Redraw(GHandle gh) {
	#if GWIN_NEED_CONTAINERS
		redo_redraw:
	#endif
	if ((gh->flags & GWIN_FLG_SYSVISIBLE)) {
		if (gh->vmt->Redraw)
			gh->vmt->Redraw(gh);
		else if ((gh->flags & GWIN_FLG_BGREDRAW)) {
			// We can't redraw but we want full coverage so just clear the area
			gdispGFillArea(gh->display, gh->x, gh->y, gh->width, gh->height, gh->bgcolor);

			// Only do an after clear if this is not a parent reveal
			if (!(gh->flags & GWIN_FLG_PARENTREVEAL) && gh->vmt->AfterClear)
				gh->vmt->AfterClear(gh);
		}

		// A real window manager would also redraw frame borders here for top level windows
		// For non-top level windows their parent is responsible for any borders

		// Redraw is done
		gh->flags &= ~(GWIN_FLG_NEEDREDRAW|GWIN_FLG_BGREDRAW|GWIN_FLG_PARENTREVEAL);

		#if GWIN_NEED_CONTAINERS
			// If this is container but not a parent reveal, mark any visible children for redraw
			//	We redraw our children here as we have overwritten them in redrawing the parent
			//	as GDISP/GWIN doesn't yet support complex clipping regions.
			if ((gh->flags & (GWIN_FLG_CONTAINER|GWIN_FLG_PARENTREVEAL)) == GWIN_FLG_CONTAINER) {
				for(gh = gwinGetFirstChild(gh); gh; gh = gwinGetSibling(gh))
					_gwinUpdate(gh);
			}
		#endif
	} else {
		if ((gh->flags & GWIN_FLG_BGREDRAW)) {
			#if GWIN_NEED_CONTAINERS
				if (gh->parent) {
					// Child redraw is done
					gh->flags &= ~(GWIN_FLG_NEEDREDRAW|GWIN_FLG_BGREDRAW|GWIN_FLG_PARENTREVEAL);

					// Get the parent to redraw the area
					gh = gh->parent;
					gh->flags |= (GWIN_FLG_BGREDRAW|GWIN_FLG_PARENTREVEAL);
					goto redo_redraw;
				}
			#endif
			gdispGFillArea(gh->display, gh->x, gh->y, gh->width, gh->height, gwinGetDefaultBgColor());
		}

		// Redraw is done
		gh->flags &= ~(GWIN_FLG_NEEDREDRAW|GWIN_FLG_BGREDRAW|GWIN_FLG_PARENTREVEAL);
	}
}

static void WM_Size(GHandle gh, coord_t w, coord_t h) {
	coord_t		v;

	#if GWIN_NEED_CONTAINERS
		if (gh->parent) {
			// Clip to the container
			v = gh->parent->x + gh->parent->width - ((const gcontainerVMT *)gh->parent->vmt)->RightBorder(gh->parent);
			if (gh->x+w > v)	w = v - gh->x;
			v = gh->parent->y + gh->parent->height - ((const gcontainerVMT *)gh->parent->vmt)->BottomBorder(gh->parent);
			if (gh->y+h > v) 	h = v - gh->y;
		}
	#endif

	// Clip to the screen
	v = gdispGGetWidth(gh->display);
	if (gh->x+w > v) 	w = v - gh->x;
	v = gdispGGetHeight(gh->display);
	if (gh->y+h > v) 	h = v - gh->y;

	// Give it a minimum size
	if (w < MIN_WIN_WIDTH)	w = MIN_WIN_WIDTH;
	if (h < MIN_WIN_HEIGHT)	h = MIN_WIN_HEIGHT;

	// If there has been no resize just exit
	if (gh->width == w && gh->height == h)
		return;

	// Set the new size and redraw
	if ((gh->flags & GWIN_FLG_SYSVISIBLE)) {
		if (w >= gh->width && h >= gh->height) {

			// The new size is larger - just redraw
			gh->width = w; gh->height = h;
			_gwinUpdate(gh);

		} else {
			// We need to make this window invisible and ensure that has been drawn
			gwinSetVisible(gh, FALSE);
			_gwinFlushRedraws(REDRAW_WAIT);

			// Resize
			gh->width = w; gh->height = h;

			#if GWIN_NEED_CONTAINERS
				// Any children outside the new area need to be moved
				if ((gh->flags & GWIN_FLG_CONTAINER)) {
					GHandle		child;

					// Move to their old relative location. THe WM_Move() will adjust as necessary
					for(child = gwinGetFirstChild(gh); child; child = gwinGetSibling(child))
						WM_Move(gh, child->x-gh->x-((const gcontainerVMT *)gh->parent->vmt)->LeftBorder(gh->parent), child->y-gh->y-((const gcontainerVMT *)gh->parent->vmt)->TopBorder(gh->parent));
				}
			#endif

			// Mark it visible again in its new location
			gwinSetVisible(gh, TRUE);
		}
	} else {
		gh->width = w; gh->height = h;

		#if GWIN_NEED_CONTAINERS
			// Any children outside the new area need to be moved
			if ((gh->flags & GWIN_FLG_CONTAINER)) {
				GHandle		child;

				// Move to their old relative location. THe WM_Move() will adjust as necessary
				for(child = gwinGetFirstChild(gh); child; child = gwinGetSibling(child))
					WM_Move(gh, child->x-gh->x-((const gcontainerVMT *)gh->parent->vmt)->LeftBorder(gh->parent), child->y-gh->y-((const gcontainerVMT *)gh->parent->vmt)->TopBorder(gh->parent));
			}
		#endif
	}
}

static void WM_Move(GHandle gh, coord_t x, coord_t y) {
	coord_t		v;

	#if GWIN_NEED_CONTAINERS
		if (gh->parent) {
			// Clip to the parent size
			v = gh->parent->width - ((const gcontainerVMT *)gh->parent->vmt)->LeftBorder(gh->parent) - ((const gcontainerVMT *)gh->parent->vmt)->RightBorder(gh->parent);
			if (x+gh->width > v)	x = v-gh->width;
			v = gh->parent->height - ((const gcontainerVMT *)gh->parent->vmt)->TopBorder(gh->parent) - ((const gcontainerVMT *)gh->parent->vmt)->BottomBorder(gh->parent);
			if (y+gh->height > v)	y = v-gh->height;
			if (x < 0) x = 0;
			if (y < 0) y = 0;

			// Convert to absolute position
			x += gh->parent->x + ((const gcontainerVMT *)gh->parent->vmt)->LeftBorder(gh->parent);
			y += gh->parent->y + ((const gcontainerVMT *)gh->parent->vmt)->TopBorder(gh->parent);
		}
	#endif

	// Clip to the screen
	v = gdispGGetWidth(gh->display);
	if (x+gh->width > v)	x = v-gh->width;
	v = gdispGGetHeight(gh->display);
	if (y+gh->height > v)	y = v-gh->height;
	if (x < 0) x = 0;
	if (y < 0) y = 0;

	// If there has been no move just exit
	if (gh->x == x && gh->y == y)
		return;

	// Clear the old area and then redraw
	if ((gh->flags & GWIN_FLG_SYSVISIBLE)) {
		// We need to make this window invisible and ensure that has been drawn
		gwinSetVisible(gh, FALSE);
		_gwinFlushRedraws(REDRAW_WAIT);

		// Do the move
		v = gh->x; gh->x = x; x = v;
		v = gh->y; gh->y = y; y = v;

		#if GWIN_NEED_CONTAINERS
			// Any children need to be moved
			if ((gh->flags & GWIN_FLG_CONTAINER)) {
				GHandle		child;

				// Move to their old relative location. THe WM_Move() will adjust as necessary
				for(child = gwinGetFirstChild(gh); child; child = gwinGetSibling(child))
					WM_Move(gh, child->x-x-((const gcontainerVMT *)gh->parent->vmt)->LeftBorder(gh->parent), child->y-y-((const gcontainerVMT *)gh->parent->vmt)->TopBorder(gh->parent));
			}
		#endif

		gwinSetVisible(gh, TRUE);
	} else {
		v = gh->x; gh->x = x; x = v;
		v = gh->y; gh->y = y; y = v;

		#if GWIN_NEED_CONTAINERS
			// Any children need to be moved
			if ((gh->flags & GWIN_FLG_CONTAINER)) {
				GHandle		child;

				// Move to their old relative location. THe WM_Move() will adjust as necessary
				for(child = gwinGetFirstChild(gh); child; child = gwinGetSibling(child))
					WM_Move(gh, child->x-x-((const gcontainerVMT *)gh->parent->vmt)->LeftBorder(gh->parent), child->y-y-((const gcontainerVMT *)gh->parent->vmt)->TopBorder(gh->parent));
			}
		#endif
	}
}

static void WM_MinMax(GHandle gh, GWindowMinMax minmax) {
	(void)gh; (void) minmax;
	// We don't support minimising, maximising or restoring
}

static void WM_Raise(GHandle gh) {
	// Take it off the list and then put it back on top
	// The order of the list then reflects the z-order.

	gfxQueueASyncRemove(&_GWINList, &gh->wmq);
	gfxQueueASyncPut(&_GWINList, &gh->wmq);

	// Redraw the window
	_gwinUpdate(gh);
}

#endif /* GFX_USE_GWIN && GWIN_NEED_WINDOWMANAGER */
/** @} */
