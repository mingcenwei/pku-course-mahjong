#ifndef TILE_TRAITS_HPP_20200414_
#define TILE_TRAITS_HPP_20200414_

#include "classes/TileTraits-1.ipp"
#include "utilities/metaprogramming.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <string_view>

namespace mahjong
{
    enum class Wind
    {
        east,
        south,
        west,
        north,
    };
    inline constexpr std::array k_windList {
        Wind::east,
        Wind::south,
        Wind::west,
        Wind::north,
    };
    constexpr std::size_t windToIndex(Wind const wind) noexcept
    {
        return static_cast<std::size_t>(wind);
    }
    constexpr Wind unsafeIndexToWind(std::size_t const index) noexcept
    {
        return static_cast<Wind>(index);
    }
    constexpr Wind indexToWind(std::size_t const index);
    constexpr Wind nextWind(
        Wind const wind, std::ptrdiff_t const step = 1) noexcept;
    constexpr std::ptrdiff_t windDistance(Wind const wind1, Wind const wind2) noexcept;

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
    inline constexpr std::string_view k_tileKindSymbols {"btwfjh"};
    static_assert(
        k_tileKindList.size() == k_tileKindSymbols.size(),
        "The sizes of k_tileKindList and k_tileKindSymbols don't match");
    inline constexpr std::array<std::size_t, k_tileKindList.size()>
        k_numsOfDistinctTiles {9, 9, 9, 4, 3, 8};
    static_assert(
        k_numsOfDistinctTiles.back() !=
            decltype(k_numsOfDistinctTiles)::value_type {},
        "The sizes of k_tileKindList and k_numsOfDistinctTiles don't match");
    constexpr bool isValidTileKindSymbol(char const ch) noexcept
    {
        return k_tileKindSymbols.find(ch) != decltype(k_tileKindSymbols)::npos;
    }
    constexpr std::size_t tileKindToIndex(TileKind const kind) noexcept
    {
        return static_cast<std::size_t>(kind);
    }
    constexpr TileKind unsafeIndexToTileKind(std::size_t const index) noexcept
    {
        return static_cast<TileKind>(index);
    }
    constexpr TileKind indexToTileKind(std::size_t const index);
    constexpr char tileKindToChar(TileKind const kind) noexcept
    {
        return k_tileKindSymbols[tileKindToIndex(kind)];
    }
    constexpr TileKind unsafeCharToTileKind(char const ch) noexcept
    {
        return k_tileKindList[k_tileKindSymbols.find(ch)];
    }

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
        static constexpr std::size_t getNumOfDistinctTiles() noexcept
        {
            return numOfDistinctTiles_;
        }
        static constexpr bool areValidArguments(
            TileKind const kind, IndexType const index) noexcept
        {
            return index <= static_cast<IndexType>(
                                k_numsOfDistinctTiles[tileKindToIndex(kind)]) &&
                index > 0;
        }

    private:
        static constexpr std::size_t numOfDistinctTiles_ {
            private_detail_::sumArray(k_numsOfDistinctTiles)};
    };
} // namespace mahjong

#include "classes/TileTraits-2.ipp"

#endif
