/**
 * This file has a different license to the rest of the GFX system.
 * You can copy, modify and distribute this file as you see fit.
 * You do not need to publish your source modifications to this file.
 * The only thing you are not permitted to do is to relicense it
 * under a different license.
 */

#ifndef _LOCALGFXCONF_H
#define _LOCALGFXCONF_H

/* Fix possible ifdefs! */
#ifndef TRUE
#define TRUE	1
#endif

/* The operating system to use - one of these must be defined */
#define GFX_USE_OS_CHIBIOS		2
#define GFX_USE_OS_WIN32		FALSE
#define GFX_USE_OS_LINUX		FALSE
#define GFX_USE_OS_OSX			FALSE

/* GFX subsystems to turn on */
#define GFX_USE_GDISP			FALSE
#define GFX_USE_TDISP			TRUE
#define GFX_USE_GWIN			FALSE
#define GFX_USE_GEVENT			FALSE
#define GFX_USE_GTIMER			FALSE
#define GFX_USE_GQUEUE			FALSE
#define GFX_USE_GINPUT			FALSE
#define GFX_USE_GADC			FALSE
#define GFX_USE_GAUDIN			FALSE
#define GFX_USE_GAUDOUT			FALSE
#define GFX_USE_GMISC			FALSE

#endif /* End of _GFXCONF_H */
