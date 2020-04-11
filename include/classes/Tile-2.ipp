#ifndef TILE_2_IPP_20200409_
#define TILE_2_IPP_20200409_

#include "classes/Tile.hpp"
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
        if (index <= std::size(k_tileKindList))
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

    constexpr TileKind unsafeCharToTileKind(char const ch) noexcept
    {
        switch (ch)
        {
        case 'b':
            return TileKind::bing;
            break;
        case 't':
            return TileKind::tiao;
            break;
        case 'w':
            return TileKind::wan;
            break;
        case 'f':
            return TileKind::feng;
            break;
        case 'j':
            return TileKind::jian;
            break;
        case 'h':
            return TileKind::hua;
            break;
        default:
            utilities::unreachableCodeBlock();
            break;
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

    constexpr TileKind Tile::getKind() noexcept
    {
        auto const tileKindIndex {
            static_cast<std::size_t>(representation_ / 10)};
        return unsafeIndexToTileKind(tileKindIndex);
    }

    constexpr bool Tile::areValidArguments(
        TileKind const kind, IndexType const index) noexcept
    {
        switch (kind)
        {
        case TileKind::bing:
            if (index < 1 || index > 9)
            {
                return false;
            }
            break;
        case TileKind::tiao:
            if (index < 1 || index > 9)
            {
                return false;
            }
            break;
        case TileKind::wan:
            if (index < 1 || index > 9)
            {
                return false;
            }
            break;
        case TileKind::feng:
            if (index < 1 || index > 4)
            {
                return false;
            }
            break;
        case TileKind::jian:
            if (index < 1 || index > 3)
            {
                return false;
            }
            break;
        case TileKind::hua:
            if (index < 1 || index > 8)
            {
                return false;
            }
            break;
        default:
            utilities::unreachableCodeBlock();
            break;
        }
        return true;
    }

    constexpr auto Tile::getBaseTypeRepresentation(
        TileKind const kind, IndexType const index) -> BaseType_
    {
        if (areValidArguments(kind, index))
        {
            constexpr auto combineIndices {
                [](auto const tileKindIndex_, auto const index_) -> BaseType_ {
                    return static_cast<BaseType_>(tileKindIndex_ * 10) +
                        static_cast<BaseType_>(index_);
                }};
            auto const tileKindIndex {tileKindToIndex(kind)};
            return combineIndices(tileKindIndex, index);
        }
        else
        {
            throwInvalidIndexException(kind, index);
        }
    }

    constexpr Tile::Tile(TileKind const kind, IndexType const index)
        : representation_ {getBaseTypeRepresentation(kind, index)}
    {}
} // namespace mahjong

namespace mahjong
{
    inline namespace literals
    {
        constexpr Tile operator""_b(unsigned long long const index)
        {
            using Index = TileTraits<Tile>::IndexType;
            constexpr auto kind {TileKind::bing};
            return TileTraits<Tile>::makeTile(kind, static_cast<Index>(index));
        }
        constexpr Tile operator""_t(unsigned long long const index)
        {
            using Index = TileTraits<Tile>::IndexType;
            constexpr auto kind {TileKind::tiao};
            return TileTraits<Tile>::makeTile(kind, static_cast<Index>(index));
        }
        constexpr Tile operator""_w(unsigned long long const index)
        {
            using Index = TileTraits<Tile>::IndexType;
            constexpr auto kind {TileKind::wan};
            return TileTraits<Tile>::makeTile(kind, static_cast<Index>(index));
        }
        constexpr Tile operator""_f(unsigned long long const index)
        {
            using Index = TileTraits<Tile>::IndexType;
            constexpr auto kind {TileKind::feng};
            return TileTraits<Tile>::makeTile(kind, static_cast<Index>(index));
        }
        constexpr Tile operator""_j(unsigned long long const index)
        {
            using Index = TileTraits<Tile>::IndexType;
            constexpr auto kind {TileKind::jian};
            return TileTraits<Tile>::makeTile(kind, static_cast<Index>(index));
        }
        constexpr Tile operator""_h(unsigned long long const index)
        {
            using Index = TileTraits<Tile>::IndexType;
            constexpr auto kind {TileKind::hua};
            return TileTraits<Tile>::makeTile(kind, static_cast<Index>(index));
        }
    } // namespace literals
} // namespace mahjong

#endif
