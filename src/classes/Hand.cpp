#include "classes/Hand.hpp"

#include <algorithm>
#include <stdexcept>
#include <string>

namespace mahjong
{
    Hand::Hand(StringViewType const representation)
    {
        if (!isValidStringRepresentation(representation))
        {
            throwInvalidStringRepresentationException(representation);
        }

        auto const length {std::size(representation)};
        if (length == 0)
        {
            return;
        }

        tiles_.reserve(length - 1);
        auto ch {representation.back()};
        TileKind currentTileKind {unsafeCharToTileKind(ch)};
        for (auto position {++std::rbegin(representation)};
             position != std::rend(representation);
             ++position)
        {
            ch = *position;
            if (ch >= '0' && ch <= '9')
            {
                auto const index {
                    static_cast<Hand::TileType::IndexType>(ch - '0')};
                tiles_.emplace_back(currentTileKind, index);
            }
            else
            {
                currentTileKind = unsafeCharToTileKind(ch);
            }
        }
        std::reverse(std::begin(tiles_), std::end(tiles_));
        tiles_.shrink_to_fit();
    }

    [[nodiscard]] auto Hand::toString() const -> StringType
    {
        if (std::empty(tiles_))
        {
            return {};
        }

        using Traits = TileTraits<TileType>;
        StringType representation {};
        representation.reserve(std::size(tiles_) * 2);
        representation += std::to_string(Traits::getIndex(tiles_.front()));
        auto currentTileKind {Traits::getKind(tiles_.front())};
        for (auto iter {++std::begin(tiles_)}; iter != std::end(tiles_); ++iter)
        {
            auto const nextTileKind {Traits::getKind(*iter)};
            if (currentTileKind != nextTileKind)
            {
                representation += tileKindToChar(currentTileKind);
                currentTileKind = nextTileKind;
            }
            representation += std::to_string(Traits::getIndex(*iter));
        }
        representation += tileKindToChar(currentTileKind);
        return representation;
    }

    [[noreturn]] void Hand::throwInvalidStringRepresentationException(
        StringViewType const representation)
    {
        using namespace std::string_literals;
        auto const msg {
            "invalid hand string representation: "s.append(representation)};
        throw std::invalid_argument {msg};
    }

    Hand::OutStreamType& operator<<(
        Hand::OutStreamType& outStream, Hand const& hand)
    {
        return (outStream << hand.toString());
    }

    Hand::InStreamType& operator>>(Hand::InStreamType& inStream, Hand& hand)
    {
        Hand::StringType representation;
        inStream >> representation;
        hand = Hand {representation};
        return inStream;
    }
} // namespace mahjong

namespace mahjong
{
    inline namespace literals
    {
        Hand operator""_H(
            char const* representation,
            [[maybe_unused]] std::size_t const length)
        {
            return Hand {representation};
        }
    } // namespace literals
} // namespace mahjong
