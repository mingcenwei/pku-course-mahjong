#ifndef HAND_IPP_20200412_
#define HAND_IPP_20200412_

#include "classes/Hand.hpp"
#include "utilities/debugging.hpp"

namespace mahjong
{
    constexpr bool Hand::isValidStringRepresentation(
        StringViewType const representation) noexcept
    {
        if (std::empty(representation))
        {
            return true;
        }

        auto ch {representation.back()};
        if (!(ch == 'b' || ch == 't' || ch == 'w' || ch == 'f' || ch == 'j' ||
              ch == 'h'))
        {
            return false;
        }
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
                if (!Hand::TileType::areValidArguments(currentTileKind, index))
                {
                    return false;
                }
                else
                {
                    continue;
                }
            }
            else if (
                ch == 'b' || ch == 't' || ch == 'w' || ch == 'f' || ch == 'j' ||
                ch == 'h')
            {
                currentTileKind = unsafeCharToTileKind(ch);
                continue;
            }
            else
            {
                return false;
            }
        }
        return true;
    }
} // namespace mahjong

#endif
