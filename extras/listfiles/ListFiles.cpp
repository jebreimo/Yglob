//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-04-26.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <Yglob/PathIterator.hpp>
#include <iostream>

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <path>\n";
        return 1;
    }

    Yglob::PathIterator it(argv[1]);
    while (it.next())
        std::cout << it.path() << '\n';

    return 0;
}
