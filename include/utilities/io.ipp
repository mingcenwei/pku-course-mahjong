#ifndef IO_IPP_20200420_
#define IO_IPP_20200420_

#include "utilities/io.hpp"

#include <iterator>

namespace mahjong::io
{
    template <typename CharT, typename Traits>
    auto ignoreUntil(
        std::basic_istream<CharT, Traits>& inStream,
        typename std::basic_istream<CharT, Traits>::int_type const delimiter,
        std::size_t const count) -> decltype((inStream))
    {
        for (std::size_t index {0}; index < count; ++index)
        {
            inStream.ignore(
                std::numeric_limits<std::streamsize>::max(), delimiter);
        }
        return inStream;
    }

    template <typename CharT, typename Traits>
    auto ignoreUntil(
        std::basic_istream<CharT, Traits>& inStream,
        std::basic_string_view<CharT, Traits> const delimiter,
        std::size_t const count) -> decltype((inStream))
    {
        if (std::size(delimiter) > 0)
        {
            for (std::size_t index {0}; index < count && inStream.good();
                 ++index)
            {
                bool matched {false};
                while (inStream.good() && !matched)
                {
                    matched = true;
                    auto const begin {std::begin(delimiter)};
                    ignoreUntil(inStream, *begin);
                    for (auto iter {std::next(begin)};
                         iter != std::end(delimiter);
                         ++iter)
                    {
                        if (*iter != inStream.get())
                        {
                            matched = false;
                            break;
                        }
                    }
                }
            }
        }
        return inStream;
    }
} // namespace mahjong::io

#endif
