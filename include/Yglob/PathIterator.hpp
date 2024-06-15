//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-06-12.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <filesystem>
#include "GlobFlags.hpp"
#include "YglobDefinitions.hpp"

namespace Yglob
{
    enum class PathIteratorFlags : unsigned
    {
        NONE = 0,
        CASE_SENSITIVE = 1,
        USE_BRACES = 2,
        USE_SETS = 4,
        FILES = 8,
        DIRECTORIES = 16,
        DEFAULT = CASE_SENSITIVE | USE_BRACES | USE_SETS | FILES
    };

    YGLOB_ENABLE_BITMASK_OPERATORS(PathIteratorFlags);

    class YGLOB_API PathIterator
    {
    public:
        PathIterator();

        explicit PathIterator(const std::filesystem::path& glob_path,
                              PathIteratorFlags flags = PathIteratorFlags::DEFAULT);

        ~PathIterator();

        PathIterator(const PathIterator& rhs) = delete;

        PathIterator(PathIterator&& rhs) noexcept;

        PathIterator& operator=(const PathIterator& rhs) = delete;

        PathIterator& operator=(PathIterator&& rhs) noexcept;

        bool next();

        [[nodiscard]]
        std::filesystem::path path() const;
    private:
        class PathIteratorImpl;
        std::unique_ptr<PathIteratorImpl> impl_;
    };
}
