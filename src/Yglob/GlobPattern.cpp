//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-03-30.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "GlobPattern.hpp"

#include <ostream>

namespace Yglob
{
    std::ostream& operator<<(std::ostream& os, const Qmark& qmark)
    {
        for (size_t i = 0; i < qmark.length; ++i)
            os << '?';
        return os;
    }

    std::ostream& operator<<(std::ostream& os, const Star&)
    {
        os << '*';
        return os;
    }

    std::ostream& operator<<(std::ostream& os, const Empty&)
    {
        return os;
    }

    std::ostream& operator<<(std::ostream& os, const MultiPattern& multi_pattern)
    {
        os << '{';
        for (size_t i = 0; i < multi_pattern.patterns.size(); ++i)
        {
            if (i)
                os << ',';
            os << *multi_pattern.patterns[i];
        }
        os << '}';
        return os;
    }

    std::ostream& operator<<(std::ostream& os, const Part& part)
    {
        std::visit([&os](const auto& p) {os << p;}, part);
        return os;
    }

    std::ostream& operator<<(std::ostream& os, const GlobPattern& pattern)
    {
        for (const auto& part : pattern.parts)
            os << part;
        return os;
    }
}
