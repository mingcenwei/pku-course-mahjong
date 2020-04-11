#ifndef TILE_1_IPP_20200409_
#define TILE_1_IPP_20200409_

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
} // namespace mahjong::private_detail_

#endif
