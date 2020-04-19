#ifndef TILE_1_IPP_20200409_
#define TILE_1_IPP_20200409_

#include "classes/TileTraits.hpp"

#include <array>
#include <cstddef>

namespace mahjong::private_detail_
{
    template <typename Tile, typename BaseType_>
    constexpr auto unsafeGetBaseTypeRepresentation(
        TileKind const kind, typename TileTraits<Tile>::IndexType const index)
        -> BaseType_
    {
        constexpr auto combineIndices {
            [](auto const tileKindIndex_, auto const index_) -> BaseType_ {
                return static_cast<BaseType_>(tileKindIndex_ * 10) +
                    static_cast<BaseType_>(index_);
            }};
        auto const tileKindIndex {tileKindToIndex(kind)};
        return combineIndices(tileKindIndex, index);
    }

    template <typename Tile, typename BaseType_>
    constexpr auto generateAllValidRepresentations()
    {
        using Traits = TileTraits<Tile>;
        using IndexType = typename Traits::IndexType;

        std::array<BaseType_, Traits::getNumOfDistinctTiles()>
            validRepresentations {};
        std::size_t arrayIndex {0};
        for (auto const kind: k_tileKindList)
        {
            for (IndexType index {1}; Traits::areValidArguments(kind, index);
                 ++index)
            {
                validRepresentations.at(arrayIndex) =
                    unsafeGetBaseTypeRepresentation<Tile, BaseType_>(
                        kind, index);
            }
        }
        return validRepresentations;
    }
} // namespace mahjong::private_detail_

#include "classes/Tile.hpp"

#endif
