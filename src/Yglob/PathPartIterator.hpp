//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-06-16.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <Ystring/Algorithms.hpp>
#include "Yglob/GlobMatcher.hpp"
#include "Yglob/PathMatcher.hpp"

namespace Yglob
{
    class PathPartIterator
    {
    public:
        virtual ~PathPartIterator() = default;

        virtual void set_base_path(std::filesystem::path base_path) = 0;

        virtual bool next() = 0;

        bool next_directory();

        [[nodiscard]]
        virtual const std::filesystem::path& path() const = 0;
    };

    class SinglePathIterator : public PathPartIterator
    {
    public:
        SinglePathIterator(std::filesystem::path path, bool has_next);

        void set_base_path(std::filesystem::path base_path) override;

        bool next() override;

        [[nodiscard]]
        const std::filesystem::path& path() const override;

    public:
        std::filesystem::path base_path_;
        std::filesystem::path path_;
        std::filesystem::path current_path_;
        bool has_next_ = true;
    };

    class GlobIterator : public PathPartIterator
    {
    public:
        GlobIterator(GlobMatcher matcher,
                     std::filesystem::directory_options options);

        bool next() override;

        void set_base_path(std::filesystem::path base_path) override;

        [[nodiscard]]
        const std::filesystem::path& path() const override;

    private:
        std::filesystem::directory_iterator it_;
        std::filesystem::directory_iterator end_;
        std::filesystem::path base_path_;
        std::filesystem::path current_path_;
        GlobMatcher matcher_;
        std::filesystem::directory_options options_;
    };

    class DoubleStarIterator : public PathPartIterator
    {
    public:
        DoubleStarIterator(PathMatcher matcher,
                           std::filesystem::directory_options options);

        void set_base_path(std::filesystem::path base_path) override;

        bool next() override;

        [[nodiscard]]
        const std::filesystem::path& path() const override;

    private:
        std::filesystem::recursive_directory_iterator it_;
        std::filesystem::recursive_directory_iterator end_;
        std::filesystem::path base_path_;
        std::filesystem::path current_path_;
        PathMatcher matcher_;
        std::filesystem::directory_options options_;
    };
}
