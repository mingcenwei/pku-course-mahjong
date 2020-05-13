#include "classes/GameInfo.hpp"

#include <sstream>

namespace mahjong
{
    io::String GameInfo::serialize(io::StringView const delimiter) const
    {
        constexpr std::size_t initialCapacity {600};
        io::String result {};
        result.reserve(initialCapacity);

        result += player_.serialize(delimiter);
        result += delimiter;
        for (auto const& rival: rivals_)
        {
            result += rival.serialize(delimiter);
            result += delimiter;
        }
        result += discardedTiles_.toString();
        result += delimiter;
        return result;
    }

    GameInfo GameInfo::deserialize(
        io::InStream& data, io::InStream::char_type const delimiter)
    {
        auto player {Player::deserialize(data, delimiter)};
        auto rival1 {Rival::deserialize(data, delimiter)};
        auto rival2 {Rival::deserialize(data, delimiter)};
        auto rival3 {Rival::deserialize(data, delimiter)};
        return {std::move(player),
                {std::move(rival1), std::move(rival2), std::move(rival3)}};
    }

    void GameInfo::parseOnePieceOfRequest(io::StringView const request)
    {
        std::basic_ostringstream<
            decltype(request)::value_type,
            decltype(request)::traits_type> requestStream {request};
            std::size_t index;
    }
} // namespace mahjong
