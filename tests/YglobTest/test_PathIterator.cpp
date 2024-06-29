//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-06-12.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yglob/PathIterator.hpp"

#include <ranges>
#include <catch2/catch_test_macros.hpp>
#include "TempFiles.hpp"

namespace
{
    bool contains(const std::vector<std::filesystem::path>& paths,
                  const std::filesystem::path& path)
    {
        return std::ranges::find(paths, path) != paths.end();
    }

    struct AutoCwd
    {
        explicit AutoCwd(const std::filesystem::path& path)
            : prev_path(std::filesystem::current_path())
        {
            std::filesystem::current_path(path);
        }

        ~AutoCwd()
        {
            std::filesystem::current_path(prev_path);
        }

        std::filesystem::path prev_path;
    };
}

TEST_CASE("Case-sensitive PathIterator with absolute paths")
{
    TempFiles files("YglobTest", true);
    files.make_files({"a/abc.txt", "a/def.txt", "a/ghi.txt"});

    auto file_paths = files.files();

    Yglob::PathIterator it(file_paths[0]);
    REQUIRE(it.next());
    REQUIRE(it.path() == file_paths[0]);
    REQUIRE_FALSE(it.next());
    REQUIRE_FALSE(it.next());

    it = Yglob::PathIterator(files.get_path("a/*.txt"));
    REQUIRE(it.next());
    REQUIRE(contains(file_paths, it.path()));
    REQUIRE(it.next());
    REQUIRE(contains(file_paths, it.path()));
    REQUIRE(it.next());
    REQUIRE(contains(file_paths, it.path()));
    REQUIRE_FALSE(it.next());
}

TEST_CASE("Case-insensitive PathIterator with absolute paths")
{
    TempFiles files("YglobTest", true);
    files.make_files({"a/abc.txt", "a/def.txt", "a/ghi.txt"});

    auto file_paths = files.files();

    auto it = Yglob::PathIterator(files.get_path("A/*.TXT"),
                                  Yglob::PathIteratorFlags::CASE_INSENSITIVE_PATHS);
    REQUIRE(it.next());
    REQUIRE(contains(file_paths, it.path()));
    REQUIRE(it.next());
    REQUIRE(contains(file_paths, it.path()));
    REQUIRE(it.next());
    REQUIRE(contains(file_paths, it.path()));
    REQUIRE_FALSE(it.next());
}

TEST_CASE("Case-sensitive PathIterator with local paths")
{
    TempFiles files("YglobTest", true);
    files.make_files({"a/abc.txt", "a/def.txt", "a/ghi.txt"});

    AutoCwd auto_cwd(files.base_directory());

    auto file_paths = files.files();

    Yglob::PathIterator it("a/abc.txt");
    REQUIRE(it.next());
    REQUIRE(absolute(it.path()) == file_paths[0]);
    REQUIRE_FALSE(it.next());

    it = Yglob::PathIterator("a/*.txt");
    REQUIRE(it.next());
    REQUIRE(contains(file_paths, absolute(it.path())));
    REQUIRE(it.next());
    REQUIRE(contains(file_paths, absolute(it.path())));
    REQUIRE(it.next());
    REQUIRE(contains(file_paths, absolute(it.path())));
    REQUIRE_FALSE(it.next());
}

TEST_CASE("PathIterator with local paths starting with glob")
{
    TempFiles files("YglobTest", true);
    files.make_files({"a/abc.txt", "a/def.txt", "a/ghi.txt"});

    AutoCwd auto_cwd(files.base_directory());

    auto file_paths = files.files();

    Yglob::PathIterator it("*/*.txt");
    REQUIRE(it.next());
    REQUIRE(contains(file_paths, canonical(it.path())));
    REQUIRE(it.next());
    REQUIRE(contains(file_paths, canonical(it.path())));
    REQUIRE(it.next());
    REQUIRE(contains(file_paths, canonical(it.path())));
    REQUIRE_FALSE(it.next());
}

TEST_CASE("Case-insensitive PathIterator with local paths")
{
    TempFiles files("YglobTest", true);
    files.make_files({"a/abc.txt", "a/def.txt", "a/ghi.txt"});

    AutoCwd auto_cwd(files.base_directory());

    auto file_paths = files.files();

    Yglob::PathIterator it("A/*.TXT", Yglob::PathIteratorFlags::CASE_INSENSITIVE_PATHS);
    REQUIRE(it.next());
    REQUIRE(contains(file_paths, canonical(it.path())));
    REQUIRE(it.next());
    REQUIRE(contains(file_paths, canonical(it.path())));
    REQUIRE(it.next());
    REQUIRE(contains(file_paths, canonical(it.path())));
    REQUIRE_FALSE(it.next());
}

TEST_CASE("PathIterator with recursive paths and just files")
{
    TempFiles files("YglobTest", true);
    files.make_files({"abc.txt", "a/def.txt", "b/ghi.txt"});

    auto file_paths = files.files();

    Yglob::PathIterator it(files.get_path("**"),
                           Yglob::PathIteratorFlags::NO_DIRECTORIES);
    REQUIRE(it.next());
    REQUIRE(contains(file_paths, it.path()));
    REQUIRE(it.next());
    REQUIRE(contains(file_paths, it.path()));
    REQUIRE(it.next());
    REQUIRE(contains(file_paths, it.path()));
    REQUIRE_FALSE(it.next());

    it = Yglob::PathIterator(files.get_path("*/**"));
    REQUIRE(it.next());
    REQUIRE(contains(file_paths, it.path()));
    REQUIRE(it.next());
    REQUIRE(contains(file_paths, it.path()));
    REQUIRE_FALSE(it.next());

    it = Yglob::PathIterator(files.get_path("**/*.txt"));
    REQUIRE(it.next());
    REQUIRE(contains(file_paths, it.path()));
    REQUIRE(it.next());
    REQUIRE(contains(file_paths, it.path()));
    REQUIRE(it.next());
    REQUIRE(contains(file_paths, it.path()));
    REQUIRE_FALSE(it.next());
}

TEST_CASE("PathIterator with recursive paths and no files")
{
    TempFiles files("YglobTest", true);
    files.make_files({"abc.txt", "a/def.txt", "b/ghi.txt"});

    auto dir_paths = files.directories();
    // Remove YglobTest itself.
    dir_paths.erase(dir_paths.begin());

    Yglob::PathIterator it(files.get_path("**"),
                           Yglob::PathIteratorFlags::NO_FILES);
    REQUIRE(it.next());
    REQUIRE(contains(dir_paths, it.path()));
    REQUIRE(it.next());
    REQUIRE(contains(dir_paths, it.path()));
    REQUIRE_FALSE(it.next());
}

TEST_CASE("PathIterator with local recursive path")
{
    TempFiles files("YglobTest", true);
    files.make_files({"abc.txt", "a/def.txt", "b/ghi.txt"});

    AutoCwd auto_cwd(files.base_directory());

    auto file_paths = files.files();

    Yglob::PathIterator it("**/*.txt");
    REQUIRE(it.next());
    REQUIRE(contains(file_paths, canonical(it.path())));
    REQUIRE(it.next());
    REQUIRE(contains(file_paths, canonical(it.path())));
    REQUIRE(it.next());
    REQUIRE(contains(file_paths, canonical(it.path())));
    REQUIRE_FALSE(it.next());
}

TEST_CASE("Case-sensitive PathIterator")
{
    TempFiles files("YglobTest", true);
    files.make_files({"abc.TXT", "a/def.TXT", "b/ghi.txt"});

    auto dir_paths = files.directories();
    // Remove YglobTest itself.
    dir_paths.erase(dir_paths.begin());

    Yglob::PathIterator it(files.get_path("**/*.txt"),
                           Yglob::PathIteratorFlags::CASE_SENSITIVE_GLOBS);
    REQUIRE(it.next());
    REQUIRE(it.path() == files.get_path("b/ghi.txt"));
    REQUIRE_FALSE(it.next());
}
