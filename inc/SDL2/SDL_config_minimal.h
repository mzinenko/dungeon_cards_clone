#ifndef SDL_config_minimal_h_
#define SDL_config_minimal_h_

#include "SDL_platform.h"

/* This is a minimal configuration for the SDL library */

#define HAVE_STDARG_H 1
#define HAVE_STDDEF_H 1
#define HAVE_STDINT_H 1

/* Most everything except Visual Studio 2003 and earlier has stdint.h now */
#if defined(_MSC_VER) && (_MSC_VER < 1600)
typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef signed short int16_t;
typedef unsigned short uint16_t;
typedef signed int int32_t;
typedef unsigned int uint32_t;
typedef signed long long int64_t;
typedef unsigned long long uint64_t;
typedef unsigned long uintptr_t;
#else
#include <stdint.h>
#endif /* Visual Studio 2003 */

#ifdef __MACOSX__
#define HAVE_GCC_ATOMICS 1
#endif

#define HAVE_CTYPE_H 1
#define HAVE_MATH_H 1
#define HAVE_SIGNAL_H 1
#define HAVE_STDIO_H 1
#define HAVE_STRING_H 1

#endif /* SDL_config_minimal_h_ */
