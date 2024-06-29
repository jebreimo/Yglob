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

argos::ParsedArguments parse_arguments(int argc, char* argv[])
{
    using namespace argos;
    return ArgumentParser()
        .add(Arg("path").help("The path to list files in."))
        .add(Opt{"-i", "--ignore-case"}
            .constant("ignore")
            .help("Ignore case when comparing file names."))
        .add(Opt{"-c", "--case-sensitive"}
            .alias("-i").constant("enforce")
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
    auto args = parse_arguments(argc, argv);

    auto flags = Yglob::PathIteratorFlags::DEFAULT;
    if (args.value("-i").as_string() == "ignore")
        flags |= Yglob::PathIteratorFlags::CASE_INSENSITIVE_PATHS;
    else if (args.value("-i").as_string() == "enforce")
        flags |= Yglob::PathIteratorFlags::CASE_SENSITIVE_GLOBS;

    if (args.value("--no-files").as_bool())
        flags |= Yglob::PathIteratorFlags::NO_FILES;
    if (args.value("--no-dirs").as_bool())
        flags |= Yglob::PathIteratorFlags::NO_DIRECTORIES;

    for (auto& path : Yglob::PathIterator(args.value("path").as_string(), flags))
        std::cout << path << '\n';

    return 0;
}
