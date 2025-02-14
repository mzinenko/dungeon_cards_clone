/*
  Simple DirectMedia Layer
<<<<<<<< HEAD:resource/framework/SDL2.framework/Versions/A/Headers/close_code.h
  Copyright (C) 1997-2019 Sam Lantinga <slouken@libsdl.org>
========
  Copyright (C) 1997-2024 Sam Lantinga <slouken@libsdl.org>
>>>>>>>> d7aea5bdb5e8756226dc5230a6a2ba0e01149398:resource/framework/temp/SDL2-2.30.0/src/video/x11/SDL_x11xfixes.h

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

#include "../../SDL_internal.h"

<<<<<<<< HEAD:resource/framework/SDL2.framework/Versions/A/Headers/close_code.h
#ifndef _begin_code_h
#error close_code.h included without matching begin_code.h
#endif
#undef _begin_code_h
========
#ifndef SDL_x11xfixes_h_
#define SDL_x11xfixes_h_
>>>>>>>> d7aea5bdb5e8756226dc5230a6a2ba0e01149398:resource/framework/temp/SDL2-2.30.0/src/video/x11/SDL_x11xfixes.h

#ifdef SDL_VIDEO_DRIVER_X11_XFIXES

#define X11_BARRIER_HANDLED_BY_EVENT 1

extern void X11_InitXfixes(_THIS);
extern int X11_XfixesIsInitialized(void);
extern void X11_SetWindowMouseRect(_THIS, SDL_Window *window);
extern int X11_ConfineCursorWithFlags(_THIS, SDL_Window *window, const SDL_Rect *rect, int flags);
extern void X11_DestroyPointerBarrier(_THIS, SDL_Window *window);
extern int X11_GetXFixesSelectionNotifyEvent(void);
#endif /* SDL_VIDEO_DRIVER_X11_XFIXES */

#endif /* SDL_x11xfixes_h_ */

/* vi: set ts=4 sw=4 expandtab: */
