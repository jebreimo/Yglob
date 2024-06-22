//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-03-25.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yglob/GlobMatcher.hpp"

#include <Ystring/Unescape.hpp>
#include "MatchGlobPattern.hpp"
#include "ParseGlobPattern.hpp"

namespace Yglob
{
    GlobMatcher::GlobMatcher() = default;

    GlobMatcher::GlobMatcher(std::string_view pattern,
                             GlobFlags flags)
        : case_sensitive(bool(flags & GlobFlags::CASE_SENSITIVE)),
          pattern_(parse_glob_pattern(pattern,
                                      {!bool(flags & GlobFlags::NO_BRACES),
                                       !bool(flags & GlobFlags::NO_SETS)}))

    {}

    GlobMatcher::GlobMatcher(const GlobMatcher& rhs)
        : case_sensitive(rhs.case_sensitive),
          pattern_(rhs.pattern_ ? std::make_unique<GlobElements>(*rhs.pattern_)
                                : nullptr)
    {}

    GlobMatcher::GlobMatcher(GlobMatcher&& rhs) noexcept
        : case_sensitive(rhs.case_sensitive),
          pattern_(std::move(rhs.pattern_))
    {}

    GlobMatcher::~GlobMatcher() = default;

    GlobMatcher& GlobMatcher::operator=(const GlobMatcher& rhs)
    {
        if (this != &rhs)
        {
            pattern_ = rhs.pattern_
                       ? std::make_unique<GlobElements>(*rhs.pattern_)
                       : nullptr;
            case_sensitive = rhs.case_sensitive;
        }
        return *this;
    }

    GlobMatcher& GlobMatcher::operator=(GlobMatcher&& rhs) noexcept
    {
        pattern_ = std::move(rhs.pattern_);
        case_sensitive = rhs.case_sensitive;
        return *this;
    }

    [[nodiscard]]
    bool GlobMatcher::match(std::string_view str) const
    {
        if (!pattern_)
            return str.empty();

        auto length = pattern_->parts.size() - pattern_->tail_length;
        std::span parts(pattern_->parts.data(), length);
        std::span tail(pattern_->parts.data() + length, pattern_->tail_length);
        return match_end(tail, str, case_sensitive)
               && match_fwd(parts, str, case_sensitive, false);
    }

    std::ostream& operator<<(std::ostream& os, const GlobMatcher& matcher)
    {
        if (matcher.pattern_)
            os << *matcher.pattern_;
        return os;
    }

    bool is_glob_pattern(std::string_view str, GlobFlags flags)
    {
        GlobParserOptions parser_opts
        {
            !bool(flags & GlobFlags::NO_BRACES),
            !bool(flags & GlobFlags::NO_SETS)
        };

        while (!str.empty())
        {
            switch (next_token_type(str, parser_opts))
            {
            case TokenType::STAR:
            case TokenType::QUESTION_MARK:
            case TokenType::OPEN_BRACE:
            case TokenType::OPEN_BRACKET:
                return true;
            default:
                break;
            }
            std::ignore = ystring::unescape_next(str);
        }
        return false;
    }
}
