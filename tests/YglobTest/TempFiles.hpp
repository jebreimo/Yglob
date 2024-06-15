//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-06-12.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <filesystem>
#include <fstream>
#include <iostream>
#include <ranges>
#include <vector>

class TempFiles
{
public:
    TempFiles(std::initializer_list<std::filesystem::path> sub_paths)
    {
        tmp_path = canonical(std::filesystem::temp_directory_path());
        for (auto& sub_path: sub_paths)
            make_file(tmp_path / sub_path);
    }

    ~TempFiles()
    {
        try
        {
            for (auto& path: files_)
                std::filesystem::remove(path);
            for (auto& path: std::ranges::reverse_view(dirs_))
                std::filesystem::remove(path);
        }
        catch (std::filesystem::filesystem_error& ex)
        {
            std::cerr << "Failed to remove temporary files: " << ex.what() << std::endl;
        }
    }

    [[nodiscard]]
    std::filesystem::path base_directory() const
    {
        return tmp_path;
    }

    [[nodiscard]]
    std::vector<std::filesystem::path> files() const
    {
        return files_;
    }

    [[nodiscard]]
    std::filesystem::path get_path(const std::filesystem::path& path) const
    {
        return tmp_path / path;
    }

private:
    void create_dirs(std::filesystem::path path)
    {
        std::vector<std::filesystem::path> dirs_to_create;
        while (path != path.root_path() && !std::filesystem::exists(path))
        {
            dirs_to_create.push_back(path);
            path = path.parent_path();
        }
        for (auto& it: std::ranges::reverse_view(dirs_to_create))
        {
            std::filesystem::create_directory(it);
            dirs_.push_back(it);
        }
    }

    void make_file(const std::filesystem::path& path)
    {
        if (std::filesystem::exists(path))
            return;

        create_dirs(path.parent_path());
        std::ofstream(path) << "Hello, world!";
        files_.push_back(path);
    }

    std::filesystem::path tmp_path;
    std::vector<std::filesystem::path> dirs_;
    std::vector<std::filesystem::path> files_;
};
