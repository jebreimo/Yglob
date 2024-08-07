//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-04-26.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <Yglob/PathIterator.hpp>

#include <iostream>
#include <Argos/Argos.hpp>

const char* to_char(const std::u8string& str)
{
    return reinterpret_cast<const char*>(str.data());
}

argos::ParsedArguments parse_arguments(int argc, char* argv[])
{
    using namespace argos;
    return ArgumentParser()
        .add(Arg("path").help("The path to list files in."))
        .add(Opt{"-a", "--absolute"}
            .help("List files with absolute paths."))
        .add(Opt{"-i", "--ignore-case"}
            .constant("ignore")
            .help("Ignore case when comparing file names."))
        .add(Opt{"-c", "--case-sensitive"}
            .alias("--ignore-case").constant("enforce")
            .help("Enforce case when comparing file names."))
        .add(Opt{"-f", "--files"}.alias("--no-files").constant(false)
            .help("Include files in the listing. (default: true)"))
        .add(Opt{"--no-files"}
            .help("Exclude files in the listing. (default: false)"))
        .add(Opt{"-d", "--directories"}.alias("--no-directories").constant(false)
            .help("Include directories in the listing. (default: true)"))
        .add(Opt{"--no-dirs"}
            .help("Exclude directories in the listing. (default: false)"))
        .parse(argc, argv);
}

int main(int argc, char* argv[])
{
    try
    {
        auto args = parse_arguments(argc, argv);

        auto flags = Yglob::PathIteratorFlags::DEFAULT;
        if (args.value("--ignore-case").as_string() == "ignore")
            flags |= Yglob::PathIteratorFlags::CASE_INSENSITIVE_PATHS;
        else if (args.value("--ignore-case").as_string() == "enforce")
            flags |= Yglob::PathIteratorFlags::CASE_SENSITIVE_GLOBS;

        if (args.value("--no-files").as_bool())
            flags |= Yglob::PathIteratorFlags::NO_FILES;
        if (args.value("--no-dirs").as_bool())
            flags |= Yglob::PathIteratorFlags::NO_DIRECTORIES;

        auto use_absolute = args.value("--absolute").as_bool();

        for (auto& path: Yglob::PathIterator(args.value("path").as_string(), flags))
        {
            if (use_absolute)
                std::cout << to_char(absolute(path.lexically_normal()).u8string()) << '\n';
            else
                std::cout << to_char(path.lexically_normal().u8string()) << '\n';
        }
    }
    catch (std::exception& ex)
    {
        std::cerr << ex.what() << '\n';
        return 1;
    }
    return 0;
}
