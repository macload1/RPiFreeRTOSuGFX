/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    src/gwin/gwidget.h
 * @brief   GWIN Widgets header file.
 *
 * @defgroup Widget Widget
 * @ingroup Widgets
 *
 * @details		A widget is a Window that supports interacting with the user
 * 				via an input device such as a mouse or toggle buttons. It is the
 * 				base class for widgets such as buttons and sliders.
 *
 * @pre		GFX_USE_GWIN and GWIN_NEED_WIDGET must be set to TRUE in your gfxconf.h
 * @{
 */

#ifndef _GWIDGET_H
#define _GWIDGET_H

/* This file is included within "gwin/gwin.h" */

// Forward definition
struct GWidgetObject;

/**
 * @brief	The GColorSet structure
 * @{
 */
typedef struct GColorSet {
	color_t			text;				// @< The text color
	color_t			edge;				// @< The edge color
	color_t			fill;				// @< The fill color
	color_t			progress;			// @< The color of progress bars
} GColorSet;
/** @} */

/**
 * @brief		The GWidgetStyle structure
 * @details		A GWidgetStyle is a set of colors that together form a "style".
 * 				These colors should not be confused with the GWindow foreground
 * 				and background colors which are used for drawing operations.
 * @{
 */
typedef struct GWidgetStyle {
	color_t			background;			// @< The window background color
	GColorSet		enabled;			// @< The colors when enabled
	GColorSet		disabled;			// @< The colors when disabled
	GColorSet		pressed;			// @< The colors when pressed
} GWidgetStyle;
/** @} */

/**
 * @brief	We define a couple of GWidgetStyle's that you can use in your
 * 			application. The Black style is the default style if you don't
 * 			specify one.
 * @note	BlackWidgetStyle means that it is designed for a Black background.
 * 			Similarly WhiteWidgetStyle is designed for a White background.
 * @{
 */
extern const GWidgetStyle BlackWidgetStyle;
extern const GWidgetStyle WhiteWidgetStyle;
/** @} */

/**
 * @brief	Defines a custom drawing function for a widget
 */
typedef void (*CustomWidgetDrawFunction)(struct GWidgetObject *gw, void *param);

/**
 * @brief	The structure to initialise a widget.
 *
 * @note	Some widgets may have extra parameters.
 * @note	If you create this structure on the stack, you should always memset
 * 			it to all zero's first in case a future version of the software
 * 			add's extra fields. Alternatively you can use @p gwinWidgetClearInit()
 * 			to clear it.
 * @note	The text element must be static string (not stack allocated). If you want to use
 * 			a dynamic string (eg a stack allocated string) use NULL for this member and then call
 * 			@p gwinSetText() with useAlloc set to TRUE.
 *
 * @{
 */
typedef struct GWidgetInit {
	GWindowInit					g;						// @< The GWIN initializer
	const char *				text;					// @< The initial text
	CustomWidgetDrawFunction	customDraw;				// @< A custom draw function - use NULL for the standard
	void *						customParam;			// @< A parameter for the custom draw function (default = NULL)
	const GWidgetStyle *		customStyle;			// @< A custom style to use - use NULL for the default style
} GWidgetInit;
/** @} */

/**
 * @brief	The GWIN Widget structure
 * @note	A widget is a GWIN window that accepts user input.
 * 			It also has a number of other properties such as its ability
 * 			to redraw itself (a widget maintains drawing state).
 * @note	Do not access the members directly. Treat it as a black-box and use the method functions.
 *
 * @{
 */
typedef struct GWidgetObject {
	GWindowObject				g;					// @< This is still a GWIN
	const char *				text;				// @< The widget text
	CustomWidgetDrawFunction	fnDraw;				// @< The current draw function
	void *						fnParam;			// @< A parameter for the current draw function
	const GWidgetStyle *		pstyle;				// @< The current widget style colors
} GWidgetObject;
/** @} */

/**
 * A comment/rant on the above structure:
 * We would really like the GWindowObject member to be anonymous. While this is
 * allowed under the C11, C99, GNU and various other standards which have been
 * around forever - compiler support often requires special flags e.g
 * gcc requires the -fms-extensions flag (no wonder the language and compilers have
 * not really progressed in 30 years). As portability is a key requirement
 * we unfortunately won't use this useful feature in case we get a compiler that
 * won't support it even with special flags.
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief	Clear a GWidgetInit structure to all zero's
 * @note	This function is provided just to prevent problems
 * 			on operating systems where using memset() causes issues
 * 			in the users application.
 *
 * @param[in] pwi	The GWidgetInit structure to clear
 *
 * @api
 */
void gwinWidgetClearInit(GWidgetInit *pwi);

/**
 * @brief   Set the default style for widgets created hereafter.
 *
 * @param[in] pstyle	The default style. Passing NULL uses the system compiled style.
 * @param[in] updateAll	If TRUE then all existing widgets that are using the current default style
 * 						will be updated to use this new style. Widgets that have custom styles different
 * 						from the default style will not be updated.
 *
 * @note	The style must be allocated statically (not on the stack) as only the pointer is stored.
 *
 * @api
 */
void gwinSetDefaultStyle(const GWidgetStyle *pstyle, bool_t updateAll);

/**
 * @brief   Get the current default style.
 *
 * @return	The current default style.
 * 
 * @api
 */
const GWidgetStyle *gwinGetDefaultStyle(void);

/**
 * @brief   Set the text of a widget.
 *
 * @param[in] gh		The widget handle
 * @param[in] text		The text to set. This must be a constant string unless useAlloc is set.
 * @param[in] useAlloc	If TRUE the string specified will be copied into dynamically allocated memory.
 *
 * @note				The widget is automatically redrawn
 * @note				Non-widgets will ignore this call.
 *
 * @api
 */
void gwinSetText(GHandle gh, const char *text, bool_t useAlloc);

/**
 * @brief   Get the text of a widget.
 * @return	The widget text or NULL if it isn't a widget
 *
 * @param[in] gh		The widget handle
 *
 * @api
 */
const char *gwinGetText(GHandle gh);

/**
 * @brief   Set the style of a widget.
 *
 * @param[in] gh		The widget handle
 * @param[in] pstyle	The style to set. This must be a static structure (not allocated on a transient stack).
 * 						Use NULL to reset to the default style.
 *
 * @note				The widget is automatically redrawn
 * @note				Non-widgets will ignore this call.
 *
 * @api
 */
void gwinSetStyle(GHandle gh, const GWidgetStyle *pstyle);

/**
 * @brief   Get the style of a widget.
 * @return	The widget style or NULL if it isn't a widget
 *
 * @param[in] gh		The widget handle
 *
 * @api
 */
const GWidgetStyle *gwinGetStyle(GHandle gh);

/**
 * @brief   Set the routine to perform a custom widget drawing.
 *
 * @param[in] gh		The widget handle
 * @param[in] fn		The function to use to draw the widget
 * @param[in] param		A parameter to pass to the widget drawing function
 *
 * @note				The widget is not automatically redrawn. Call @p gwinDraw() to redraw the widget.
 * @note				Non-widgets will ignore this call.
 *
 * @api
 */
void gwinSetCustomDraw(GHandle gh, CustomWidgetDrawFunction fn, void *param);

/**
 * @brief	Attach a Listener to listen for widget events
 * @return	TRUE on success
 *
 * @param[in] pl		The listener
 *
 * @api
 */
bool_t gwinAttachListener(GListener *pl);

#if GFX_USE_GINPUT && GINPUT_NEED_MOUSE
	/**
	 * @brief	Set the mouse to be used to control the widgets
	 * @return	TRUE on success
	 *
	 * @param[in] instance	The mouse instance
	 *
	 * @note	Every widget uses the same mouse.
	 *
	 * @api
	 */
	bool_t gwinAttachMouse(uint16_t instance);
#endif

#if GFX_USE_GINPUT && GINPUT_NEED_TOGGLE
	/**
	 * @brief	Attach a toggle to a widget
	 * @return	TRUE on success
	 *
	 * @param[in] gh		The widget handle
	 * @param[in] role		The function the toggle will perform for the widget
	 * @param[in] instance	The toggle instance
	 *
	 * @note		See the documentation on the specific widget to see the possible
	 * 				values for the role parameter. If it is out of range, this function
	 * 				will return FALSE
	 *
	 * @api
	 */
	bool_t gwinAttachToggle(GHandle gh, uint16_t role, uint16_t instance);
#endif

#if GFX_USE_GINPUT && GINPUT_NEED_DIAL
	/**
	 * @brief	Attach a toggle to a widget
	 * @return	TRUE on success
	 *
	 * @param[in] gh		The widget handle
	 * @param[in] role		The function the dial will perform for the widget
	 * @param[in] instance	The dial instance
	 *
	 * @note		See the documentation on the specific widget to see the possible
	 * 				values for the role parameter. If it is out of range, this function
	 * 				will return FALSE
	 *
	 * @api
	 */
	bool_t gwinAttachDial(GHandle gh, uint16_t role, uint16_t instance);
#endif

#ifdef __cplusplus
}
#endif

/* Include extra widget types */
#if GWIN_NEED_BUTTON || defined(__DOXYGEN__)
	#include "src/gwin/button.h"
#endif

#if GWIN_NEED_SLIDER || defined(__DOXYGEN__)
	#include "src/gwin/slider.h"
#endif

#if GWIN_NEED_CHECKBOX || defined(__DOXYGEN__)
	#include "src/gwin/checkbox.h"
#endif

#if GWIN_NEED_RADIO || defined(__DOXYGEN__)
	#include "src/gwin/radio.h"
#endif

#if GWIN_NEED_LABEL || defined(__DOXYGEN__)
	#include "src/gwin/label.h"
#endif

#if GWIN_NEED_LIST || defined(__DOXYGEN__)
	#include "src/gwin/list.h"
#endif

#if GWIN_NEED_PROGRESSBAR || defined(__DOXYGEN__)
	#include "src/gwin/progressbar.h"
#endif

#endif /* _GWIDGET_H */
/** @} */
