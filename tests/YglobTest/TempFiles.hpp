//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-06-12.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <filesystem>
#include <vector>

class TempFiles
{
public:
    explicit TempFiles(const std::filesystem::path& base_path,
                       bool erase = false);

    ~TempFiles();

    TempFiles& make_file(const std::filesystem::path& path);

    TempFiles& make_files(const std::vector<std::filesystem::path>& paths);

    TempFiles& make_directory(const std::filesystem::path& path);

    [[nodiscard]]
    std::filesystem::path base_directory() const;

    [[nodiscard]]
    std::vector<std::filesystem::path> files() const;

    [[nodiscard]]
    std::vector<std::filesystem::path> directories() const;

    [[nodiscard]]
    std::filesystem::path get_path(const std::filesystem::path& path) const;

private:
    static std::filesystem::path
    get_base_path(const std::filesystem::path& base_path);

    void make_directory_impl(const std::filesystem::path& path);

    static void remove_all(const std::filesystem::path& path);

    std::filesystem::path base_path_;
    std::vector<std::filesystem::path> dirs_;
    std::vector<std::filesystem::path> files_;
};
