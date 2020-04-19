#ifndef TILE_TRAITS_1_IPP_20200414_
#define TILE_TRAITS_1_IPP_20200414_

#include <cstddef>
#include <type_traits>

namespace mahjong::private_detail_
{
    template <typename T, typename Given, typename = void>
    struct IndexTypeMemberTypeOrGiveType
    {
        using Type = Given;
    };
    template <typename T, typename Given>
    struct IndexTypeMemberTypeOrGiveType<
        T,
        Given,
        std::void_t<typename T::IndexType>>
    {
        using Type = typename T::IndexType;
    };
    template <typename T, typename Given>
    using IndexTypeMemberTypeOrGiveType_t =
        typename IndexTypeMemberTypeOrGiveType<T, Given>::Type;

    template <typename Array>
    constexpr typename Array::value_type sumArray(Array const& array)
    {
        typename Array::value_type sum {};
        for (auto const& value: array)
        {
            sum += value;
        }
        return sum;
    }
} // namespace mahjong::private_detail_

#include "classes/TileTraits.hpp"

#endif
