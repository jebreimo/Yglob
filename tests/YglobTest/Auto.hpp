//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-06-17.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

// Copied from https://quuxplusone.github.io/blog/2018/08/11/the-auto-macro/

template<class L>
class AtScopeExit {
    L& m_lambda;
public:
    explicit AtScopeExit(L& action) : m_lambda(action) {}
    ~AtScopeExit() noexcept(false) { m_lambda(); }
};

#define TOKEN_PASTEx(x, y) x ## y
#define TOKEN_PASTE(x, y) TOKEN_PASTEx(x, y)

#define Auto_INTERNAL1(lname, aname, ...) \
    auto lname = [&]() { __VA_ARGS__; }; \
    AtScopeExit<decltype(lname)> aname(lname);

#define Auto_INTERNAL2(ctr, ...) \
    Auto_INTERNAL1(TOKEN_PASTE(Auto_func_, ctr), \
        TOKEN_PASTE(Auto_instance_, ctr), __VA_ARGS__)

#define Auto(...) \
    Auto_INTERNAL2(__COUNTER__, __VA_ARGS__)
