#ifndef TILE_HPP_20200409_
#define TILE_HPP_20200409_

#include "utilities/debugging.hpp"
#include "utilities/metaprogramming.hpp"

#include <array>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

namespace mahjong::private_detail
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
} // namespace mahjong::private_detail

namespace mahjong
{
    enum class TileKind
    {
        Bing,
        Tiao,
        Wan,
        Feng,
        Jian,
        Hua,
    };
    inline constexpr std::array k_tileKindList {
        TileKind::Bing,
        TileKind::Tiao,
        TileKind::Wan,
        TileKind::Feng,
        TileKind::Jian,
        TileKind::Hua,
    };
    namespace private_detail
    {
        using OutputString = std::string;
        OutputString tileKindToString(TileKind const& kind)
        {
            switch (kind)
            {
            case TileKind::Bing:
                return {"饼"};
                break;
            case TileKind::Tiao:
                return {"条"};
                break;
            case TileKind::Wan:
                return {"万"};
                break;
            case TileKind::Feng:
                return {"风"};
                break;
            case TileKind::Jian:
                return {"箭"};
                break;
            case TileKind::Hua:
                return {"花"};
                break;
            default:
                utilities::unreachableCodeBlock();
                break;
            }
        }
    } // namespace private_detail

    template <typename Tile_, bool passByValue_ = true>
    struct TileTraits
    {
    public:
        using TileType = Tile_;
        using IndexType = typename TileType::IndexType;

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
            if constexpr (private_detail::hasMakeTileStaticMemberFunction_v<
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

    class Tile
    {
    public:
        using IndexType = std::int_fast8_t;

        constexpr Tile(TileKind const kind, IndexType const index) {}

    private:
        using BaseType_ = std::int_fast8_t;

        BaseType_ const representation;

        [[noreturn]] static void throwInvalidIndexException(
            TileKind const kind, IndexType const index)
        {
            throw std::invalid_argument {"invalid tile index"};
        }

        static constexpr BaseType_ getBaseTypeRepresentation(
            TileKind const kind, IndexType const index)
        {
            switch (kind)
            {
            case TileKind::Bing:
                if (index >= 1 && index <= 9)
                {
                }
                else
                {
                    throwInvalidIndexException();
                }
                break;
            case TileKind::Tiao:
                break;
            case TileKind::Wan:
                break;
            case TileKind::Feng:
                break;
            case TileKind::Jian:
                break;
            case TileKind::Hua:
                break;
            default:
                utilities::unreachableCodeBlock();
                break;
            }
        }
    };
} // namespace mahjong

namespace mahjong
{
    inline namespace literals
    {
        constexpr Tile operator""_b(unsigned long long const index)
        {
            using Index = TileTraits<Tile>::IndexType;
            constexpr auto kind {TileKind::Bing};
            return TileTraits<Tile>::makeTile(kind, static_cast<Index>(index));
        }
        constexpr Tile operator""_t(unsigned long long const index)
        {
            using Index = TileTraits<Tile>::IndexType;
            constexpr auto kind {TileKind::Tiao};
            return TileTraits<Tile>::makeTile(kind, static_cast<Index>(index));
        }
        constexpr Tile operator""_w(unsigned long long const index)
        {
            using Index = TileTraits<Tile>::IndexType;
            constexpr auto kind {TileKind::Wan};
            return TileTraits<Tile>::makeTile(kind, static_cast<Index>(index));
        }
        constexpr Tile operator""_f(unsigned long long const index)
        {
            using Index = TileTraits<Tile>::IndexType;
            constexpr auto kind {TileKind::Feng};
            return TileTraits<Tile>::makeTile(kind, static_cast<Index>(index));
        }
        constexpr Tile operator""_j(unsigned long long const index)
        {
            using Index = TileTraits<Tile>::IndexType;
            constexpr auto kind {TileKind::Jian};
            return TileTraits<Tile>::makeTile(kind, static_cast<Index>(index));
        }
        constexpr Tile operator""_h(unsigned long long const index)
        {
            using Index = TileTraits<Tile>::IndexType;
            constexpr auto kind {TileKind::Hua};
            return TileTraits<Tile>::makeTile(kind, static_cast<Index>(index));
        }
    } // namespace literals
} // namespace mahjong



#endif
