//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-06-17.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "TempFiles.hpp"
#include <fstream>
#include <iostream>
#include <ranges>
#include <thread>

TempFiles::TempFiles(const std::filesystem::path& base_path, bool erase)
    : base_path_(get_base_path(base_path))
{
    if (erase && std::filesystem::exists(base_path_))
        remove_all(base_path_);
    if (!std::filesystem::exists(base_path_))
    {
        std::filesystem::create_directories(base_path_);
        dirs_.push_back(base_path_);
    }
    base_path_ = std::filesystem::canonical(base_path_);
}

TempFiles::~TempFiles()
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

TempFiles& TempFiles::make_file(const std::filesystem::path& path)
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

TempFiles& TempFiles::make_files(const std::vector<std::filesystem::path>& paths)
{
    for (auto& path: paths)
        make_file(path);
    return *this;
}

TempFiles& TempFiles::make_directory(const std::filesystem::path& path)
{
    make_directory_impl(base_path_ / path);
    return *this;
}

std::filesystem::path TempFiles::base_directory() const
{
    return base_path_;
}

std::vector<std::filesystem::path> TempFiles::files() const
{
    return files_;
}

std::vector<std::filesystem::path> TempFiles::directories() const
{
    return dirs_;
}

std::filesystem::path TempFiles::get_path(const std::filesystem::path& path) const
{
    return base_path_ / path;
}

std::filesystem::path TempFiles::get_base_path(const std::filesystem::path& base_path)
{
    std::filesystem::path path;
    if (base_path.is_absolute())
        return base_path;
    return std::filesystem::temp_directory_path() / base_path;
}

void TempFiles::make_directory_impl(const std::filesystem::path& path)
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

void TempFiles::remove_all(const std::filesystem::path& path)
{
    using namespace std::chrono_literals;
    std::unique_ptr<std::filesystem::filesystem_error> error;

    auto end_time = std::chrono::system_clock::now() + 1s;
    do
    {
        try
        {
            std::filesystem::remove_all(path);
            return;
        }
        catch (std::filesystem::filesystem_error& ex)
        {
            if (!error)
                error = std::make_unique<std::filesystem::filesystem_error>(ex);

            std::this_thread::sleep_for(50ms);
        }
    } while (std::chrono::system_clock::now() < end_time);

    throw std::filesystem::filesystem_error(*error);
}
