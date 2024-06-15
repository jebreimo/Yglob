//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-03-31.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <type_traits>

/**
 * @brief Defines bitwise operators for enum classes.
 */
namespace Yglob
{
    template <typename E>
    struct enable_bitmask_operators
    {
        static constexpr bool enable = false;
    };

    template <typename E>
    constexpr bool enable_bitmask_operators_v
        = enable_bitmask_operators<E>::enable;

    template <typename E>
    typename std::enable_if_t<enable_bitmask_operators_v<E>, E>
    operator&(E lhs, E rhs)
    {
        typedef typename std::underlying_type_t<E> underlying;
        return static_cast<E>(static_cast<underlying>(lhs)
                              & static_cast<underlying>(rhs));
    }

    template <typename E>
    typename std::enable_if_t<enable_bitmask_operators_v<E>, E>
    operator|(E lhs, E rhs)
    {
        typedef typename std::underlying_type_t<E> underlying;
        return static_cast<E>(static_cast<underlying>(lhs)
                              | static_cast<underlying>(rhs));
    }

    template <typename E>
    typename std::enable_if_t<enable_bitmask_operators_v<E>, E>
    operator^(E lhs, E rhs)
    {
        typedef typename std::underlying_type_t<E> underlying;
        return static_cast<E>(static_cast<underlying>(lhs)
                              ^ static_cast<underlying>(rhs));
    }

    template <typename E>
    typename std::enable_if_t<enable_bitmask_operators_v<E>, E&>
    operator&=(E& lhs, E rhs)
    {
        typedef typename std::underlying_type_t<E> underlying;
        lhs = static_cast<E>(static_cast<underlying>(lhs)
                             & static_cast<underlying>(rhs));
        return lhs;
    }

    template <typename E>
    typename std::enable_if_t<enable_bitmask_operators_v<E>, E&>
    operator|=(E& lhs, E rhs)
    {
        typedef typename std::underlying_type_t<E> underlying;
        lhs = static_cast<E>(static_cast<underlying>(lhs)
                             | static_cast<underlying>(rhs));
        return lhs;
    }

    template <typename E>
    typename std::enable_if_t<enable_bitmask_operators_v<E>, E&>
    operator^=(E& lhs, E rhs)
    {
        typedef typename std::underlying_type_t<E> underlying;
        lhs = static_cast<E>(static_cast<underlying>(lhs)
                             ^ static_cast<underlying>(rhs));
        return lhs;
    }
}

#define YGLOB_ENABLE_BITMASK_OPERATORS(E) \
    template <> \
    struct enable_bitmask_operators<E> \
    { \
        static constexpr bool enable = true; \
    }
