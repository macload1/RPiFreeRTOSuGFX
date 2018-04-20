/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/*
 * @file    src/gwin/class_gwin.h
 * @brief   GWIN Graphic window subsystem header file.
 *
 * @defgroup Internal Internal
 * @ingroup GWIN
 *
 * @note	These definitions are normally not used by an application program. They are useful
 * 			only if you want to create your own custom GWIN window or widget.
 * @note	To access these definitions you must include "gwin/class_gwin.h" in your source file.
 *
 * @{
 */
#ifndef _CLASS_GWIN_H
#define _CLASS_GWIN_H

#if GFX_USE_GWIN || defined(__DOXYGEN__)

/**
 * @brief	The predefined flags for a Window
 * @{
 */
#define GWIN_FIRST_CONTROL_FLAG			0x00000001			// @< 8 bits free for the control to use
#define GWIN_FLG_VISIBLE				0x00000100			// @< The window is "visible"
#define GWIN_FLG_SYSVISIBLE				0x00000200			// @< The window is visible after parents are tested
#define GWIN_FLG_ENABLED				0x00000400			// @< The window is "enabled"
#define GWIN_FLG_SYSENABLED				0x00000800			// @< The window is enabled after parents are tested
#define GWIN_FLG_DYNAMIC				0x00001000			// @< The GWIN structure is allocated
#define GWIN_FLG_ALLOCTXT				0x00002000			// @< The text/label is allocated
#define GWIN_FLG_NEEDREDRAW				0x00004000			// @< Redraw is needed but has been delayed
#define GWIN_FLG_BGREDRAW				0x00008000			// @< On redraw, if not visible redraw the revealed under-side
#define GWIN_FLG_SUPERMASK				0x000F0000			// @< The bit mask to leave just the window superclass type
#define GWIN_FLG_WIDGET					0x00010000			// @< This is a widget
#define GWIN_FLG_CONTAINER				0x00020000			// @< This is a container
#define GWIN_FLG_MINIMIZED				0x00100000			// @< The window is minimized
#define GWIN_FLG_MAXIMIZED				0x00200000			// @< The window is maximized
#define GWIN_FLG_MOUSECAPTURE			0x00400000			// @< The window has captured the mouse
#define GWIN_FIRST_WM_FLAG				0x01000000			// @< 8 bits free for the window manager to use
/** @} */

/**
 * @brief	The Virtual Method Table for a GWIN window
 * @{
 */
typedef struct gwinVMT {
	const char *		classname;						// @< The GWIN classname (mandatory)
	size_t				size;							// @< The size of the class object
	void (*Destroy)		(GWindowObject *gh);			// @< The GWIN destroy function (optional)
	void (*Redraw)		(GWindowObject *gh);			// @< The GWIN redraw routine (optional)
	void (*AfterClear)	(GWindowObject *gh);			// @< The GWIN after-clear function (optional)
} gwinVMT;
/** @} */

#if GWIN_NEED_WIDGET || defined(__DOXYGEN__)

	/**
	 * @brief	An toggle/dial instance is not being used
	 */
	#define GWIDGET_NO_INSTANCE		((uint16_t)-1)

	/**
	 * @brief	The source handle that widgets use when sending events
	 */
	#define GWIDGET_SOURCE			((GSourceHandle)(void *)_gwidgetCreate)

	/**
	 * @brief	The Virtual Method Table for a widget
	 * @note	A widget must have a destroy function. Either use @p _gwidgetDestroy() or use your own function
	 * 			which internally calls @p _gwidgetDestroy().
	 * @note	A widget must have a redraw function. Use @p _gwidgetRedraw().
	 * @note	If toggleroles != 0, ToggleAssign(), ToggleGet() and one or both of ToggleOff() and ToggleOn() must be specified.
	 * @note	If dialroles != 0, DialAssign(), DialGet() and DialMove() must be specified.
	 * @{
	 */
	typedef struct gwidgetVMT {
		struct gwinVMT				g;														// @< This is still a GWIN
		void (*DefaultDraw)			(GWidgetObject *gw, void *param);						// @< The default drawing routine (mandatory)
		#if GINPUT_NEED_MOUSE
			struct {
				void (*MouseDown)		(GWidgetObject *gw, coord_t x, coord_t y);				// @< Process mouse down events (optional)
				void (*MouseUp)			(GWidgetObject *gw, coord_t x, coord_t y);				// @< Process mouse up events (optional)
				void (*MouseMove)		(GWidgetObject *gw, coord_t x, coord_t y);				// @< Process mouse move events (optional)
			};
		#endif
		#if GINPUT_NEED_TOGGLE
			struct {
				uint16_t				toggleroles;											// @< The roles supported for toggles (0->toggleroles-1)
				void (*ToggleAssign)	(GWidgetObject *gw, uint16_t role, uint16_t instance);	// @< Assign a toggle to a role (optional)
				uint16_t (*ToggleGet)	(GWidgetObject *gw, uint16_t role);						// @< Return the instance for a particular role (optional)
				void (*ToggleOff)		(GWidgetObject *gw, uint16_t role);						// @< Process toggle off events (optional)
				void (*ToggleOn)		(GWidgetObject *gw, uint16_t role);						// @< Process toggle on events (optional)
			};
		#endif
		#if GINPUT_NEED_DIAL
			struct {
				uint16_t				dialroles;												// @< The roles supported for dials (0->dialroles-1)
				void (*DialAssign)		(GWidgetObject *gw, uint16_t role, uint16_t instance);	// @< Test the role and save the dial instance handle (optional)
				uint16_t (*DialGet)		(GWidgetObject *gw, uint16_t role);						// @< Return the instance for a particular role (optional)
				void (*DialMove)		(GWidgetObject *gw, uint16_t role, uint16_t value, uint16_t max);	// @< Process dial move events (optional)
			};
		#endif
	} gwidgetVMT;
	/** @} */
#endif

#if GWIN_NEED_CONTAINERS || defined(__DOXYGEN__)

	/**
	 * @brief	The Virtual Method Table for a container
	 * @note	A container must have a destroy function. Either use @p _gcontainerDestroy() or use your own function
	 * 			which internally calls @p _gcontainerDestroy().
	 * @note	A container must have a gwin redraw function. Use @p _containerRedraw().
	 * @note	If toggleroles != 0, ToggleAssign(), ToggleGet() and one or both of ToggleOff() and ToggleOn() must be specified.
	 * @note	If dialroles != 0, DialAssign(), DialGet() and DialMove() must be specified.
	 * @{
	 */
	typedef struct gcontainerVMT {
		gwidgetVMT	gw;
		coord_t (*LeftBorder)		(GHandle gh);							// @< The size of the left border (mandatory)
		coord_t (*TopBorder)		(GHandle gh);							// @< The size of the top border (mandatory)
		coord_t (*RightBorder)		(GHandle gh);							// @< The size of the right border (mandatory)
		coord_t (*BottomBorder)		(GHandle gh);							// @< The size of the bottom border (mandatory)
		void (*NotifyAdd)			(GHandle gh, GHandle ghChild);			// @< Notification that a child has been added (optional)
		void (*NotifyDelete)		(GHandle gh, GHandle ghChild);			// @< Notification that a child has been deleted (optional)
	} gcontainerVMT;
	/** @} */
#endif

#if GWIN_NEED_WINDOWMANAGER || defined(__DOXYGEN__)
	// @note	There is only ever one instance of each GWindowManager type
	typedef struct GWindowManager {
		const struct gwmVMT	*vmt;
	} GWindowManager;

	/**
	 * @brief	The Virtual Method Table for a window manager
	 * @{
	 */
	typedef struct gwmVMT {
		void (*Init)		(void);									// @< The window manager has just been set as the current window manager
		void (*DeInit)		(void);									// @< The window manager has just been removed as the current window manager
		bool_t (*Add)		(GHandle gh, const GWindowInit *pInit);	// @< A window has been added
		void (*Delete)		(GHandle gh);							// @< A window has been deleted
		void (*Redraw)		(GHandle gh);							// @< A window needs to be redraw (or undrawn)
		void (*Size)		(GHandle gh, coord_t w, coord_t h);		// @< A window wants to be resized
		void (*Move)		(GHandle gh, coord_t x, coord_t y);		// @< A window wants to be moved
		void (*Raise)		(GHandle gh);							// @< A window wants to be on top
		void (*MinMax)		(GHandle gh, GWindowMinMax minmax);		// @< A window wants to be minimized/maximised
	} gwmVMT;
	/** @} */

	/**
	 * @brief	The current window manager
	 */
	extern GWindowManager * _GWINwm;
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief	Initialise (and allocate if necessary) the base GWIN object
 *
 * @param[in]	g		The GDisplay to use for this window
 * @param[in]	pgw		The GWindowObject structure. If NULL one is allocated from the heap
 * @param[in]	pInit	The user initialization parameters
 * @param[in]	vmt		The virtual method table for the GWIN object
 * @param[in]	flags	The default flags to use
 *
 * @return	The GHandle of the created window
 *
 * @notapi
 */
GHandle _gwindowCreate(GDisplay *g, GWindowObject *pgw, const GWindowInit *pInit, const gwinVMT *vmt, uint32_t flags);

/**
 * @brief	Redraw the window after a status change.
 *
 * @param[in]	gh		The widget to redraw
 *
 * @note	Mark a window for redraw.
 * @note	The window will get redrawn at some later time.
 * @note	This call is designed to be fast and non-blocking
 *
 * @notapi
 */
void _gwinUpdate(GHandle gh);

/**
 * @brief	How to flush the redraws
 * @notes	REDRAW_WAIT			- Wait for a drawing session to be available
 * @notes	REDRAW_NOWAIT		- Do nothing if the drawing session is not available
 * @note	REDRAW_INSESSION	- We are already in a drawing session
 */
typedef enum GRedrawMethod { REDRAW_WAIT, REDRAW_NOWAIT, REDRAW_INSESSION }	GRedrawMethod;

/**
 * @brief	Flush any pending redraws in the system.
 *
 * @param[in]	doWait		Do we wait for the lock?
 *
 * @note	This call will attempt to flush any pending redraws
 * 			in the system. The doWait parameter tells this call
 * 			how to handle someone already holding the drawing lock.
 * 			If doWait is TRUE it waits to obtain the lock. If FALSE
 * 			and the drawing lock is free then the redraw is done
 * 			immediately. If the drawing lock was taken it will postpone the flush
 * 			on the basis that someone else will do it for us later.
 *
 * @notapi
 */
void _gwinFlushRedraws(GRedrawMethod how);

/**
 * @brief	Obtain a drawing session
 * @return	TRUE if the drawing session was obtained, FALSE if the window is not visible
 *
 * @param[in]	gh		The window
 *
 * @note	This function blocks until a drawing session is available if the window is visible
 */
bool_t _gwinDrawStart(GHandle gh);

/**
 * @brief	Release a drawing session
 *
 * @param[in]	gh		The window
 */
void _gwinDrawEnd(GHandle gh);

/**
 * @brief	Add a window to the window manager and set its position and size
 * @return	TRUE if successful
 *
 * @param[in]	gh		The window
 * @param[in]	pInit	The window init structure
 */
bool_t _gwinWMAdd(GHandle gh, const GWindowInit *pInit);

#if GWIN_NEED_WIDGET || defined(__DOXYGEN__)
	/**
	 * @brief	Initialise (and allocate if necessary) the base Widget object
	 *
	 * @param[in]	g		The GDisplay to display this window on
	 * @param[in]	pgw		The GWidgetObject structure. If NULL one is allocated from the heap
	 * @param[in]	pInit	The user initialization parameters
	 * @param[in]	vmt		The virtual method table for the Widget object
	 *
	 * @return	The GHandle of the created widget
 	 *
	 * @notapi
	 */
	GHandle _gwidgetCreate(GDisplay *g, GWidgetObject *pgw, const GWidgetInit *pInit, const gwidgetVMT *vmt);

	/**
	 * @brief	Destroy the Widget object
	 *
	 * @param[in]	gh		The widget to destroy
	 *
	 * @notapi
	 */
	void _gwidgetDestroy(GHandle gh);

	/**
	 * @brief	Redraw the Widget object (VMT method only)
	 *
	 * @param[in]	gh		The widget to redraw
	 *
	 * @note	Do not use this routine to update a widget after a status change.
	 * 			Use @p _gwinUpdate() instead. This routine should only be used in the
	 * 			VMT.
	 *
	 * @notapi
	 */
	void _gwidgetRedraw(GHandle gh);
#endif

#if GWIN_NEED_CONTAINERS || defined(__DOXYGEN__)
	/**
	 * @brief	Initialise (and allocate if necessary) the base Container object
	 *
	 * @param[in]	g		The GDisplay to display this window on
	 * @param[in]	pgw		The GContainerObject structure. If NULL one is allocated from the heap
	 * @param[in]	pInit	The user initialization parameters
	 * @param[in]	vmt		The virtual method table for the Container object
	 *
	 * @return	The GHandle of the created widget
 	 *
	 * @notapi
	 */
	GHandle _gcontainerCreate(GDisplay *g, GContainerObject *pgw, const GWidgetInit *pInit, const gcontainerVMT *vmt);

	/**
	 * @brief	Destroy the Container object
	 *
	 * @param[in]	gh		The container to destroy
	 *
	 * @notapi
	 */
	void _gcontainerDestroy(GHandle gh);

	/**
	 * @brief	Redraw the Container object (VMT method only)
	 *
	 * @param[in]	gh		The container to redraw
	 *
	 * @note	Do not use this routine to update a container after a status change.
	 * 			Use @p _gwinUpdate() instead. This routine should only be used in the
	 * 			VMT.
	 *
	 * @notapi
	 */
	#define _gcontainerRedraw		_gwidgetRedraw
#endif

#ifdef __cplusplus
}
#endif

#endif /* GFX_USE_GWIN */

#endif /* _CLASS_GWIN_H */
/** @} */
