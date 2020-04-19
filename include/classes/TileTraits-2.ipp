#ifndef TILE_TRAITS_2_IPP_20200414_
#define TILE_TRAITS_2_IPP_20200414_


#include "classes/TileTraits.hpp"
#include "utilities/debugging.hpp"
#include "utilities/metaprogramming.hpp"

#include <cstddef>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

namespace mahjong::private_detail_
{
    template <
        typename Tile_,
        typename TileKind_,
        typename TileIndex_,
        typename = Tile_>
    struct HasMakeTileStaticMemberFunction : std::false_type
    {};
    template <typename Tile_, typename TileKind_, typename TileIndex_>
    struct HasMakeTileStaticMemberFunction<
        Tile_,
        TileKind_,
        TileIndex_,
        decltype(Tile_::makeTile(
            std::declval<TileKind_>(), std::declval<TileIndex_>()))>
        : std::true_type
    {};
    template <typename Tile_, typename TileKind_, typename TileIndex_>
    inline constexpr bool hasMakeTileStaticMemberFunction_v {
        HasMakeTileStaticMemberFunction<Tile_, TileKind_, TileIndex_>::value};
} // namespace mahjong::private_detail_

namespace mahjong
{
    constexpr TileKind indexToTileKind(std::size_t const index)
    {
        if (index < std::size(k_tileKindList))
        {
            return unsafeIndexToTileKind(index);
        }
        else
        {
            using namespace std::string_literals;
            throw std::out_of_range {"invalid tile kind index: "s +
                                     std::to_string(index)};
        }
    }

    template <typename Tile_, bool passByValue_>
    [[nodiscard]] constexpr auto TileTraits<Tile_, passByValue_>::makeTile(
        TileKind const kind, IndexType const index) -> TileType
    {
        if constexpr (private_detail_::hasMakeTileStaticMemberFunction_v<
                          TileType,
                          TileKind,
                          IndexType>)
        {
            return TileType::makeTile(kind, index);
        }
        else
        {
            return TileType {kind, index};
        }
    }

    template <typename Tile_, bool passByValue_>
    constexpr bool TileTraits<Tile_, passByValue_>::isSame(
        ConstTileOrConstTileRef tile1, ConstTileOrConstTileRef tile2) noexcept
    {
        if constexpr (utilities::equalityDefined_v<TileType>)
        {
            static_assert(
                noexcept(tile1 == tile2),
                "Comparing equality may throw exceptions");
            return tile1 == tile2;
        }
        else
        {
            return sameKind(tile1, tile2) && sameIndex(tile1, tile2);
        }
    }
} // namespace mahjong

#endif
