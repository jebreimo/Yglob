//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-06-12.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include <stdexcept>

/** @file
  * @brief Defines the exception thrown by Yglob functions.
  */

namespace YGlob
{
    /**
     * @brief The exception class used throughout Yglob.
     */
    class YglobException : public std::runtime_error
    {
    public:
        using std::runtime_error::runtime_error;
    };
}

#define YGLOB_IMPL_THROW_3(file, line, msg) \
    throw ::YGlob::YglobException(file ":" #line ": " msg)

#define YGLOB_IMPL_THROW_2(file, line, msg) \
    YGLOB_IMPL_THROW_3(file, line, msg)

#define YGLOB_THROW(msg) \
    YGLOB_IMPL_THROW_2(__FILE__, __LINE__, msg)
