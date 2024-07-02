//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-06-12.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yglob/PathIterator.hpp"
#include <Ystring/Algorithms.hpp>
#include "Yglob/GlobMatcher.hpp"
#include "Yglob/PathMatcher.hpp"
#include "PathPartIterator.hpp"

namespace Yglob
{
    namespace
    {
        std::filesystem::path
        make_path(const std::filesystem::path::const_iterator& begin,
                  const std::filesystem::path::const_iterator& end,
                  const std::filesystem::path& prefix)
        {
            std::filesystem::path path = prefix;
            for (auto it = begin; it != end; ++it)
                path /= *it;
            return path;
        }

        void handle_plain_path(std::vector<std::unique_ptr<PathPartIterator>>& iterators,
                               std::filesystem::path& path)
        {
            if (!path.empty())
            {
                iterators.emplace_back(
                    std::make_unique<SinglePathIterator>(std::move(path),
                                                         iterators.empty()));
                path = std::filesystem::path();
            }
        }

        GlobFlags to_glob_flags(PathIteratorFlags flags)
        {
            GlobFlags result = {};
            if (bool(flags & PathIteratorFlags::CASE_SENSITIVE_GLOBS))
                result |= GlobFlags::CASE_SENSITIVE;
            if (bool(flags & PathIteratorFlags::NO_BRACES))
                result |= GlobFlags::NO_BRACES;
            if (bool(flags & PathIteratorFlags::NO_SETS))
                result |= GlobFlags::NO_SETS;
            return result;
        }

        std::filesystem::directory_options
        to_directory_options(PathIteratorFlags flags)
        {
            std::filesystem::directory_options result = {};
            if (!bool(flags & PathIteratorFlags::THROW_IF_ACCESS_DENIED))
                result |= std::filesystem::directory_options::skip_permission_denied;
            return result;
        }

        std::vector<std::unique_ptr<PathPartIterator>>
        parse_glob_path(const std::filesystem::path& path,
                        PathIteratorFlags flags)
        {
            std::vector<std::unique_ptr<PathPartIterator>> result;
            std::filesystem::path plain_path;

            auto case_insensitive_paths = bool(flags & PathIteratorFlags::CASE_INSENSITIVE_PATHS);
            for (auto it = path.begin(), end = path.end(); it != end; ++it)
            {
                auto name = it->generic_u8string();
                if (name == u8"**")
                {
                    handle_plain_path(result, plain_path);
                    result.emplace_back(std::make_unique<DoubleStarIterator>(
                        PathMatcher(make_path(++it, end, u8"**")),
                        to_directory_options(flags)));
                    if (result.size() == 1)
                        result.back()->set_base_path(".");
                    break;
                }

                std::optional<GlobFlags> glob_flags;
                if (case_insensitive_paths && !it->has_root_path())
                    glob_flags = GlobFlags::NO_SETS | GlobFlags::NO_BRACES;
                else if (is_glob_pattern(ystring::to_string_view(name)))
                    glob_flags = to_glob_flags(flags);

                if (glob_flags)
                {
                    handle_plain_path(result, plain_path);
                    result.emplace_back(std::make_unique<GlobIterator>(
                        GlobMatcher(ystring::to_string_view(name),
                                    *glob_flags),
                        to_directory_options(flags)));
                    if (result.size() == 1)
                        result.back()->set_base_path(".");
                }
                else
                {
                    plain_path /= *it;
                }
            }

            handle_plain_path(result, plain_path);
            return result;
        }
    }

    class PathIterator::PathIteratorImpl
    {
    public:
        explicit PathIteratorImpl(const std::filesystem::path& glob_path,
                                  PathIteratorFlags flags)
            : iterators_(parse_glob_path(glob_path, flags)),
              flags_(flags)
        {}

        bool next()
        {
            if (iterators_.empty())
                return false;
            while (iterators_.back()->next())
            {
                if (is_acceptable(iterators_.back()->path()))
                    return true;
            }

            auto last = --iterators_.cend();
            auto it = last;
            while (true)
            {
                if (!find_prev_with_next(it))
                    return false;

                std::filesystem::path path = (*it)->path();
                for (++it; it != last; ++it)
                {
                    (*it)->set_base_path(std::move(path));
                    if (!(*it)->next_directory())
                        break;
                    path = (*it)->path();
                }
                if (it == last)
                {
                    (*it)->set_base_path(std::move(path));
                    while ((*it)->next())
                    {
                        if (is_acceptable((*it)->path()))
                            return true;
                    }
                }
            }
        }

        [[nodiscard]]
        const std::filesystem::path& path() const
        {
            return iterators_.back()->path();
        }

    private:
        using Container = std::vector<std::unique_ptr<PathPartIterator>>;

        bool find_prev_with_next(Container::const_iterator& it) const
        {
            while (it != iterators_.cbegin())
            {
                --it;
                if ((*it)->next_directory())
                    return true;
            }
            return false;
        }

        [[nodiscard]]
        bool is_acceptable(const std::filesystem::path& path) const
        {
            auto no_files = bool(flags_ & PathIteratorFlags::NO_FILES);
            auto no_dirs = bool(flags_ & PathIteratorFlags::NO_DIRECTORIES);
            return (!no_files || !std::filesystem::is_regular_file(path))
                   && (!no_dirs || !std::filesystem::is_directory(path));
        }

        std::vector<std::unique_ptr<PathPartIterator>> iterators_;
        PathIteratorFlags flags_;
    };

    PathIterator::PathIterator() = default;

    PathIterator::PathIterator(const std::filesystem::path& glob_path,
                               PathIteratorFlags flags)
        : impl_(std::make_unique<PathIteratorImpl>(glob_path, flags))
    {}

    PathIterator::~PathIterator() = default;

    PathIterator::PathIterator(PathIterator&& rhs) noexcept
        : impl_(std::move(rhs.impl_))
    {}

    PathIterator& PathIterator::operator=(PathIterator&& rhs) noexcept
    {
        if (this != &rhs)
            impl_ = std::move(rhs.impl_);
        return *this;
    }

    bool PathIterator::next()
    {
        return impl_ && impl_->next();
    }

    const std::filesystem::path& PathIterator::path() const
    {
        static const std::filesystem::path empty_path;
        return impl_ ? impl_->path() : empty_path;
    }
}
