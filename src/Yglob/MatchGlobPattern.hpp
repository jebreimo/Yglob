//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-06-12.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <span>
#include "GlobElements.hpp"

namespace Yglob
{
    bool match_fwd(std::span<GlobElement> parts, std::string_view& str,
                   bool case_sensitive,
                   bool is_subpattern);

    bool search_fwd(std::span<GlobElement> parts, std::string_view& str,
                    bool case_sensitive,
                    bool is_subpattern);

    bool match_end(std::span<GlobElement> parts, std::string_view& str,
                   bool case_sensitive);
}
