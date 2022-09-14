//-----------------------------------------------------------------------------
// Copyright 2011-2022 DigSol Sweden AB
//------------------------------------------------------------------------------
//  This code may only be used together with ds30 Loader boot loader firmwares
//  purchased from DigSol Sweden AB.
//------------------------------------------------------------------------------
#ifndef DS30_BUILD_CONF_H
#define DS30_BUILD_CONF_H


//------------------------------------------------------------------------------
// Determine build environment
//------------------------------------------------------------------------------
#if defined(_WIN32)
	#define BUILD_WIN32

#elif defined(__linux)
	#define BUILD_LINUX

#elif defined(__APPLE__)
    #include "TargetConditionals.h"

    #if TARGET_OS_MAC
        #define BUILD_OS_X

    //#elif TARGET_IPHONE_SIMULATOR // iOS Simulator

    //#elif TARGET_OS_IPHONE        // iOS device

    #else
        #error "Unsupported platform"
    #endif

#else
	#error "Unknown build environment"
#endif


//------------------------------------------------------------------------------
// Platform defines
//------------------------------------------------------------------------------
#if defined(BUILD_WIN32)
    #pragma warning( disable : 4668 )   //is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'
    #pragma warning( disable : 4820 )   //'x' bytes padding added after data member y
	#pragma warning( disable : 4996 )   //secure functions are not available with gcc
    #pragma warning( disable : 5045 )   //compiler will insert Spectre mitigation for memory load if /Qspectre switch specified

	#define NEWLINE	                "\r\n"

#else
    #define NEWLINE	                "\n"
#endif


//------------------------------------------------------------------------------
// End of file
//------------------------------------------------------------------------------
#endif//DS30_BUILD_CONF_H
