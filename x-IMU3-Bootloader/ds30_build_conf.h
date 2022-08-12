//-----------------------------------------------------------------------------
// Copyright 2011-2021 DigSol Sweden AB
//-----------------------------------------------------------------------------
//  This code may only be used together with ds30 Loader boot loader firmwares
//  purchased from DigSol Sweden AB.
//-----------------------------------------------------------------------------
#ifndef DS30_BUILD_CONF_H
#define DS30_BUILD_CONF_H


//-----------------------------------------------------------------------------
// Determine build environment
//-----------------------------------------------------------------------------
#if defined(_WIN32)
	#define BUILD_WIN32

#elif defined(__linux)
	#define BUILD_LINUX

#elif defined(__APPLE__)
    #include "TargetConditionals.h"

    #if TARGET_IPHONE_SIMULATOR
         // iOS Simulator

    #elif TARGET_OS_IPHONE
        // iOS device

    #elif TARGET_OS_MAC
        #define BUILD_OS_X

    #else
        #error "Unsupported platform"
    #endif

#else
	#error "Unknown build environment"
#endif


//-----------------------------------------------------------------------------
// Platform defines
//-----------------------------------------------------------------------------
#if defined(BUILD_WIN32)
	#pragma warning(disable:4996)   //secure functions are not available with gcc
	#define NEWLINE	                "\r\n"

#else
    #define NEWLINE	                "\n"
#endif


//-----------------------------------------------------------------------------
// End of file
//-----------------------------------------------------------------------------
#endif//DS30_BUILD_CONF_H
