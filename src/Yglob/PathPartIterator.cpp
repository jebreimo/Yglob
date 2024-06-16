//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-06-16.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "PathPartIterator.hpp"

namespace Yglob
{
    SinglePathIterator::SinglePathIterator(std::filesystem::path path, bool has_next)
        : path_(std::move(path)),
          has_next_(has_next)
    {
    }

    void SinglePathIterator::set_base_path(std::filesystem::path base_path)
    {
        base_path_ = std::move(base_path);
        has_next_ = true;
    }

    bool SinglePathIterator::next()
    {
        if (!has_next_)
            return false;

        has_next_ = false;
        return exists(base_path_ / path_);
    }

    std::filesystem::path SinglePathIterator::path() const
    {
        return base_path_ / path_;
    }

    GlobIterator::GlobIterator(GlobMatcher matcher)
        : matcher_(std::move(matcher))
    {
    }

    bool GlobIterator::next()
    {
        while (it_ != end_)
        {
            auto filename = it_->path().filename().u8string();
            if (matcher_.match(ystring::to_string_view(filename)))
            {
                current_path_ = it_->path();
                ++it_;
                return true;
            }
            ++it_;
        }
        return false;
    }

    void GlobIterator::set_base_path(std::filesystem::path base_path)
    {
        base_path_ = std::move(base_path);
        it_ = std::filesystem::directory_iterator(base_path_);
        end_ = end(it_);
    }

    std::filesystem::path GlobIterator::path() const
    {
        return current_path_;
    }

    DoubleStarIterator::DoubleStarIterator(PathMatcher matcher)
        : matcher_(std::move(matcher))
    {
    }

    void DoubleStarIterator::set_base_path(std::filesystem::path base_path)
    {
        base_path_ = std::move(base_path);
        it_ = std::filesystem::recursive_directory_iterator(base_path_);
        end_ = end(it_);
    }

    bool DoubleStarIterator::next()
    {
        while (it_ != end_)
        {
            if (matcher_.match(it_->path()))
            {
                current_path_ = it_->path();
                ++it_;
                return true;
            }
            ++it_;
        }
        return false;
    }

    std::filesystem::path DoubleStarIterator::path() const
    {
        return current_path_;
    }

    bool PathPartIterator::next_directory()
    {
        while (next())
        {
            if (std::filesystem::is_directory(path()))
                return true;
        }
        return false;
    }
}
