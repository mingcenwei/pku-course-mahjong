#ifndef TILE_HPP_20200409_
#define TILE_HPP_20200409_

#include "classes/Tile-1.ipp"
#include "utilities/metaprogramming.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace mahjong
{
    enum class TileKind
    {
        bing,
        tiao,
        wan,
        feng,
        jian,
        hua,
    };
    inline constexpr std::array k_tileKindList {
        TileKind::bing,
        TileKind::tiao,
        TileKind::wan,
        TileKind::feng,
        TileKind::jian,
        TileKind::hua,
    };
    constexpr std::size_t tileKindToIndex(TileKind const kind) noexcept
    {
        return static_cast<std::size_t>(kind);
    }
    constexpr TileKind unsafeIndexToTileKind(
        std::size_t const index) noexcept
    {
        return static_cast<TileKind>(index);
    }
    constexpr TileKind indexToTileKind(std::size_t const index);
    constexpr TileKind unsafeCharToTileKind(char const ch) noexcept;

    template <typename Tile_, bool passByValue_ = true>
    struct TileTraits
    {
    public:
        using TileType = Tile_;
        using IndexType = private_detail_::
            IndexTypeMemberTypeOrGiveType_t<TileType, std::int_fast8_t>;

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
        [[nodiscard]] static constexpr TileType makeTile(
            TileKind const kind, IndexType const index);

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
            ConstTileOrConstTileRef tile2) noexcept;
    };

    class Tile
    {
    public:
        using IndexType = std::int_fast8_t;

        static constexpr bool areValidArguments(
            TileKind const kind, IndexType const index) noexcept;

        constexpr Tile(TileKind const kind, IndexType const index);
        constexpr TileKind getKind() noexcept;
        constexpr IndexType getIndex() noexcept
        {
            return static_cast<IndexType>(representation_ % 10);
        }

#if __cplusplus >= 202002L
        friend constexpr auto operator<=>(
            Tile const tile1, Tile const tile2) noexcept = default;
#else
        friend constexpr bool operator==(
            Tile const tile1, Tile const tile2) noexcept
        {
            return tile1.representation_ == tile2.representation_;
        }
        friend constexpr bool operator!=(
            Tile const tile1, Tile const tile2) noexcept
        {
            return tile1.representation_ != tile2.representation_;
        }
        friend constexpr bool operator<(
            Tile const tile1, Tile const tile2) noexcept
        {
            return tile1.representation_ < tile2.representation_;
        }
        friend constexpr bool operator<=(
            Tile const tile1, Tile const tile2) noexcept
        {
            return tile1.representation_ <= tile2.representation_;
        }
        friend constexpr bool operator>(
            Tile const tile1, Tile const tile2) noexcept
        {
            return tile1.representation_ > tile2.representation_;
        }
        friend constexpr bool operator>=(
            Tile const tile1, Tile const tile2) noexcept
        {
            return tile1.representation_ >= tile2.representation_;
        }
#endif

    private:
        using BaseType_ = std::int_fast8_t;

        BaseType_ const representation_;

        [[noreturn]] static void throwInvalidIndexException(
            TileKind const kind, IndexType const index);
        static constexpr BaseType_ getBaseTypeRepresentation(
            TileKind const kind, IndexType const index);
    };
} // namespace mahjong

namespace mahjong
{
    inline namespace literals
    {
        constexpr Tile operator""_b(unsigned long long const index);
        constexpr Tile operator""_t(unsigned long long const index);
        constexpr Tile operator""_w(unsigned long long const index);
        constexpr Tile operator""_f(unsigned long long const index);
        constexpr Tile operator""_j(unsigned long long const index);
        constexpr Tile operator""_h(unsigned long long const index);
    } // namespace literals
} // namespace mahjong

#include "classes/Tile-2.ipp"

#endif
