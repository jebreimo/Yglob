//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-06-12.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "GlobElements.hpp"

namespace Yglob
{
    struct GlobParserOptions
    {
        bool support_braces = true;
        bool support_sets = true;
        bool is_subpattern = false;
    };

    std::unique_ptr<GlobElements>
    parse_glob_pattern(std::string_view& pattern,
                       const GlobParserOptions& options);

    enum class TokenType
    {
        NONE,
        CHAR,
        QUESTION_MARK,
        STAR,
        OPEN_BRACKET,
        OPEN_BRACE,
        COMMA,
        END_BRACE
    };

    TokenType next_token_type(std::string_view pattern,
                              const GlobParserOptions& options);

    ystring::CodepointSet extract_char_set(std::string_view& pattern);

    std::string extract_string(std::string_view& pattern,
                               const GlobParserOptions& options);

    StarElement extract_stars(std::string_view& pattern);

    QmarkElement extract_qmarks(std::string_view& pattern);

    MultiGlob extract_multi_glob(std::string_view& pattern,
                                 GlobParserOptions options);
}
