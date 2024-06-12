//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-07-10.
//
// This file is distributed under the Simplified BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <cstdint>

/** @file
  * @brief Defines constants and macros shared by all the other files
  *     in @a Ystring.
  */

/// @cond

#ifdef _WIN32
    // Disabling warnings about exposing std::string, std::vector and
    // std::logic_error through the external DLL interface. The warnings are
    // valid, but it would defeat much of the purpose of Yglo to remove
    // their cause (i.e. stop using std::string etc.).
    // The issue is instead solved by having separate debug and release
    // versions of the DLL and LIB files.
    #pragma warning(disable: 4251 4275)
    #ifdef YGLOB_EXPORTS
        #define YGLOB_API __declspec(dllexport)
    #else
        #define YGLOB_API
    #endif
#else
    #define YGLOB_API
#endif

/// @endcond

/** @brief The top-level namespace for all functions and classes
  *     in @a Yglob.
  */
namespace Yglob
{}
