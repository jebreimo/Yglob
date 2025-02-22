//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-03-27.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yglob/GlobMatcher.hpp"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Test GlobMatcher with empty pattern")
{
    using namespace Yglob;
    GlobMatcher matcher("");
    REQUIRE(matcher.match(""));
    REQUIRE(!matcher.match("a"));
    REQUIRE(!matcher.match("ab"));
}

TEST_CASE("Test GlobMatcher with exact match")
{
    using namespace Yglob;
    GlobMatcher matcher("abc");
    REQUIRE(matcher.match("abc"));
    REQUIRE(!matcher.match(""));
    REQUIRE(!matcher.match("ab"));
    REQUIRE(!matcher.match("abcd"));
    REQUIRE(!matcher.match("acb"));
}

TEST_CASE("Test GlobMatcher with just *")
{
    using namespace Yglob;
    GlobMatcher matcher("*");
    REQUIRE(matcher.match(""));
    REQUIRE(matcher.match("a"));
    REQUIRE(matcher.match("ac"));
    REQUIRE(matcher.match("axc"));
}

TEST_CASE("Test GlobMatcher with *")
{
    using namespace Yglob;
    GlobMatcher matcher("a*c");
    REQUIRE(matcher.match("abc"));
    REQUIRE(matcher.match("ab_bb_bc"));
    REQUIRE(matcher.match("ac"));
    REQUIRE(matcher.match("axc"));
    REQUIRE(!matcher.match("a"));
    REQUIRE(!matcher.match("ab"));
    REQUIRE(!matcher.match("abcd"));
    REQUIRE(!matcher.match("acb"));
}

TEST_CASE("Test GlobMatcher with ?")
{
    using namespace Yglob;
    GlobMatcher matcher("a?c");
    REQUIRE(matcher.match("abc"));
    REQUIRE(matcher.match("axc"));
    REQUIRE(!matcher.match("ac"));
    REQUIRE(!matcher.match("a"));
    REQUIRE(!matcher.match("ab"));
    REQUIRE(!matcher.match("abcd"));
    REQUIRE(!matcher.match("acb"));
}

TEST_CASE("Test GlobMatcher with [")
{
    using namespace Yglob;
    GlobMatcher matcher("a[b-d]e");
    REQUIRE(matcher.match("abe"));
    REQUIRE(matcher.match("ace"));
    REQUIRE(matcher.match("ade"));
    REQUIRE(!matcher.match(""));
    REQUIRE(!matcher.match("a"));
    REQUIRE(!matcher.match("ab"));
    REQUIRE(!matcher.match("ae"));
    REQUIRE(!matcher.match("aae"));
    REQUIRE(!matcher.match("aee"));
}

TEST_CASE("Test GlobMatcher with [^")
{
    using namespace Yglob;
    GlobMatcher matcher("a[^b-d]e");
    REQUIRE(!matcher.match("abe"));
    REQUIRE(!matcher.match("ace"));
    REQUIRE(!matcher.match("ade"));
    REQUIRE(matcher.match("aee"));
    REQUIRE(!matcher.match("a"));
    REQUIRE(!matcher.match("ab"));
    REQUIRE(!matcher.match("ae"));
    REQUIRE(matcher.match("aae"));
}

TEST_CASE("Test GlobMatcher with multiple patterns")
{
    using namespace Yglob;
    GlobMatcher matcher("a[b-d]e*");
    REQUIRE(matcher.match("abe"));
    REQUIRE(matcher.match("aceggg"));
    REQUIRE(matcher.match("ade123"));
    REQUIRE(!matcher.match("a"));
    REQUIRE(!matcher.match("ab"));
    REQUIRE(!matcher.match("ae"));
    REQUIRE(!matcher.match("aae"));
    REQUIRE(!matcher.match("aee"));
}

TEST_CASE("Test GlobMatcher with subpatterns")
{
    using namespace Yglob;
    GlobMatcher matcher("ab{cd,ef,}gh");
    REQUIRE(matcher.match("abcdgh"));
    REQUIRE(matcher.match("abefgh"));
    REQUIRE(matcher.match("abgh"));
    REQUIRE(!matcher.match("abcgh"));
    REQUIRE(!matcher.match("abdegh"));
}

TEST_CASE("Test GlobMatcher with escaped characters")
{
    using namespace Yglob;
    GlobMatcher matcher("a\\[b\\]c");
    REQUIRE(matcher.match("a[b]c"));
    REQUIRE(!matcher.match("abc"));
    REQUIRE(!matcher.match("a\\[b\\]c"));
    REQUIRE(!matcher.match("a\\[bc"));
    REQUIRE(!matcher.match("a[b\\]c"));
}

TEST_CASE("Test GlobMatcher with escaped characters in subpatterns")
{
    using namespace Yglob;
    GlobMatcher matcher("ab{c\\,d,e\\,f,}gh");
    REQUIRE(matcher.match("abc,dgh"));
    REQUIRE(matcher.match("abe,fgh"));
    REQUIRE(matcher.match("abgh"));
    REQUIRE(!matcher.match("abcgh"));
    REQUIRE(!matcher.match("abefgh"));
}

TEST_CASE("Test GlobMatcher with escaped characters in character ranges")
{
    using namespace Yglob;
    GlobMatcher matcher("a[b\\-d]e");
    REQUIRE(matcher.match("abe"));
    REQUIRE(matcher.match("a-e"));
    REQUIRE(matcher.match("ade"));
    REQUIRE(!matcher.match("a-ee"));
    REQUIRE(!matcher.match("ace"));
}

TEST_CASE("Test GlobMatcher with stars and question marks")
{
    using namespace Yglob;
    GlobMatcher matcher("aaa*?b?*c?dd");
    REQUIRE(matcher.match("aaabbbccdd"));
    REQUIRE(matcher.match("aaabbbccdccdd"));
    REQUIRE(matcher.match("aaabbbccddccdd"));
}

TEST_CASE("Test GlobMatcher with subpatterns within subpatterns")
{
    using namespace Yglob;
    GlobMatcher matcher("ab.{{pn,jp{e,}}g,gif}");
    REQUIRE(matcher.match("ab.png"));
    REQUIRE(matcher.match("ab.jpg"));
    REQUIRE(matcher.match("ab.jpeg"));
    REQUIRE(matcher.match("ab.gif"));
    REQUIRE(!matcher.match("ab.pnf"));
    REQUIRE(!matcher.match("ab.jpe"));
}

TEST_CASE("Test is_glob_pattern")
{
    using namespace Yglob;
    REQUIRE(!is_glob_pattern("a"));
    REQUIRE(!is_glob_pattern("a\\["));
    REQUIRE(!is_glob_pattern("a\\{"));
    REQUIRE(!is_glob_pattern("a\\*"));
    REQUIRE(!is_glob_pattern("a\\?"));
    REQUIRE(is_glob_pattern("a?"));
    REQUIRE(is_glob_pattern("a*"));
    REQUIRE(is_glob_pattern("a[b-d]"));
    REQUIRE(is_glob_pattern("a[^b-d]"));
    REQUIRE(is_glob_pattern("a{b,c,d}"));
}

TEST_CASE("GlobMatcher with sets disabled")
{
    using namespace Yglob;
    GlobMatcher matcher("a[b-d]e", GlobFlags::NO_SETS);
    REQUIRE(matcher.match("a[b-d]e"));
    REQUIRE(!matcher.match("abe"));
}

TEST_CASE("GlobMatcher with braces disabled")
{
    using namespace Yglob;
    GlobMatcher matcher("ab{c,d,e}f", GlobFlags::NO_BRACES);
    REQUIRE(matcher.match("ab{c,d,e}f"));
    REQUIRE(!matcher.match("abcf"));
}
