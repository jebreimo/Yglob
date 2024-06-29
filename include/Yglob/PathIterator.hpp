//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-06-12.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <filesystem>
#include "Flags.hpp"
#include "YglobDefinitions.hpp"

namespace Yglob
{
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
        const std::filesystem::path& path() const;
    private:
        class PathIteratorImpl;
        std::unique_ptr<PathIteratorImpl> impl_;
    };

    /**
     * @brief Adapts a PathIterator to be used in range-based for loops.
     *
     * This class is not intended to be used directly, use the begin() and
     * end() functions on PathIterator instead.
     */
    class PathInputIteratorAdapter
    {
    public:
        typedef std::filesystem::path value_type;
        typedef std::input_iterator_tag iterator_category;
        typedef void difference_type;
        typedef const value_type* pointer;
        typedef const value_type& reference;

        PathInputIteratorAdapter() = default;

        explicit PathInputIteratorAdapter(PathIterator& iterator)
            : iterator_(&iterator),
              is_end_(!iterator_->next())
        {}

        PathInputIteratorAdapter(const PathInputIteratorAdapter&) = delete;

        PathInputIteratorAdapter& operator++(int)
        {
            if (!is_end_)
                is_end_ = !iterator_->next();
            return *this;
        }

        void operator++()
        {
            if (!is_end_)
                is_end_ = !iterator_->next();
        }

        const std::filesystem::path& operator*() const
        {
            return iterator_->path();
        }

        const std::filesystem::path* operator->() const
        {
            return &iterator_->path();
        }
    private:
        friend bool operator==(const PathInputIteratorAdapter& lhs,
                               const PathInputIteratorAdapter& rhs);

        PathIterator* iterator_ = nullptr;
        bool is_end_ = true;
    };

    inline bool operator==(const PathInputIteratorAdapter& lhs,
                           const PathInputIteratorAdapter& rhs)
    {
        return lhs.iterator_ == rhs.iterator_ || lhs.is_end_ == rhs.is_end_;
    }

    inline bool operator!=(const PathInputIteratorAdapter& lhs,
                           const PathInputIteratorAdapter& rhs)
    {
        return !(lhs == rhs);
    }

    inline auto begin(PathIterator& iterator)
    {
        return PathInputIteratorAdapter(iterator);
    }

    inline auto end(const PathIterator&)
    {
        return PathInputIteratorAdapter();
    }
}
