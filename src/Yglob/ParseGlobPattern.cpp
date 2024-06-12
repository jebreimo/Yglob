//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-06-12.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "ParseGlobPattern.hpp"

#include <Ystring/Algorithms.hpp>
#include <Ystring/DecodeUtf8.hpp>
#include <Ystring/Unescape.hpp>

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
        State state = State::AWAITING_FIRST;
        bool did_unescape = false;
        while (auto ch = ystring::unescape_next(pattern, &did_unescape))
        {
            if (ch == U']' && !did_unescape)
            {
                if (state == State::AWAITING_LAST)
                    YSTRING_THROW("Incomplete character range in glob pattern.");
                return result;
            }
            else if (ch == U'-' && !did_unescape)
            {
                if (state == State::HAS_FIRST)
                    state = State::AWAITING_LAST;
                else if (state == State::AWAITING_FIRST && result.ranges.empty())
                    result.ranges.emplace_back('-', '-');
                else
                    YSTRING_THROW("Invalid character range in glob pattern.");
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
                YSTRING_THROW("Invalid character range in glob pattern.");
            }
        }
        YSTRING_THROW("Unmatched '[' in glob pattern.");
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

    Star extract_stars(std::string_view& pattern)
    {
        Star result;
        while (!pattern.empty() && pattern[0] == '*')
        {
            pattern.remove_prefix(1);
        }
        return result;
    }

    Qmark extract_qmarks(std::string_view& pattern)
    {
        Qmark result;
        while (!pattern.empty() && pattern[0] == '?')
        {
            result.length++;
            pattern.remove_prefix(1);
        }
        return result;
    }

    // NOLINTBEGIN(misc-no-recursion)
    MultiPattern extract_multi_pattern(std::string_view& pattern,
                                       GlobParserOptions options)
    {
        options.is_subpattern = true;

        MultiPattern result;
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
                    YSTRING_THROW(
                        "Empty subpattern in glob pattern. Did you mean to use '\\{\\}'?");
                pattern.remove_prefix(1);
                return result;
            default:
                YSTRING_THROW("Unmatched '{' in glob pattern.");
            }
        }
        YSTRING_THROW("Unmatched '{' in glob pattern.");
    }

    [[nodiscard]]
    bool has_star(const std::vector<Part>& parts);

    [[nodiscard]]
    bool has_star(const Part& part)
    {
        if (auto multi_pattern = std::get_if<MultiPattern>(&part))
        {
            for (const auto& pattern: multi_pattern->patterns)
            {
                if (has_star(pattern->parts))
                    return true;
            }
        }
        return std::holds_alternative<Star>(part);
    }

    [[nodiscard]]
    bool has_star(const std::vector<Part>& parts)
    {
        return std::any_of(parts.begin(), parts.end(),
                           [](auto& p) {return has_star(p);}
        );
    }

    void optimize(GlobPattern& pattern)
    {
        pattern.tail_length = 0;
        for (auto it = pattern.parts.rbegin(); it != pattern.parts.rend(); ++it)
        {
            if (has_star(*it))
                break;
            pattern.tail_length++;
        }
    }

    std::unique_ptr<GlobPattern>
    parse_glob_pattern(std::string_view& pattern,
                       const GlobParserOptions& options)
    {
        auto result = std::make_unique<GlobPattern>();

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
                result->parts.emplace_back(extract_multi_pattern(pattern,
                                                                 options));
                break;
            case TokenType::COMMA:
            case TokenType::END_BRACE:
            default:
                if (result->parts.empty())
                    result->parts.emplace_back(Empty());
                done = true;
                break;
            }
        }

        if (!options.is_subpattern)
            optimize(*result);

        return result;
    }
}
