//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-06-02.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yglob/PathMatcher.hpp"
#include <filesystem>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Relative glob, relative paths")
{
    Yglob::PathMatcher matcher(std::filesystem::path("abc/*.txt"));
    REQUIRE(matcher.match(std::filesystem::path("abc/def.txt")));
}

TEST_CASE("Relative glob, relative path starting with ./")
{
    Yglob::PathMatcher matcher(std::filesystem::path("abc/*.txt"));
    REQUIRE(matcher.match(std::filesystem::path("./abc/a.txt")));
}

TEST_CASE("Relative glob, absolute path")
{
    Yglob::PathMatcher matcher(std::filesystem::path("abc/*.txt"));
    REQUIRE_FALSE(matcher.match(std::filesystem::path("/abc/a.txt")));
}

TEST_CASE("Relative glob starting with ./, relative path")
{
    Yglob::PathMatcher matcher(std::filesystem::path("./abc/*.txt"));
    REQUIRE(matcher.match(std::filesystem::path("abc/a.txt")));
}

TEST_CASE("Absolute glob, relative path")
{
    Yglob::PathMatcher matcher(std::filesystem::path("/abc/cde/*.txt"));
    REQUIRE_FALSE(matcher.match(std::filesystem::path("abc/cde/a.txt")));
}

TEST_CASE("Absolute glob, absolute path")
{
    Yglob::PathMatcher matcher(std::filesystem::path("/abc/cde/*.txt"));
    REQUIRE(matcher.match(std::filesystem::path("/abc/cde/a.txt")));
}

TEST_CASE("Multi-glob")
{
    Yglob::PathMatcher matcher(std::filesystem::path("/*/cde/*.txt"));
    REQUIRE(matcher.match(std::filesystem::path("/abc/cde/a.txt")));
}

TEST_CASE("Un-rooted multi-dir glob matches rooted path")
{
    Yglob::PathMatcher matcher(std::filesystem::path("**/cde/*.txt"));
    REQUIRE(matcher.match(std::filesystem::path("/abc/cde/a.txt")));
}

TEST_CASE("Un-rooted multi-dir glob matches un-rooted path")
{
    Yglob::PathMatcher matcher(std::filesystem::path("**/cde/*.txt"));
    REQUIRE(matcher.match(std::filesystem::path("abc/cde/a.txt")));
}

TEST_CASE("Rooted multi-dir glob matches rooted path")
{
    Yglob::PathMatcher matcher(std::filesystem::path("/**/cde/*.txt"));
    REQUIRE(matcher.match(std::filesystem::path("/abc/cde/a.txt")));
}

TEST_CASE("Rooted multi-dir glob does not match un-rooted path")
{
    Yglob::PathMatcher matcher(std::filesystem::path("/**/cde/*.txt"));
    REQUIRE_FALSE(matcher.match(std::filesystem::path("abc/cde/a.txt")));
}

TEST_CASE("Multi-dir glob 2")
{
    Yglob::PathMatcher matcher(std::filesystem::path("abc/**/cde/*.txt"));
    REQUIRE(matcher.match(std::filesystem::path("abc/a/b/cde/a.txt")));
}
