#ifndef MAHJONG_HPP_20200409_
#define MAHJONG_HPP_20200409_

namespace name::private_detail
{}

namespace name
{
    template <typename Mahjong_>
    struct MahjongTraits
    {
        static constexpr bool isTong(Mahjong_ const&) noexcept;
        static constexpr bool isTiao(Mahjong_ const&) noexcept;
        static constexpr bool isWan(Mahjong_ const&) noexcept;
        static constexpr bool isFeng(Mahjong_ const&) noexcept;
        static constexpr bool isJian(Mahjong_ const&) noexcept;
        static constexpr bool isHua(Mahjong_ const&) noexcept;
    };
} // namespace name

#endif
