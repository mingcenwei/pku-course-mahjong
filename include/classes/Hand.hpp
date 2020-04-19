#ifndef HAND_HPP_20200412_
#define HAND_HPP_20200412_

#include "classes/Tile.hpp"

#include <cstddef>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace mahjong
{
    class Hand
    {
    public:
        using TileType = Tile;
        using StringType = std::string;
        using StringViewType = std::string_view;
        using OutStreamType = std::basic_ostream<StringType::value_type>;
        using InStreamType = std::basic_istream<StringViewType::value_type>;

        static constexpr bool isValidStringRepresentation(
            StringViewType const representation) noexcept;

        Hand() = default;
        Hand(std::initializer_list<TileType> const tiles) : tiles_ {tiles} {}
        explicit Hand(StringViewType const representation);

        [[nodiscard]] StringType toString() const;

    private:
        using Container_ = std::vector<TileType>;

        Container_ tiles_ {};

        [[noreturn]] static void throwInvalidStringRepresentationException(
            StringViewType const representation);
    };

    Hand::OutStreamType& operator<<(
        Hand::OutStreamType& outStream, Hand const& hand);
    Hand::InStreamType& operator>>(Hand::InStreamType& inStream, Hand& hand);
} // namespace mahjong

namespace mahjong
{
    inline namespace literals
    {
        Hand operator""_H(char const* representation, std::size_t const length);
    } // namespace literals
} // namespace mahjong

#include "classes/Hand.ipp"

#endif
