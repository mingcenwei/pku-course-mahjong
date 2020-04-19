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
        case TileKind::bing:
            return "饼"sv;
            break;
        case TileKind::tiao:
            return "条"sv;
            break;
        case TileKind::wan:
            return "万"sv;
            break;
        case TileKind::feng:
            return "风"sv;
            break;
        case TileKind::jian:
            return "箭"sv;
            break;
        case TileKind::hua:
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
        constexpr std::string_view promptPrefix {
            "invalid tile index: tile kind "};
        constexpr auto promptPrefixLength {std::size(promptPrefix)};
        constexpr auto memoryToReserve {promptPrefixLength + 15};
        std::string msg {};
        msg.reserve(memoryToReserve);
        msg += promptPrefix;
        msg += tileKindToStringView(kind);
        msg += ", index ";
        msg += std::to_string(index);
        throw std::invalid_argument {msg};
    }
} // namespace mahjong
