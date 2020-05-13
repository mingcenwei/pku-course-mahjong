#include "classes/Player.hpp"
#include "utilities/debugging.hpp"

#include <array>
#include <charconv>
#include <functional>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace
{
    namespace fan_calculation_library
    {
#if defined(_BOTZONE_ONLINE)
    #include "MahjongGB/fan_calculator.cpp"
    #include "MahjongGB/shanten.cpp"
#else
    #include "fan_calculator.cpp"
    #include "shanten.cpp"
#endif
    } // namespace fan_calculation_library

    namespace fcl = fan_calculation_library::mahjong;

    using CalculationParameters = fcl::calculate_param_t;
    using FclTile = fcl::tile_t;
    using FclTileKind = fcl::suit_t;
    using FclTileIndex = fcl::rank_t;
    using FclWind = fcl::wind_t;
    using FclHuFlags = fcl::win_flag_t;
    using FclFulu = fcl::pack_t;
    using FclFanTable = fcl::fan_table_t;
    using FclAllHandTiles = fcl::hand_tiles_t;

    constexpr auto generateTileConversionMap() noexcept
    {
        using Traits = mahjong::TileTraits<mahjong::Tile>;
        using IndexType = Traits::IndexType;

        constexpr std::hash<mahjong::Tile> hash {};
        constexpr auto mapSize {std::size(mahjong::k_tileKindList) * 10};
        std::array<FclTile, mapSize> map {};

        IndexType tileIndex {0};
        for (FclTile mappedTile {0x11}; mappedTile <= 0x19; ++mappedTile)
        {
            ++tileIndex;
            mahjong::Tile const tile {mahjong::TileKind::wan, tileIndex};
            map.at(hash(tile)) = mappedTile;
        }
        tileIndex = {0};
        for (FclTile mappedTile {0x21}; mappedTile <= 0x29; ++mappedTile)
        {
            ++tileIndex;
            mahjong::Tile const tile {mahjong::TileKind::tiao, tileIndex};
            map.at(hash(tile)) = mappedTile;
        }
        tileIndex = {0};
        for (FclTile mappedTile {0x31}; mappedTile <= 0x39; ++mappedTile)
        {
            ++tileIndex;
            mahjong::Tile const tile {mahjong::TileKind::bing, tileIndex};
            map.at(hash(tile)) = mappedTile;
        }
        tileIndex = {0};
        for (FclTile mappedTile {0x41}; mappedTile <= 0x44; ++mappedTile)
        {
            ++tileIndex;
            mahjong::Tile const tile {mahjong::TileKind::feng, tileIndex};
            map.at(hash(tile)) = mappedTile;
        }
        tileIndex = {0};
        for (FclTile mappedTile {0x45}; mappedTile <= 0x47; ++mappedTile)
        {
            ++tileIndex;
            mahjong::Tile const tile {mahjong::TileKind::jian, tileIndex};
            map.at(hash(tile)) = mappedTile;
        }
        tileIndex = {0};
        for (FclTile mappedTile {0x51}; mappedTile <= 0x58; ++mappedTile)
        {
            ++tileIndex;
            mahjong::Tile const tile {mahjong::TileKind::hua, tileIndex};
            map.at(hash(tile)) = mappedTile;
        }
        return map;
    }

    constexpr auto k_tileConversionMap {generateTileConversionMap()};

    constexpr FclTile convertTile(mahjong::Tile const tile)
    {
        constexpr std::hash<mahjong::Tile> hash {};
        return k_tileConversionMap[hash(tile)];
    }

    static_assert(
        convertTile(mahjong::Tile {mahjong::TileKind::jian, 1}) == fcl::TILE_C,
        "error in function 'convertTile'");

    constexpr FclWind convertWind(mahjong::Wind const wind)
    {
        return static_cast<FclWind>(wind);
    }

    static_assert(
        convertWind(mahjong::Wind::south) == FclWind::SOUTH,
        "error in function 'convertWind'");

    template <typename To, typename From>
    constexpr void assignInteger(To& to, From const from) noexcept
    {
        static_assert(std::is_integral_v<To>);
        static_assert(std::is_integral_v<From>);
        to = static_cast<To>(from);
    }

    constexpr FclHuFlags getHuFlags(
        bool const isZimo,
        bool const isJuezhang,
        bool const isGang,
        bool const isLastTile,
        bool const isQishou) noexcept
    {
        FclHuFlags flags {0};
        if (isZimo)
        {
            flags |= WIN_FLAG_SELF_DRAWN;
        }
        if (isJuezhang)
        {
            flags |= WIN_FLAG_4TH_TILE;
        }
        if (isGang)
        {
            flags |= WIN_FLAG_ABOUT_KONG;
        }
        if (isLastTile)
        {
            flags |= WIN_FLAG_WALL_LAST;
        }
        if (isQishou)
        {
            flags |= WIN_FLAG_INIT;
        }
        return flags;
    }

    template <
        template <typename>
        typename FlagContainer_,
        typename Chi_,
        typename Peng_,
        typename Gang_>
    void convertFulu(
        FclFulu* fulu,
        mahjong::Wind const menfeng,
        Chi_ const& firstTileOfChi,
        FlagContainer_<std::size_t> const& chiIndices,
        Peng_ const& peng,
        FlagContainer_<mahjong::Wind> const& pengFrom,
        Gang_ const& gang,
        FlagContainer_<mahjong::Wind> const& gangFrom)
    {
        for (std::size_t index {0}; index < std::size(firstTileOfChi); ++index)
        {
            using namespace utilities;

            using FclOfferType = uint8_t;

            auto const chiIndex {
                static_cast<FclOfferType>(getElement(chiIndices, index))};
            auto tile {convertTile(getElement(firstTileOfChi, index))};
            assignInteger(tile, tile + 1);
            *fulu = fcl::make_pack(chiIndex, PACK_TYPE_CHOW, tile);
            ++fulu;
        }
        for (std::size_t index {0}; index < std::size(peng); ++index)
        {
            using namespace utilities;

            using FclOfferType = uint8_t;

            auto const fromWind {getElement(pengFrom, index)};
            auto const fromIndex {static_cast<FclOfferType>(
                mahjong::windDistance(menfeng, fromWind))};
            auto const tile {convertTile(getElement(peng, index))};
            *fulu = fcl::make_pack(fromIndex, PACK_TYPE_PUNG, tile);
            ++fulu;
        }
        for (std::size_t index {0}; index < std::size(gang); ++index)
        {
            using namespace utilities;

            using FclOfferType = uint8_t;

            auto const fromWind {getElement(gangFrom, index)};
            auto const fromIndex {static_cast<FclOfferType>(
                mahjong::windDistance(menfeng, fromWind))};
            auto const tile {convertTile(getElement(gang, index))};
            *fulu = fcl::make_pack(fromIndex, PACK_TYPE_KONG, tile);
            ++fulu;
        }
    }

    template <
        template <typename>
        typename FlagContainer_,
        typename Hand_,
        typename Chi_,
        typename Peng_,
        typename Gang_>
    FclAllHandTiles getAllHandTiles(
        Hand_ const& hand,
        Chi_ const& firstTileOfChi,
        FlagContainer_<std::size_t> const& chiIndices,
        Peng_ const& peng,
        FlagContainer_<mahjong::Wind> const& pengFrom,
        Gang_ const& gang,
        FlagContainer_<mahjong::Wind> const& gangFrom,
        mahjong::Wind const menfeng)
    {
        FclAllHandTiles allHandTiles {};

        {
            std::size_t index {0};
            for (auto tile: hand)
            {
                allHandTiles.standing_tiles[index] = convertTile(tile);
                ++index;
            }
        }
        convertFulu<FlagContainer_>(
            allHandTiles.fixed_packs,
            menfeng,
            firstTileOfChi,
            chiIndices,
            peng,
            pengFrom,
            gang,
            gangFrom);
        assignInteger(
            allHandTiles.pack_count,
            std::size(firstTileOfChi) + std::size(peng) + std::size(gang));
        assignInteger(allHandTiles.tile_count, std::size(hand));

        return allHandTiles;
    }

    CalculationParameters getCalculationParameters(
        FclAllHandTiles&& allHandTiles,
        mahjong::Tile const winningTile,
        mahjong::Wind const quanfeng,
        mahjong::Wind const menfeng,
        std::size_t const numOfHua,
        bool const isZimo,
        bool const isJuezhang,
        bool const isGang,
        bool const isLastTile,
        bool const isQishou) noexcept
    {
        CalculationParameters params {};

        params.hand_tiles = std::move(allHandTiles);
        params.win_tile = convertTile(winningTile);
        assignInteger(params.flower_count, numOfHua);
        params.win_flag =
            getHuFlags(isZimo, isJuezhang, isGang, isLastTile, isQishou);
        params.prevalent_wind = convertWind(quanfeng);
        params.seat_wind = convertWind(menfeng);

        return params;
    }
    [[maybe_unused]] CalculationParameters getCalculationParameters(
        FclAllHandTiles const& allHandTiles,
        mahjong::Tile const winningTile,
        mahjong::Wind const quanfeng,
        mahjong::Wind const menfeng,
        std::size_t const numOfHua,
        bool const isZimo,
        bool const isJuezhang,
        bool const isGang,
        bool const isLastTile,
        bool const isQishou) noexcept
    {
        CalculationParameters params {};

        params.hand_tiles = allHandTiles;
        params.win_tile = convertTile(winningTile);
        assignInteger(params.flower_count, numOfHua);
        params.win_flag =
            getHuFlags(isZimo, isJuezhang, isGang, isLastTile, isQishou);
        params.prevalent_wind = convertWind(quanfeng);
        params.seat_wind = convertWind(menfeng);

        return params;
    }

    template <typename Integer>
    Integer stringViewToInteger(mahjong::io::StringView strView)
    {
        Integer num;
        auto const conversionResult {
            std::from_chars(std::begin(strView), std::end(strView), num)};
        if constexpr (utilities::k_isDebugging)
        {
            if (conversionResult.ec == std::errc::invalid_argument)
            {
                throw std::invalid_argument {
                    "the string_view cannot be converted to an integer"};
            }
            else if (conversionResult.ec == std::errc::result_out_of_range)
            {
                throw std::range_error {"integer conversion out of range"};
            }
        }
        return num;
    }

    mahjong::io::String serializeWind(mahjong::Wind const wind)
    {
        return std::to_string(mahjong::windToIndex(wind));
    }
    mahjong::Wind unsafeDeserializeWind(mahjong::io::InStream& inStream)
    {
        std::size_t index;
        inStream >> index;
        return mahjong::indexToWind(index);
    }
    mahjong::Wind unsafeStrToWind(mahjong::io::StringView const& str)
    {
        return mahjong::indexToWind(stringViewToInteger<std::size_t>(str));
    }

    template <typename Container, typename Serializer>
    mahjong::io::String serializeContainer(
        Container const& container,
        Serializer const& serializer,
        std::size_t const eachLength = 1,
        mahjong::io::StringView const delimiter = "")
    {
        mahjong::io::String result {};
        auto const size {std::size(container)};
        auto const delimiterLength {std::size(delimiter)};
        auto const capacity {size * (eachLength + delimiterLength)};
        result.reserve(capacity);
        if constexpr (std::is_same_v<Container, std::vector<bool>>)
        {
            for (auto object: container)
            {
                result += serializer(object);
                result += delimiter;
            }
        }
        else
        {
            for (auto&& object: container)
            {
                result += serializer(object);
                result += delimiter;
            }
        }

        return result;
    }

    template <typename Container, typename Deserializer>
    Container deserializeContainer(
        mahjong::io::String const& data,
        Deserializer const& deserializer,
        std::size_t const eachLength = 1,
        mahjong::io::StringView const delimiter = "")
    {
        Container result {};
        auto const delimiterLength {std::size(delimiter)};
        auto const itemStringLength {eachLength + delimiterLength};
        if constexpr (utilities::k_isDebugging)
        {
            if (std::size(data) % itemStringLength != 0)
            {
                throw std::invalid_argument {"invalid data string"};
            }
        }
        auto numOfItems {std::size(data) / itemStringLength};
        result.reserve(numOfItems);
        for (std::size_t index {0}; index < numOfItems; ++index)
        {
            auto const offset {index * itemStringLength};
            mahjong::io::StringView const itemStringView {
                std::data(data) + offset, eachLength};
            result.emplace_back(deserializer(itemStringView));
        }
        return result;
    }
} // namespace

namespace mahjong
{
    auto Player::calculateFan(
        Tile const winningTile,
        Wind const quanfeng,
        bool const isZimo,
        bool const isJuezhang,
        bool const isGang,
        bool const isLastTile,
        bool const isQishou) const noexcept(!utilities::k_isDebugging) -> Fan
    {
        auto allHandTiles {getAllHandTiles<FlagContainer_>(
            hand_,
            firstTileOfChi_,
            chiIndices_,
            peng_,
            pengFrom_,
            gang_,
            gangFrom_,
            menfeng_)};
        auto const params {getCalculationParameters(
            std::move(allHandTiles),
            winningTile,
            quanfeng,
            menfeng_,
            std::size(hua_),
            isZimo,
            isJuezhang,
            isGang,
            isLastTile,
            isQishou)};

        FclFanTable fanTable {};
        auto const fan {fcl::calculate_fan(
            std::addressof(params), std::addressof(fanTable))};

        if constexpr (utilities::k_isDebugging)
        {
            if (fan == ERROR_WRONG_TILES_COUNT)
            {
                throw std::invalid_argument {
                    "the total count of the tiles is incorrect"};
            }
            else if (fan == ERROR_TILE_COUNT_GREATER_THAN_4)
            {
                throw std::invalid_argument {
                    "some tile appears more than 4 times"};
            }
            else if (fan == ERROR_NOT_WIN)
            {
                throw std::invalid_argument {"cannot hu right now"};
            }
        }

        return static_cast<Fan>(fan);
    }

    auto Player::calculateFanTable(
        Tile const winningTile,
        Wind const quanfeng,
        bool const isZimo,
        bool const isJuezhang,
        bool const isGang,
        bool const isLastTile,
        bool const isQishou) const -> FanTable
    {
        auto allHandTiles {getAllHandTiles<FlagContainer_>(
            hand_,
            firstTileOfChi_,
            chiIndices_,
            peng_,
            pengFrom_,
            gang_,
            gangFrom_,
            menfeng_)};
        auto const params {getCalculationParameters(
            std::move(allHandTiles),
            winningTile,
            quanfeng,
            menfeng_,
            std::size(hua_),
            isZimo,
            isJuezhang,
            isGang,
            isLastTile,
            isQishou)};

        FclFanTable fanTable {};
        auto const fan {fcl::calculate_fan(
            std::addressof(params), std::addressof(fanTable))};

        if constexpr (utilities::k_isDebugging)
        {
            if (fan == ERROR_WRONG_TILES_COUNT)
            {
                throw std::invalid_argument {
                    "the total count of the tiles is incorrect"};
            }
            else if (fan == ERROR_TILE_COUNT_GREATER_THAN_4)
            {
                throw std::invalid_argument {
                    "some tile appears more than 4 times"};
            }
            else if (fan == ERROR_NOT_WIN)
            {
                throw std::invalid_argument {"cannot hu right now"};
            }
        }

        constexpr auto length {std::size(fanTable)};
        FanTable newFanTable {};
        newFanTable.reserve(length);

        for (std::size_t index {0}; index < length; ++index)
        {
            newFanTable.emplace_back(
                fcl::fan_name[index],
                fcl::fan_value_table[index],
                fanTable[index]);
        }
        return newFanTable;
    }

    void Player::printFanTable(
        FanTable const& fantable, bool const onlyPrintExisting)
    {
        for (auto const& [fanName, fan, count]: fantable)
        {
            if (!onlyPrintExisting || count > 0)
            {
                std::cout << fanName << ", " << fan << ": " << count << "\n";
            }
        }
    }

    io::String Player::serialize(io::StringView const delimiter) const
    {
        constexpr std::size_t initialCapacity {300};
        io::String result {};
        result.reserve(initialCapacity);

        result += serializeWind(menfeng_);
        result += delimiter;
        result += hand_.toString();
        result += delimiter;
        result += hua_.toString();
        result += delimiter;
        result += firstTileOfChi_.toString();
        result += delimiter;
        constexpr auto sizeTypeToString {
            [](std::size_t const size) { return std::to_string(size); }};
        result += serializeContainer(chiIndices_, sizeTypeToString);
        result += delimiter;
        result += peng_.toString();
        result += delimiter;
        result += serializeContainer(pengFrom_, serializeWind);
        result += delimiter;
        result += gang_.toString();
        result += delimiter;
        result += serializeContainer(gangFrom_, serializeWind);
        result += delimiter;
        result += serializeContainer(isJiagang_, sizeTypeToString);
        result += delimiter;

        result.shrink_to_fit();
        return result;
    }

    Player Player::deserialize(
        io::InStream& data, io::InStream::char_type const delimiter)
    {
        constexpr std::size_t initialBufferCapacity {100};
        io::String buffer {};
        buffer.reserve(initialBufferCapacity);

        auto const menfeng {unsafeDeserializeWind(data)};
        io::ignoreUntil(data, delimiter);
        std::getline(data, buffer, delimiter);
        Player::Hand_ hand {buffer};
        Player player {menfeng, std::move(hand)};
        std::getline(data, buffer, delimiter);
        player.hua_ = Hua_ {buffer};
        std::getline(data, buffer, delimiter);
        player.firstTileOfChi_ = Chi_ {buffer};
        std::getline(data, buffer, delimiter);
        player.chiIndices_ = deserializeContainer<decltype(player.chiIndices_)>(
            buffer, stringViewToInteger<std::size_t>);
        std::getline(data, buffer, delimiter);
        player.peng_ = Peng_ {buffer};
        std::getline(data, buffer, delimiter);
        player.pengFrom_ = deserializeContainer<decltype(player.pengFrom_)>(
            buffer, unsafeStrToWind);
        std::getline(data, buffer, delimiter);
        player.gang_ = Gang_ {buffer};
        std::getline(data, buffer, delimiter);
        player.gangFrom_ = deserializeContainer<decltype(player.gangFrom_)>(
            buffer, unsafeStrToWind);
        std::getline(data, buffer, delimiter);
        player.isJiagang_ = deserializeContainer<decltype(player.isJiagang_)>(
            buffer, [](auto const& str) { return str == "0" ? true : false; });
        return player;
    }

    io::String Rival::serialize(io::StringView const delimiter) const
    {
        constexpr std::size_t initialCapacity {200};
        io::String result {};
        result.reserve(initialCapacity);

        result += serializeWind(menfeng_);
        result += delimiter;
        result += hua_.toString();
        result += delimiter;
        result += firstTileOfChi_.toString();
        result += delimiter;
        constexpr auto sizeTypeToString {
            [](std::size_t const size) { return std::to_string(size); }};
        result += serializeContainer(chiIndices_, sizeTypeToString);
        result += delimiter;
        result += peng_.toString();
        result += delimiter;
        result += serializeContainer(pengFrom_, serializeWind);
        result += delimiter;
        result += gang_.toString();
        result += delimiter;
        result += serializeContainer(gangFrom_, serializeWind);
        result += delimiter;
        result += serializeContainer(isJiagang_, sizeTypeToString);
        result += delimiter;

        result.shrink_to_fit();
        return result;
    }

    Rival Rival::deserialize(
        io::InStream& data, io::InStream::char_type const delimiter)
    {
        constexpr std::size_t initialBufferCapacity {100};
        io::String buffer {};
        buffer.reserve(initialBufferCapacity);

        auto const menfeng {unsafeDeserializeWind(data)};
        io::ignoreUntil(data, delimiter);
        Rival rival {menfeng};
        std::getline(data, buffer, delimiter);
        rival.hua_ = Hua_ {buffer};
        std::getline(data, buffer, delimiter);
        rival.firstTileOfChi_ = Chi_ {buffer};
        std::getline(data, buffer, delimiter);
        rival.chiIndices_ = deserializeContainer<decltype(rival.chiIndices_)>(
            buffer, stringViewToInteger<std::size_t>);
        std::getline(data, buffer, delimiter);
        rival.peng_ = Peng_ {buffer};
        std::getline(data, buffer, delimiter);
        rival.pengFrom_ = deserializeContainer<decltype(rival.pengFrom_)>(
            buffer, unsafeStrToWind);
        std::getline(data, buffer, delimiter);
        rival.gang_ = Gang_ {buffer};
        std::getline(data, buffer, delimiter);
        rival.gangFrom_ = deserializeContainer<decltype(rival.gangFrom_)>(
            buffer, unsafeStrToWind);
        std::getline(data, buffer, delimiter);
        rival.isJiagang_ = deserializeContainer<decltype(rival.isJiagang_)>(
            buffer, [](auto const& str) { return str == "0" ? true : false; });
        return rival;
    }
} // namespace mahjong
