//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-03-30.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <iosfwd>
#include <memory>
#include <string>
#include <string_view>
#include <variant>
#include <vector>
#include <Ystring/CodepointSet.hpp>

namespace Yglob
{
    struct GlobElements;

    struct QmarkElement
    {
        size_t length = 0;
    };

    std::ostream& operator<<(std::ostream& os, const QmarkElement& qmark);

    struct StarElement
    {};

    std::ostream& operator<<(std::ostream& os, const StarElement& star);

    struct EmptyElement
    {};

    std::ostream& operator<<(std::ostream& os, const EmptyElement& empty);

    struct MultiGlob
    {
        MultiGlob() = default;

        MultiGlob(const MultiGlob& rhs)
            : patterns(rhs.patterns.size())
        {
            for (size_t i = 0; i < rhs.patterns.size(); ++i)
                patterns[i] = std::make_unique<GlobElements>(*rhs.patterns[i]);
        }

        MultiGlob(MultiGlob&& rhs) noexcept
            : patterns(std::move(rhs.patterns))
        {}

        ~MultiGlob() = default;

        MultiGlob& operator=(const MultiGlob& rhs)
        {
            if (this != &rhs)
            {
                patterns.resize(rhs.patterns.size());
                for (size_t i = 0; i < rhs.patterns.size(); ++i)
                    patterns[i] = std::make_unique<GlobElements>(*rhs.patterns[i]);
            }
            return *this;
        }

        MultiGlob& operator=(MultiGlob&& rhs) noexcept
        {
            patterns = std::move(rhs.patterns);
            return *this;
        }

        std::vector<std::unique_ptr<GlobElements>> patterns;
    };

    std::ostream& operator<<(std::ostream& os, const MultiGlob& multi_pattern);

    using GlobElement = std::variant<
        EmptyElement,
        StarElement,
        QmarkElement,
        ystring::CodepointSet,
        std::string,
        MultiGlob
        >;

    std::ostream& operator<<(std::ostream& os, const GlobElement& part);

    struct GlobElements
    {
        std::vector<GlobElement> parts;
        size_t tail_length = 0;
    };

    std::ostream& operator<<(std::ostream& os, const GlobElements& pattern);
}
