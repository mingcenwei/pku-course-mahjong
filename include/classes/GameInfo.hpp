#ifndef GAMEINFO_HPP_20190420_
#define GAMEINFO_HPP_20190420_

#include "classes/Player.hpp"
#include "utilities/io.hpp"

#include <array>
#include <cstddef>
#include <optional>
#include <utility>

namespace mahjong
{
    class GameInfo
    {
    public:
        static constexpr std::size_t numOfPlayers {4};

        GameInfo() = delete;
        GameInfo(Player player, std::array<Rival, numOfPlayers - 1> rivals)
            : player_ {std::move(player)}, rivals_ {std::move(rivals)}
        {}

        io::String serialize(io::StringView delimiter = ",") const;
        static GameInfo deserialize(
            io::InStream& data, io::InStream::char_type delimiter);

        void parseOnePieceOfRequest(io::StringView request);

    private:
        Player player_;
        std::array<Rival, numOfPlayers - 1> rivals_;
        Hand discardedTiles_ {};
    };
} // namespace mahjong

#endif
