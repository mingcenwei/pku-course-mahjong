#ifndef TILE_HPP_20200409_
#define TILE_HPP_20200409_

#include "classes/Tile-1.ipp"
#include "classes/TileTraits.hpp"
#include "utilities/debugging.hpp"

#include <cstdint>

namespace mahjong
{
    class Tile
    {
    public:
        using IndexType = std::int_fast8_t;

        constexpr Tile(TileKind const kind, IndexType const index);
        constexpr TileKind getKind() const noexcept;
        constexpr IndexType getIndex() const noexcept
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
        using Traits_ = TileTraits<Tile>;
        using BaseType_ = std::int_fast8_t;

        BaseType_ representation_;

        static constexpr auto validRepresentations_ {
            private_detail_::
                generateAllValidRepresentations<Tile, BaseType_>()};

        constexpr Tile(BaseType_ const representation) noexcept(
            !utilities::k_isDebugging);

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
