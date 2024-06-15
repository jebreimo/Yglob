//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-03-25.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include "BitmaskOperators.hpp"

namespace Yglob
{
    enum class GlobFlags
    {
        NONE = 0,
        CASE_SENSITIVE = 1,
        USE_BRACES = 2,
        USE_SETS = 4,
        DEFAULT = CASE_SENSITIVE | USE_BRACES | USE_SETS
    };

    YGLOB_ENABLE_BITMASK_OPERATORS(GlobFlags);
}
