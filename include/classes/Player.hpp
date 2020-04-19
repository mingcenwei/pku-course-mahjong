#ifndef PLAYER_HPP_20200412_
#define PLAYER_HPP_20200412_

#include "classes/Hand.hpp"
#include "classes/Tile.hpp"
#include "utilities/debugging.hpp"

#include <cstddef>
#include <string_view>
#include <tuple>
#include <utility>
#include <vector>

namespace mahjong
{
    class Player
    {
    public:
        using Fan = std::size_t;
        using FanTable =
            std::vector<std::tuple<std::string_view, Fan, std::size_t>>;

        Player() = delete;
        Player(Wind const menfeng) noexcept : menfeng_ {menfeng} {}

        Player(Wind const menfeng, Hand const& hand)
            : menfeng_ {menfeng}, hand_ {hand}
        {}
        Player(Wind const menfeng, Hand&& hand)
            : menfeng_ {menfeng}, hand_ {std::move(hand)}
        {}

        Fan calculateFan(
            Tile winningTile,
            Wind quanfeng,
            bool isZimo = false,
            bool isJuezhang = false,
            bool isGang = false,
            bool isLastTile = false,
            bool isQishou = false) const noexcept(!utilities::k_isDebugging);

        FanTable calculateFanTable(
            Tile winningTile,
            Wind quanfeng,
            bool isZimo = false,
            bool isJuezhang = false,
            bool isGang = false,
            bool isLastTile = false,
            bool isQishou = false) const;

        static void printFanTable(
            FanTable const& fantable, bool onlyPrintExisting = true);

    private:
        friend class Rival;

        template <typename Flag>
        using FlagContainer_ = std::vector<Flag>;

        using Hand_ = Hand;
        using Hua_ = Hand_;
        using Chi_ = Hand_;
        using Peng_ = Hand_;
        using Gang_ = Hand_;

        Wind menfeng_ {};
        Hand_ hand_ {};
        Hua_ hua_ {};
        Chi_ firstTileOfChi_ {};
        FlagContainer_<std::size_t> chiIndices_ {};
        Peng_ peng_ {};
        FlagContainer_<Wind> pengFrom_ {};
        Gang_ gang_ {};
        FlagContainer_<Wind> gangFrom_ {};
        FlagContainer_<bool> isJiagang_ {};
    };

    class Rival
    {
    public:
        Rival() = delete;
        // Rival(Wind const menfeng) : menfeng_ {menfeng} {}

    private:
        friend class Player;

        template <typename Flag>
        using FlagContainer_ = Player::FlagContainer_<Flag>;

        using Hand_ = Player::Hand_;
        using Hua_ = Player::Hua_;
        using Chi_ = Player::Chi_;
        using Peng_ = Player::Peng_;
        using Gang_ = Player::Gang_;

        // Wind menfeng_ {};
        // Hua_ hua_ {};
        // Chi_ firstTileOfChi_ {};
        // FlagContainer_<std::size_t> chiIndices_ {};
        // Peng_ peng_ {};
        // FlagContainer_<Wind> pengFrom_ {};
        // Gang_ gang_ {};
        // FlagContainer_<Wind> gangFrom_ {};
        // FlagContainer_<bool> isJiagang_ {};
    };
} // namespace mahjong

#endif
