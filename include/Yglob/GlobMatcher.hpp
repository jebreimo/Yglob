//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-03-25.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <iosfwd>
#include <memory>
#include <string_view>
#include "GlobFlags.hpp"
#include "YglobDefinitions.hpp"

namespace Yglob
{
    struct GlobElements;

    class YGLOB_API GlobMatcher
    {
    public:
        GlobMatcher();

        explicit GlobMatcher(std::string_view pattern,
                             GlobFlags flags = GlobFlags::DEFAULT);

        GlobMatcher(const GlobMatcher& rhs);

        GlobMatcher(GlobMatcher&& rhs) noexcept;

        ~GlobMatcher();

        GlobMatcher& operator=(const GlobMatcher& rhs);

        GlobMatcher& operator=(GlobMatcher&& rhs) noexcept;

        bool case_sensitive = true;

        [[nodiscard]]
        bool match(std::string_view str) const;
    private:
        friend YGLOB_API std::ostream&
        operator<<(std::ostream&, const GlobMatcher&);

        std::unique_ptr<GlobElements> pattern_;
    };

    YGLOB_API std::ostream&
    operator<<(std::ostream& os, const GlobMatcher& matcher);

    [[nodiscard]]
    YGLOB_API bool
    is_glob_pattern(std::string_view str,
                    GlobFlags flags = GlobFlags::DEFAULT);
}
