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
    explicit TempFiles(const std::filesystem::path& base_path,
                       bool erase = false)
        : base_path_(get_base_path(base_path))
    {
        if (erase && std::filesystem::exists(base_path_))
            std::filesystem::remove_all(base_path_);
        if (!std::filesystem::exists(base_path_))
        {
            std::filesystem::create_directories(base_path_);
            dirs_.push_back(base_path_);
        }
        base_path_ = std::filesystem::canonical(base_path_);
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

    TempFiles& make_file(const std::filesystem::path& path)
    {
        auto real_path = base_path_ / path;
        if (std::filesystem::exists(real_path))
            return *this;

        make_directory_impl(real_path.parent_path());
        auto contents = path.u8string();
        std::ofstream(real_path).write(
            reinterpret_cast<const char*>(contents.data()),
            std::streamsize(contents.size()));
        files_.push_back(real_path);
        return *this;
    }

    TempFiles& make_files(const std::vector<std::filesystem::path>& paths)
    {
        for (auto& path: paths)
            make_file(path);
        return *this;
    }

    TempFiles& make_directory(const std::filesystem::path& path)
    {
        make_directory_impl(base_path_ / path);
        return *this;
    }

    [[nodiscard]]
    std::filesystem::path base_directory() const
    {
        return base_path_;
    }

    [[nodiscard]]
    std::vector<std::filesystem::path> files() const
    {
        return files_;
    }

    [[nodiscard]]
    std::vector<std::filesystem::path> directories() const
    {
        return dirs_;
    }

    [[nodiscard]]
    std::filesystem::path get_path(const std::filesystem::path& path) const
    {
        return base_path_ / path;
    }

private:
    static std::filesystem::path
    get_base_path(const std::filesystem::path& base_path)
    {
        std::filesystem::path path;
        if (base_path.is_absolute())
            return base_path;
        return std::filesystem::temp_directory_path() / base_path;
    }

    void make_directory_impl(const std::filesystem::path& path)
    {
        std::vector<std::filesystem::path> dirs;

        auto tmp = path;
        while (!std::filesystem::exists(tmp))
        {
            dirs.push_back(tmp);
            tmp = tmp.parent_path();
        }

        dirs_.insert(dirs_.end(), dirs.rbegin(), dirs.rend());

        std::filesystem::create_directories(path);
    }

    std::filesystem::path base_path_;
    std::vector<std::filesystem::path> dirs_;
    std::vector<std::filesystem::path> files_;
};
