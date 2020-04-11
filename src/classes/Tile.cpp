#include "classes/Tile.hpp"

#include <stdexcept>
#include <string>
#include <string_view>

namespace
{
    using StringViewForOutput = std::string_view;
    constexpr StringViewForOutput tileKindToStringView(
        mahjong::TileKind const& kind) noexcept
    {
        using namespace mahjong;
        using namespace std::string_view_literals;

        switch (kind)
        {
        case TileKind::Bing:
            return "饼"sv;
            break;
        case TileKind::Tiao:
            return "条"sv;
            break;
        case TileKind::Wan:
            return "万"sv;
            break;
        case TileKind::Feng:
            return "风"sv;
            break;
        case TileKind::Jian:
            return "箭"sv;
            break;
        case TileKind::Hua:
            return "花"sv;
            break;
        default:
            utilities::unreachableCodeBlock();
            break;
        }
    }
} // namespace

namespace mahjong
{
    [[noreturn]] void Tile::throwInvalidIndexException(
        TileKind const kind, IndexType const index)
    {
        using namespace std::string_literals;
        auto msg {"invalid tile index: tile kind "s};
        constexpr std::size_t additionalMemoryToReserve {15};
        msg.reserve(msg.size() + additionalMemoryToReserve);
        msg += tileKindToStringView(kind);
        msg += ", index ";
        msg += std::to_string(index);
        throw std::invalid_argument {msg};
    }
} // namespace mahjong
