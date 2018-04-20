/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    src/gwin/sys_options.h
 * @brief   GWIN sub-system options header file.
 *
 * @addtogroup GWIN
 * @brief	The GWIN module uses all the other modules (GDISP, GINPUT, GTIMER...) to
 *			form a complete GUI toolkit.
 * 
 * @{
 */

#ifndef _GWIN_OPTIONS_H
#define _GWIN_OPTIONS_H

/**
 * @name    GWIN Functionality to be included
 * @{
 */
	/**
	 * @brief   Should window manager support be included
	 * @details	Defaults to FALSE
	 */
	#ifndef GWIN_NEED_WINDOWMANAGER
		#define GWIN_NEED_WIDGET	FALSE
	#endif
	/**
	 * @brief	Should the widget hierarchy be included. This provides parent-child features.
	 * @details	Defaults to FALSE
	 */
	#ifndef GWIN_NEED_CONTAINERS
		#define GWIN_NEED_CONTAINERS	FALSE
	#endif
	/**
	 * @brief   Should widget functions be included. Needed for any widget (eg Buttons, Sliders etc)
	 * @details	Defaults to FALSE
	 */
	#ifndef GWIN_NEED_WIDGET
		#define GWIN_NEED_WIDGET	FALSE
	#endif
	/**
	 * @brief	Should the simple container be included.
	 * @details	Defaults to FALSE
	 */
	#ifndef GWIN_NEED_CONTAINER
		#define GWIN_NEED_CONTAINER		FALSE
	#endif
	/**
	 * @brief	Should the frame widget be included.
	 * @details	Defaults to FALSE
	 */
	#ifndef GWIN_NEED_FRAME
		#define GWIN_NEED_FRAME		FALSE
	#endif
	/**
	 * @brief   Should console functions be included.
	 * @details	Defaults to FALSE
	 */
	#ifndef GWIN_NEED_CONSOLE
		#define GWIN_NEED_CONSOLE	FALSE
	#endif
	/**
	 * @brief   Should graph functions be included.
	 * @details	Defaults to FALSE
	 */
	#ifndef GWIN_NEED_GRAPH
		#define GWIN_NEED_GRAPH		FALSE
	#endif
	/**
	 * @brief   Should button functions be included.
	 * @details	Defaults to FALSE
	 */
	#ifndef GWIN_NEED_BUTTON
		#define GWIN_NEED_BUTTON	FALSE
	#endif
	/**
	 * @brief   Should progressbar functions be included.
	 * @details	Defaults to FALSE
	 */
	#ifndef GWIN_NEED_PROGRESSBAR
		#define GWIN_NEED_PROGRESSBAR	FALSE
	#endif
	/**
	 * @brief   Should slider functions be included.
	 * @details	Defaults to FALSE
	 */
	#ifndef GWIN_NEED_SLIDER
		#define GWIN_NEED_SLIDER	FALSE
	#endif
	/**
	 * @brief   Should checkbox functions be included.
	 * @details	Defaults to FALSE
	 */
	#ifndef GWIN_NEED_CHECKBOX
		#define GWIN_NEED_CHECKBOX	FALSE
	#endif
	/**
	 * @brief   Should image functions be included.
	 * @details	Defaults to FALSE
	 */
	#ifndef GWIN_NEED_IMAGE
		#define GWIN_NEED_IMAGE		FALSE
	#endif
	/**
	 * @brief   Should label functions be included.
	 * @details	Defaults to FALSE
	 */
	#ifndef GWIN_NEED_LABEL
		#define GWIN_NEED_LABEL		FALSE
	#endif
	/**
	 * @brief   Should radio button functions be included.
	 * @details	Defaults to FALSE
	 */
	#ifndef GWIN_NEED_RADIO
		#define GWIN_NEED_RADIO		FALSE
	#endif
/**
 * @}
 *
 * @name    GWIN Optional Parameters
 * @{
 */
	/**
	 * @brief   Use flat styling for controls rather than a 3D look
	 * @details	Defaults to FALSE
	 * @note	This may appear better on color-restricted displays
	 * @note	Flat styling is less graphics and cpu intensive (marginally) than the default 3D look.
	 */
	#ifndef GWIN_FLAT_STYLING
		#define GWIN_FLAT_STYLING		FALSE
	#endif
	/**
	 * @brief	Don't use a timer for redrawing windows
	 * @details	Defaults to FALSE
	 * @note	Normally windows and widgets are redrawn on a timer. Setting this
	 * 			option causes them to be redrawn immediately. Note that this can
	 * 			cause extended blocking times on events and saves little code.
	 * @note	If GWIN_NEED_WINDOWMANAGER is FALSE then this setting is ignored
	 * 			as redrawing always occurs immediately.
	 */
	#ifndef GWIN_REDRAW_IMMEDIATE
		#define GWIN_REDRAW_IMMEDIATE	FALSE
	#endif
	/**
	 * @brief	Redraw all windows in a single operation
	 * @details	Defaults to FALSE
	 * @note	Windows are normally redraw one per gtimer cycle.
	 * 			Setting this option causes all windows to be redrawn in
	 * 			a single gtimer cycle. Note that this can
	 * 			cause extended blocking times on the timer thread but may
	 * 			speed up redraw slightly.
	 * @note	This is only relevant if GWIN_REDRAW_IMMEDIATE is FALSE.
	 * 			Everything always gets redrawn in a single operation if
	 * 			GWIN_REDRAW_IMMEDIATE is TRUE.
	 */
	#ifndef GWIN_REDRAW_SINGLEOP
		#define GWIN_REDRAW_SINGLEOP	FALSE
	#endif
	/**
	 * @brief   Buttons should not insist the mouse is over the button on mouse release
	 * @details	Defaults to FALSE
	 */
	#ifndef GWIN_BUTTON_LAZY_RELEASE
		#define GWIN_BUTTON_LAZY_RELEASE		FALSE
	#endif
	/**
	 * @brief	Should the content of the console be saved for redrawing.
	 * @details	Defaults to FALSE
	 * @details	If this feature is enabled, the contents of the console will be saved
	 * 			as it is written. If a redraw is required it will be redrawn from the
	 * 			history. Scrolling will also use the history buffer if it is turned on.
	 * @note	Using this option allocates the amount of memory to store the
	 * 			history based on the minimum character width in the current font
	 * 			at the time the history is turned on. Using a fixed width font is a good
	 * 			idea to minimize memory usage.
	 * @note	If you change the size of the window or you change the font being displayed
	 * 			you should turn off the history and then turn it back on in order to get
	 * 			a new buffer of the correct size for the window/font combination. Strange
	 * 			redrawing and scrolling effects can occur if the buffer is too small to
	 * 			save a complete screen of data. Note the system tries to optimize storage
	 * 			so this may only be evident in very limited situations eg with a console
	 * 			with many characters in it.
	 * @note	@p gwinConsoleSetBuffer() can be used to turn the history buffer off and on.
	 */
	#ifndef GWIN_CONSOLE_USE_HISTORY
		#define GWIN_CONSOLE_USE_HISTORY		FALSE
	#endif
	/**
	 * @brief	Use font width averaging for the history buffer allocation.
	 * @details	Defaults to FALSE
	 * @details	If this feature is enabled, the width one third of the way between
	 * 			the font's character width minimum and maximum will be used instead
	 * 			of the font's minimum width.
	 * @note	This option reduces the memory allocation for a variable width font's
	 * 			history buffer. Note that strange
	 * 			redrawing and scrolling effects can occur if the buffer is too small to
	 * 			save a complete screen of data. The system tries to optimize storage
	 * 			so this may only be evident in very limited situations eg with a console
	 * 			with many characters in it.
	 */
	#ifndef GWIN_CONSOLE_HISTORY_AVERAGING
		#define GWIN_CONSOLE_HISTORY_AVERAGING	FALSE
	#endif
	/**
	 * @brief	Should the history be turned on for all console windows when they are first created.
	 * @details	Defaults to FALSE
	 * @note	@p gwinConsoleSetBuffer() can be used to turn the history buffer off and on at
	 * 			any time.
	 */
	#ifndef GWIN_CONSOLE_HISTORY_ATCREATE
		#define GWIN_CONSOLE_HISTORY_ATCREATE	FALSE
	#endif
	/**
	 * @brief   Console Windows need floating point support in @p gwinPrintf
	 * @details	Defaults to FALSE
	 */
	#ifndef GWIN_CONSOLE_USE_FLOAT
		#define GWIN_CONSOLE_USE_FLOAT			FALSE
	#endif
	/**
	 * @brief   Console windows support escape sequences to control display
	 * @details	Defaults to FALSE
	 *
	 * @note
	 * 		Currently supported:
	 * 			ESC color		Change subsequent text color
	 * 							color:	"0" = black, "1" = red, "2" = green, "3" = yellow, "4" = blue,
	 * 									"5" = magenta, "6" = cyan, "7" = white
	 * 			ESC C			Revert subsequent text color to the window default
	 * 			ESC u			Turn on underline
	 * 			ESC U			Turn off underline
	 * 			ESC b			Turn on bold
	 * 			ESC B			Turn off bold
	 * 			ESC J			Clear the window
	 */
	#ifndef GWIN_CONSOLE_ESCSEQ
		#define GWIN_CONSOLE_ESCSEQ				FALSE
	#endif
	/**
	 * @brief   Console Windows need BaseStreamSequential support (ChibiOS only)
	 * @details	Defaults to FALSE
	 * @note	To use the ChibiOS basestream functions such as chprintf()
	 * 			for printing in a console window you need to set this option to
	 * 			TRUE in your gfxconf.h and include in your application source file...
	 * 			\#include "chprintf.h"
	 * 			In your makefile, as part of your list of C source files, include
	 * 			${CHIBIOS}/os/various/chprintf.c
	 */
	#ifndef GWIN_CONSOLE_USE_BASESTREAM
		#define GWIN_CONSOLE_USE_BASESTREAM		FALSE
	#endif
	/**
	 * @brief   Image windows can optionally support animated images
	 * @details	Defaults to FALSE
	 */
	#ifndef GWIN_NEED_IMAGE_ANIMATION
		#define GWIN_NEED_IMAGE_ANIMATION		FALSE
	#endif
	/**
	 * @brief	Enable the API to automatically increment the progressbar over time
	 * @details	Defaults to FALSE
	 */
	#ifndef GWIN_PROGRESSBAR_AUTO
	 	#define GWIN_PROGRESSBAR_AUTO			FALSE
	#endif
/** @} */

#endif /* _GWIN_OPTIONS_H */
/** @} */
