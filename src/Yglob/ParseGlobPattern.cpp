//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-06-12.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "ParseGlobPattern.hpp"

#include <ranges>
#include <Ystring/Algorithms.hpp>
#include <Ystring/Unescape.hpp>
#include "Yglob/YglobException.hpp"

namespace Yglob
{
    TokenType next_token_type(std::string_view pattern,
                              const GlobParserOptions& options)
    {
        if (pattern.empty())
            return TokenType::NONE;

        switch (pattern[0])
        {
        case '?':
            return TokenType::QUESTION_MARK;
        case '*':
            return TokenType::STAR;
        case '[':
            if (options.support_sets)
                return TokenType::OPEN_BRACKET;
            break;
        case '{':
            if (options.support_braces)
                return TokenType::OPEN_BRACE;
            break;
        default:
            break;
        }

        if (options.is_subpattern)
        {
            if (pattern[0] == '}')
                return TokenType::END_BRACE;
            if (pattern[0] == ',')
                return TokenType::COMMA;
        }
        return TokenType::CHAR;
    }

    ystring::CodepointSet extract_char_set(std::string_view& pattern)
    {
        // Remove '['
        pattern.remove_prefix(1);

        ystring::CodepointSet result;
        if (!pattern.empty() && pattern[0] == '^')
        {
            result.negated = true;
            pattern.remove_prefix(1);
        }

        enum class State
        {
            AWAITING_FIRST,
            HAS_FIRST,
            AWAITING_LAST
        };

        auto state = State::AWAITING_FIRST;
        bool did_unescape = false;
        while (auto ch = ystring::unescape_next(pattern, &did_unescape))
        {
            if (ch == U']' && !did_unescape)
            {
                if (state == State::AWAITING_LAST)
                    YGLOB_THROW("Incomplete character range in glob pattern.");
                return result;
            }
            else if (ch == U'-' && !did_unescape)
            {
                if (state == State::HAS_FIRST)
                    state = State::AWAITING_LAST;
                else if (state == State::AWAITING_FIRST && result.ranges.empty())
                    result.ranges.emplace_back('-', '-');
                else
                    YGLOB_THROW("Invalid character range in glob pattern.");
            }
            else if (state != State::AWAITING_LAST)
            {
                result.ranges.emplace_back(ch.value(), ch.value());
                state = State::HAS_FIRST;
            }
            else if (result.ranges.back().second < ch.value())
            {
                result.ranges.back().second = ch.value();
                state = State::AWAITING_FIRST;
            }
            else
            {
                YGLOB_THROW("Invalid character range in glob pattern.");
            }
        }
        YGLOB_THROW("Unmatched '[' in glob pattern.");
    }

    std::string extract_string(std::string_view& pattern,
                               const GlobParserOptions& options)
    {
        std::string result;
        while (!pattern.empty())
        {
            if (pattern[0] == '\\')
            {
                auto ch = ystring::unescape_next(pattern);
                ystring::append(result, ch.value());
            }
            else if (pattern[0] == '?'
                     || pattern[0] == '*'
                     || (options.support_sets && pattern[0] == '[')
                     || (options.support_braces && pattern[0] == '{')
                     || (options.is_subpattern
                         && (pattern[0] == '}' || pattern[0] == ',')))
            {
                break;
            }
            else
            {
                result.push_back(pattern[0]);
                pattern.remove_prefix(1);
            }
        }
        return result;
    }

    StarElement extract_stars(std::string_view& pattern)
    {
        while (!pattern.empty() && pattern[0] == '*')
        {
            pattern.remove_prefix(1);
        }
        return {};
    }

    QmarkElement extract_qmarks(std::string_view& pattern)
    {
        QmarkElement result;
        while (!pattern.empty() && pattern[0] == '?')
        {
            result.length++;
            pattern.remove_prefix(1);
        }
        return result;
    }

    // NOLINTBEGIN(misc-no-recursion)

    MultiGlob extract_multi_glob(std::string_view& pattern,
                                 GlobParserOptions options)
    {
        options.is_subpattern = true;

        MultiGlob result;
        while (!pattern.empty())
        {
            switch (next_token_type(pattern, options))
            {
            case TokenType::OPEN_BRACE:
            case TokenType::COMMA:
                pattern.remove_prefix(1);
                result.patterns.push_back(parse_glob_pattern(pattern, options));
                break;
            case TokenType::END_BRACE:
                if (result.patterns.empty())
                    YGLOB_THROW(
                        "EmptyElement subpattern in glob pattern. Did you mean to use '\\{\\}'?");
                pattern.remove_prefix(1);
                return result;
            default:
                YGLOB_THROW("Unmatched '{' in glob pattern.");
            }
        }
        YGLOB_THROW("Unmatched '{' in glob pattern.");
    }

    [[nodiscard]]
    bool has_star(const std::vector<GlobElement>& parts);

    [[nodiscard]]
    bool has_star(const GlobElement& part)
    {
        if (const auto multi_pattern = std::get_if<MultiGlob>(&part))
        {
            for (const auto& pattern: multi_pattern->patterns)
            {
                if (has_star(pattern->parts))
                    return true;
            }
        }
        return std::holds_alternative<StarElement>(part);
    }

    [[nodiscard]]
    bool has_star(const std::vector<GlobElement>& parts)
    {
        return std::ranges::any_of(parts, [](auto& p) {return has_star(p);});
    }

    void optimize(GlobElements& pattern)
    {
        pattern.tail_length = 0;
        for (auto& part : std::ranges::reverse_view(pattern.parts))
        {
            if (has_star(part))
                break;
            pattern.tail_length++;
        }
    }

    std::unique_ptr<GlobElements>
    parse_glob_pattern(std::string_view& pattern,
                       const GlobParserOptions& options)
    {
        auto result = std::make_unique<GlobElements>();

        bool done = false;
        while (!done)
        {
            switch (next_token_type(pattern, options))
            {
            case TokenType::CHAR:
                result->parts.emplace_back(extract_string(pattern, options));
                break;
            case TokenType::QUESTION_MARK:
                result->parts.emplace_back(extract_qmarks(pattern));
                break;
            case TokenType::STAR:
                result->parts.emplace_back(extract_stars(pattern));
                break;
            case TokenType::OPEN_BRACKET:
                result->parts.emplace_back(extract_char_set(pattern));
                break;
            case TokenType::OPEN_BRACE:
                result->parts.emplace_back(extract_multi_glob(pattern,
                                                              options));
                break;
            case TokenType::COMMA:
            case TokenType::END_BRACE:
            default:
                if (result->parts.empty())
                    result->parts.emplace_back(EmptyElement());
                done = true;
                break;
            }
        }

        if (!options.is_subpattern)
            optimize(*result);

        return result;
    }

    // NOLINTEND(misc-no-recursion)

}
