/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    gfx.h
 * @brief   GFX system header file.
 *
 * @addtogroup GFX
 *
 * @brief	Main module to glue all the others together
 *
 * @{
 */

#ifndef _GFX_H
#define _GFX_H

/**
 * These two definitions below are required before anything else so that we can
 * turn module definitions off and on.
 */

/**
 * @brief   Generic 'false' boolean constant.
 */
#if !defined(FALSE) || defined(__DOXYGEN__)
	#define FALSE       0
#endif

/**
 * @brief   Generic 'true' boolean constant.
 */
#if !defined(TRUE) || defined(__DOXYGEN__)
	#define TRUE        -1
#endif

/**
 * @brief   Mark a function as deprecated.
 */
#ifndef DEPRECATED
	#if defined(__GNUC__) || defined(__MINGW32_) || defined(__CYGWIN__)
		#define DEPRECATED(msg)		__attribute__((deprecated(msg)))
	#elif defined(_MSC_VER)
		#define DEPRECATED(msg)		__declspec(deprecated(msg))
	#else
		#define DEPRECATED(msg)
	#endif
#endif

/* gfxconf.h is the user's project configuration for the GFX system. */
#include "gfxconf.h"

/**
 * @name    GFX sub-systems that can be turned on
 * @{
 */
	/**
	 * @brief   GFX Graphics Display Basic API
	 * @details	Defaults to FALSE
	 * @note	Also add the specific hardware driver to your makefile.
	 * 			Eg.  include $(GFXLIB)/drivers/gdisp/Nokia6610/driver.mk
	 */
	#ifndef GFX_USE_GDISP
		#define GFX_USE_GDISP	FALSE
	#endif
	/**
	 * @brief   GFX Graphics Windowing API
	 * @details	Defaults to FALSE
	 * @details	Extends the GDISP API to add the concept of graphic windows.
	 * @note	Also supports high-level "window" objects such as console windows,
	 * 			buttons, graphing etc
	 */
	#ifndef GFX_USE_GWIN
		#define GFX_USE_GWIN	FALSE
	#endif
	/**
	 * @brief   GFX Event API
	 * @details	Defaults to FALSE
	 * @details	Defines the concept of a "Source" that can send "Events" to "Listeners".
	 */
	#ifndef GFX_USE_GEVENT
		#define GFX_USE_GEVENT	FALSE
	#endif
	/**
	 * @brief   GFX Timer API
	 * @details	Defaults to FALSE
	 * @details	Provides thread context timers - both one-shot and periodic.
	 */
	#ifndef GFX_USE_GTIMER
		#define GFX_USE_GTIMER	FALSE
	#endif
	/**
	 * @brief   GFX Queue API
	 * @details	Defaults to FALSE
	 * @details	Provides queue management.
	 */
	#ifndef GFX_USE_GQUEUE
		#define GFX_USE_GQUEUE	FALSE
	#endif
	/**
	 * @brief   GFX Input Device API
	 * @details	Defaults to FALSE
	 * @note	Also add the specific hardware drivers to your makefile.
	 * 			Eg.
	 * 				include $(GFXLIB)/drivers/ginput/toggle/Pal/driver.mk
	 * 			and...
	 * 				include $(GFXLIB)/drivers/ginput/touch/MCU/driver.mk
	 */
	#ifndef GFX_USE_GINPUT
		#define GFX_USE_GINPUT	FALSE
	#endif
	/**
	 * @brief   GFX Generic Periodic ADC API
	 * @details	Defaults to FALSE
	 */
	#ifndef GFX_USE_GADC
		#define GFX_USE_GADC	FALSE
	#endif
	/**
	 * @brief   GFX Audio API
	 * @details	Defaults to FALSE
	 * @note	Also add the specific hardware drivers to your makefile.
	 * 			Eg.
	 * 				include $(GFXLIB)/drivers/gaudio/GADC/driver.mk
	 */
	#ifndef GFX_USE_GAUDIO
		#define GFX_USE_GAUDIO	FALSE
	#endif
	/**
	 * @brief   GFX Miscellaneous Routines API
	 * @details	Defaults to FALSE
	 * @note	Turning this on without turning on any GMISC_NEED_xxx macros will result
	 * 			in no extra code being compiled in. GMISC is made up from the sum of its
	 * 			parts.
	 */
	#ifndef GFX_USE_GMISC
		#define GFX_USE_GMISC	FALSE
	#endif
	/**
	 * @brief   GFX File API
	 * @details	Defaults to FALSE
	 */
	#ifndef GFX_USE_GFILE
		#define GFX_USE_GFILE	FALSE
	#endif
/** @} */

/**
 * Get all the options for each sub-system.
 *
 */
#include "src/gos/sys_options.h"
#include "src/gfile/sys_options.h"
#include "src/gmisc/sys_options.h"
#include "src/gqueue/sys_options.h"
#include "src/gevent/sys_options.h"
#include "src/gtimer/sys_options.h"
#include "src/gdisp/sys_options.h"
#include "src/gwin/sys_options.h"
#include "src/ginput/sys_options.h"
#include "src/gadc/sys_options.h"
#include "src/gaudio/sys_options.h"

/**
 * Interdependency safety checks on the sub-systems.
 * These must be in dependency order.
 *
 */
#ifndef GFX_DISPLAY_RULE_WARNINGS
	#define GFX_DISPLAY_RULE_WARNINGS	FALSE
#endif
#include "src/gwin/sys_rules.h"
#include "src/ginput/sys_rules.h"
#include "src/gdisp/sys_rules.h"
#include "src/gaudio/sys_rules.h"
#include "src/gadc/sys_rules.h"
#include "src/gevent/sys_rules.h"
#include "src/gtimer/sys_rules.h"
#include "src/gqueue/sys_rules.h"
#include "src/gmisc/sys_rules.h"
#include "src/gfile/sys_rules.h"
#include "src/gos/sys_rules.h"

/**
 *  Include the sub-system header files
 */
#include "src/gos/sys_defs.h"
#include "src/gfile/sys_defs.h"
#include "src/gmisc/sys_defs.h"
#include "src/gqueue/sys_defs.h"
#include "src/gevent/sys_defs.h"
#include "src/gtimer/sys_defs.h"
#include "src/gdisp/sys_defs.h"
#include "src/gwin/sys_defs.h"
#include "src/ginput/sys_defs.h"
#include "src/gadc/sys_defs.h"
#include "src/gaudio/sys_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

	/**
	 * @brief	The one call to start it all
	 *
	 * @note	This will initialise each sub-system that has been turned on.
	 * 			For example, if GFX_USE_GDISP is defined then display will be initialised
	 * 			and cleared to black.
	 *
	 * @api
	 */
	void gfxInit(void);

	/**
	 * @brief	The one call to end it all
	 *
	 * @note	This will de-initialise each sub-system that has been turned on.
	 *
	 * @api
	 */
	void gfxDeinit(void);

#ifdef __cplusplus
}
#endif

#endif /* _GFX_H */
/** @} */

