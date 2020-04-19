#ifndef TILE_2_IPP_20200409_
#define TILE_2_IPP_20200409_

#include "classes/Tile.hpp"

#include <cstddef>
#include <stdexcept>
#include <string>

namespace mahjong
{
    constexpr TileKind Tile::getKind() const noexcept
    {
        auto const tileKindIndex {
            static_cast<std::size_t>(representation_ / 10)};
        return unsafeIndexToTileKind(tileKindIndex);
    }

    constexpr auto Tile::getBaseTypeRepresentation(
        TileKind const kind, IndexType const index) -> BaseType_
    {
        using Traits = TileTraits<Tile>;
        if (Traits::areValidArguments(kind, index))
        {
            return private_detail_::
                unsafeGetBaseTypeRepresentation<Tile, BaseType_>(kind, index);
        }
        else
        {
            throwInvalidIndexException(kind, index);
        }
    }

    constexpr Tile::Tile(TileKind const kind, IndexType const index)
        : representation_ {getBaseTypeRepresentation(kind, index)}
    {}

    constexpr Tile::Tile(BaseType_ const representation) noexcept(
        !utilities::k_isDebugging)
        : representation_ {representation}
    {
        if constexpr (utilities::k_isDebugging)
        {
            for (auto const& validRepresentation: validRepresentations_)
            {
                if (representation_ == validRepresentation)
                {
                    return;
                }
            }
            {
                using namespace std::string_literals;
                throw std::invalid_argument {
                    "invalid tile base representation: "s +
                    std::to_string(representation)};
            }
        }
    }
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
