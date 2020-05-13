#include "classes/Hand.hpp"
#include "classes/Player.hpp"
#include "classes/Tile.hpp"
#include "utilities/debugging.hpp"
#include "utilities/metaprogramming.hpp"

#include <algorithm>
#include <array>
#include <charconv>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <list>
#include <map>
#include <numeric>
#include <queue>
#include <regex>
#include <set>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

namespace
{
    int main_impl(int const argc, char const* const* const argv)
    {
        static_cast<void>(argc);
        static_cast<void>(argv);

        using namespace mahjong;
        using namespace utilities;

        // mahjong::Tile const tile1 {mahjong::TileKind::wan, 2};
        // auto const tile2 {2_w};
        // auto const tile3 {tile2};
        // mahjong::Tile tile4 = tile3;
        // auto tile5 = 8_h;
        // mahjong::Tile tile6 {std::move(tile3)};

        // debugPrintLine(tile1 == tile3);
        // debugPrintLine(tile2 != tile4);

        // auto hand {"213w333f4h5h999b3f4f3w"_H};
        // debugPrintLine(hand);

        auto hand {"1129345167899w"_H};
        constexpr auto winningTile {5_w};
        constexpr auto menfeng {Wind::south};
        constexpr auto quanfeng {Wind::west};


        constexpr bool isZimo {false};
        constexpr bool isJuezhang {false};
        constexpr bool isGang {false};
        constexpr bool isLastTile {false};
        constexpr bool isQishou {true};

        Player player {menfeng, hand};
        debugPrintLine(
            player.calculateFan(
                winningTile,
                quanfeng,
                isZimo,
                isJuezhang,
                isGang,
                isLastTile,
                isQishou),
            "\n");

        debugPrintLine(hand.toString());
        auto fanTable {player.calculateFanTable(
            winningTile,
            quanfeng,
            isZimo,
            isJuezhang,
            isGang,
            isLastTile,
            isQishou)};
        Player::printFanTable(fanTable);
        debugPrintLine(player.serialize(","));

        hand.sort();
        debugPrintLine(hand.toString());
        player.setHand(hand);

        fanTable = {player.calculateFanTable(
            winningTile,
            quanfeng,
            isZimo,
            isJuezhang,
            isGang,
            isLastTile,
            isQishou)};
        Player::printFanTable(fanTable);
        debugPrintLine(player.serialize("|"));


        std::istringstream iSStream {"come_on!!_Here_we_go!"};
        mahjong::io::ignoreUntil(iSStream, {"e_"}, 3);
        // mahjong::io::ignoreUntil(iSStream, {"_"});
        std::string buffer;
        iSStream >> buffer;
        debugPrintLine("\n", buffer);

        debugPrintLine("\nFinished.\n");
        return 0;
    }
} // namespace

int main(int const argc, char const* const* const argv)
{
    try
    {
        return main_impl(argc, argv);
    }
    catch (utilities::ExitNormally_t const& signal)
    {
        return signal.returnCode;
    }
    catch (utilities::ExitAbnormally_t const& signal)
    {
        return signal.returnCode;
    }
}
