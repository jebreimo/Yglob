//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-06-12.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "MatchGlobPattern.hpp"

#include <Ystring/Algorithms.hpp>

namespace Yglob
{
    namespace detail
    {
        bool ends_with(const std::string_view str,
                       const std::string_view cmp,
                       const bool case_sensitive)
        {
            return case_sensitive
                       ? ystring::ends_with(str, cmp)
                       : ystring::case_insensitive::ends_with(str, cmp);
        }

        bool starts_with(const std::string_view str,
                         const std::string_view cmp,
                         const bool case_sensitive)
        {
            return case_sensitive
                       ? ystring::starts_with(str, cmp)
                       : ystring::case_insensitive::starts_with(str, cmp);
        }

        bool contains(const ystring::CodepointSet& set, const char32_t ch,
                      const bool case_sensitive)
        {
            return case_sensitive
                       ? set.contains(ch)
                       : set.case_insensitive_contains(ch);
        }
    }

    bool starts_with(std::string_view& str, const GlobElement& part,
                     const bool case_sensitive)
    {
        struct StartsWithVisitor
        {
            bool operator()(const std::string& s) const
            {
                if (detail::starts_with(str, s, case_sensitive))
                {
                    str.remove_prefix(s.size());
                    return true;
                }
                return false;
            }

            bool operator()(const ystring::CodepointSet& s) const
            {
                if (const auto ch = ystring::pop_utf8_codepoint(str))
                {
                    return detail::contains(s, *ch, case_sensitive);
                }
                return false;
            }

            bool operator()(const MultiGlob& mp) const
            {
                for (auto& pattern: mp.patterns)
                {
                    if (match_fwd(std::span(pattern->parts), str,
                                  case_sensitive, true))
                    {
                        return true;
                    }
                }
                return false;
            }

            bool operator()(const QmarkElement& qm) const
            {
                for (size_t i = 0; i < qm.length; ++i)
                {
                    if (!ystring::remove_utf8_codepoint(str))
                        return false;
                }
                return true;
            }

            bool operator()(const StarElement&) const
            {
                str.remove_prefix(str.size());
                return true;
            }

            bool operator()(const EmptyElement&) const
            {
                return true;
            }

            std::string_view& str;
            bool case_sensitive;
        };

        return std::visit(StartsWithVisitor{str, case_sensitive}, part);
    }

    bool ends_with(std::string_view& str,
                   const GlobElement& part,
                   const bool case_sensitive)
    {
        struct EndsWithVisitor
        {
            bool operator()(const std::string& s) const
            {
                if (detail::ends_with(str, s, case_sensitive))
                {
                    str.remove_suffix(s.size());
                    return true;
                }
                return false;
            }

            bool operator()(const ystring::CodepointSet& s) const
            {
                if (const auto ch = ystring::pop_last_utf8_codepoint(str))
                {
                    return detail::contains(s, *ch, case_sensitive);
                }
                return false;
            }

            bool operator()(const MultiGlob& mp) const
            {
                for (auto& pattern: mp.patterns)
                {
                    const std::span parts(pattern->parts);
                    if (match_end(parts, str, case_sensitive))
                        return true;
                }
                return false;
            }

            bool operator()(const QmarkElement& qm) const
            {
                for (size_t i = 0; i < qm.length; ++i)
                {
                    if (!ystring::remove_last_utf8_codepoint(str))
                        return false;
                }
                return true;
            }

            bool operator()(const StarElement&) const
            {
                return false;
            }

            bool operator()(const EmptyElement&) const
            {
                return true;
            }

            std::string_view& str;
            bool case_sensitive;
        };

        return std::visit(EndsWithVisitor{str, case_sensitive}, part);
    }

    // NOLINTBEGIN(misc-no-recursion)

    bool match_fwd(const std::span <GlobElement> parts,
                   std::string_view& str,
                   const bool case_sensitive,
                   const bool is_subpattern)
    {
        const auto str_copy = str;
        for (size_t i = 0; i < parts.size(); ++i)
        {
            if (std::holds_alternative <StarElement>(parts[i])
                && search_fwd(parts.subspan(i + 1), str,
                              case_sensitive, is_subpattern))
            {
                return true;
            }
            else if (!starts_with(str, parts[i], case_sensitive))
            {
                str = str_copy;
                return false;
            }
        }

        if (str.empty() || is_subpattern)
            return true;

        str = str_copy;
        return false;
    }

    bool search_fwd(const std::span <GlobElement> parts,
                    std::string_view& str,
                    const bool case_sensitive,
                    const bool is_subpattern)
    {
        if (parts.empty())
        {
            str.remove_prefix(str.size());
            return true;
        }

        while (!str.empty())
        {
            if (match_fwd(parts, str, case_sensitive, is_subpattern))
                return true;

            ystring::remove_utf8_codepoint(str);
        }

        return false;
    }

    bool match_end(const std::span <GlobElement> parts,
                   std::string_view& str,
                   const bool case_sensitive)
    {
        const auto str_copy = str;
        for (size_t i = parts.size(); i-- > 0;)
        {
            if (!ends_with(str, parts[i], case_sensitive))
            {
                str = str_copy;
                return false;
            }
        }
        return true;
    }

    // NOLINTEND(misc-no-recursion)
}
