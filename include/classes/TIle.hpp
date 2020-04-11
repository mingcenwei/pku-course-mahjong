#ifndef TILE_HPP_20200409_
#define TILE_HPP_20200409_

#include "utilities/metaprogramming.hpp"

#include <array>
#include <cstdint>

namespace name::private_detail
{}

namespace name
{
    enum class TileKind
    {
        Tong,
        Tiao,
        Wan,
        Feng,
        Jian,
        Hua,
    };
    inline constexpr std::array k_tileKindList {
        TileKind::Tong,
        TileKind::Tiao,
        TileKind::Wan,
        TileKind::Feng,
        TileKind::Jian,
        TileKind::Hua,
    };

    template <typename Tile_, bool passByValue_ = true>
    struct TileTraits
    {
    public:
        using TileType = Tile_;
        using IndexType = std::int_fast8_t;

    private:
        using TileOrTileRef = utilities::LvalueRefIf_t<TileType, !passByValue_>;
        using ConstTileOrConstTileRef =
            utilities::LvalueRefIf_t<TileType const, !passByValue_>;

    public:
        constexpr TileTraits() noexcept = delete;

        static constexpr bool passByValue() noexcept { return passByValue_; }

        static constexpr TileKind getKind(ConstTileOrConstTileRef tile) noexcept
        {
            return tile.getKind();
        }
        static constexpr IndexType getIndex(
            ConstTileOrConstTileRef tile) noexcept
        {
            return tile.getIndex();
        }
        static constexpr TileType makeTile(
            TileKind const kind, IndexType const index)
        {
            return TileType::makeTile(kind, index);
        }

        static constexpr bool isOfKind(
            ConstTileOrConstTileRef tile, TileKind const kind) noexcept
        {
            return getKind(tile) == kind;
        }
        static constexpr bool hasIndex(
            ConstTileOrConstTileRef tile, IndexType const index) noexcept
        {
            return getIndex(tile) == index;
        }
        static constexpr bool sameKind(
            ConstTileOrConstTileRef tile1,
            ConstTileOrConstTileRef tile2) noexcept
        {
            return getKind(tile1) == getKind(tile2);
        }
        static constexpr bool sameIndex(
            ConstTileOrConstTileRef tile1,
            ConstTileOrConstTileRef tile2) noexcept
        {
            return getIndex(tile1) == getIndex(tile2);
        }
        static constexpr bool isSame(
            ConstTileOrConstTileRef tile1,
            ConstTileOrConstTileRef tile2) noexcept
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
    };
} // namespace name

#endif
