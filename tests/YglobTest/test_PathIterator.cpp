//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-06-12.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yglob/PathIterator.hpp"

#include <catch2/catch_test_macros.hpp>
#include "TempFiles.hpp"

namespace
{
    bool contains(const std::vector<std::filesystem::path>& paths,
                  const std::filesystem::path& path)
    {
        return std::find(paths.begin(), paths.end(), path) != paths.end();
    }
}

TEST_CASE("PathIterator with absolute paths")
{
    if (std::filesystem::exists("YglobTest"))
        std::filesystem::remove_all("YglobTest");
    TempFiles files{"YglobTest/abc.txt", "YglobTest/def.txt", "YglobTest/ghi.txt"};

    auto file_paths = files.files();

    Yglob::PathIterator it(file_paths[0]);
    REQUIRE(it.next());
    REQUIRE(it.path() == file_paths[0]);
    REQUIRE_FALSE(it.next());

    it = Yglob::PathIterator(files.get_path("YglobTest/*.txt"));
    REQUIRE(it.next());
    REQUIRE(contains(file_paths, it.path()));
    REQUIRE(it.next());
    REQUIRE(contains(file_paths, it.path()));
    REQUIRE(it.next());
    REQUIRE(contains(file_paths, it.path()));
    REQUIRE_FALSE(it.next());
}

TEST_CASE("PathIterator with local paths")
{
    if (std::filesystem::exists("YglobTest"))
        std::filesystem::remove_all("YglobTest");
    TempFiles files{"YglobTest/abc.txt", "YglobTest/def.txt", "YglobTest/ghi.txt"};

    std::filesystem::current_path(files.base_directory());

    auto file_paths = files.files();

    Yglob::PathIterator it("YglobTest/abc.txt");
    REQUIRE(it.next());
    REQUIRE(absolute(it.path()) == file_paths[0]);
    REQUIRE_FALSE(it.next());

    it = Yglob::PathIterator("YglobTest/*.txt");
    REQUIRE(it.next());
    REQUIRE(contains(file_paths, absolute(it.path())));
    REQUIRE(it.next());
    REQUIRE(contains(file_paths, absolute(it.path())));
    REQUIRE(it.next());
    REQUIRE(contains(file_paths, absolute(it.path())));
    REQUIRE_FALSE(it.next());
}
