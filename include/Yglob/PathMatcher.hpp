//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-05-24.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <filesystem>
#include <memory>
#include <string_view>
#include "GlobFlags.hpp"
#include "YglobDefinitions.hpp"

namespace Yglob
{
    class YGLOB_API PathMatcher
    {
    public:
        PathMatcher();

        explicit PathMatcher(std::string_view pattern,
                             GlobFlags flags = GlobFlags::DEFAULT);

        explicit PathMatcher(const std::filesystem::path& pattern,
                             GlobFlags flags = GlobFlags::DEFAULT);

        PathMatcher(const PathMatcher& rhs);

        PathMatcher(PathMatcher&& rhs) noexcept;

        ~PathMatcher();

        PathMatcher& operator=(const PathMatcher& rhs);

        PathMatcher& operator=(PathMatcher&& rhs) noexcept;

        [[nodiscard]]
        bool match(std::string_view str) const;

        [[nodiscard]]
        bool match(const std::filesystem::path& str) const;
    private:
        class PathMatcherImpl;
        std::unique_ptr<PathMatcherImpl> impl_;
    };
}
