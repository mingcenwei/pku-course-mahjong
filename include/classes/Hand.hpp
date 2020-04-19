#ifndef HAND_HPP_20200412_
#define HAND_HPP_20200412_

#include "classes/Tile.hpp"

#include <algorithm>
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

        void sort()
        {
            std::sort(std::begin(tiles_), std::end(tiles_));
        }
        template <typename Compare>
        void sort(Compare compare)
        {
            std::sort(std::begin(tiles_), std::end(tiles_), compare);
        }

        template <typename... ArgTypes>
        decltype(auto) emplace(ArgTypes&&... args)
        {
            return tiles_.emplace_back(std::forward<ArgTypes>(args)...);
        }

        [[nodiscard]] StringType toString() const;

        [[nodiscard]] auto begin() noexcept(noexcept(std::begin(tiles_)))
        {
            return std::begin(tiles_);
        }
        [[nodiscard]] auto begin() const noexcept(noexcept(std::begin(tiles_)))
        {
            return std::begin(tiles_);
        }
        [[nodiscard]] auto end() noexcept(noexcept(std::end(tiles_)))
        {
            return std::end(tiles_);
        }
        [[nodiscard]] auto end() const noexcept(noexcept(std::end(tiles_)))
        {
            return std::end(tiles_);
        }
        [[nodiscard]] decltype(auto) at(std::size_t const index)
        {
            return tiles_.at(index);
        }
        [[nodiscard]] decltype(auto) at(std::size_t const index) const
        {
            return tiles_.at(index);
        }
        [[nodiscard]] decltype(auto) operator[](std::size_t const index)
        {
            return tiles_[index];
        }
        [[nodiscard]] decltype(auto) operator[](std::size_t const index) const
        {
            return tiles_[index];
        }
        [[nodiscard]] auto size() const
            noexcept(noexcept(std::size(tiles_))) {return std::size(tiles_);}

    private:
        using Container_ = std::vector<TileType>;

        Container_ tiles_ {};

        [[noreturn]] static void throwInvalidStringRepresentationException(
            StringViewType const representation);

    public:
        using Iterator = decltype(std::begin(std::declval<Container_>()));
        using ConstIterator =
            decltype(std::begin(std::declval<Container_ const>()));
        using Reference = decltype(std::declval<Container_>().at(0));
        using ConstReference = decltype(std::declval<Container_ const>().at(0));
        using SizeType = decltype(std::size(std::declval<Container_>()));
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
