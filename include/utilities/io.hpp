#ifndef IO_HPP_20190420_
#define IO_HPP_20190420_

#include <cstddef>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

namespace mahjong::io
{
    using InStream = std::istream;
    using OutStream = std::ostream;
    using String = std::string;
    using StringView = std::string_view;

    template <typename CharT, typename Traits>
    auto ignoreUntil(
        std::basic_istream<CharT,Traits>& inStream,
        typename std::basic_istream<CharT,Traits>::int_type delimiter = '\n',
        std::size_t count = 1) -> decltype((inStream));

    template <typename CharT, typename Traits>
    auto ignoreUntil(
        std::basic_istream<CharT,Traits>& inStream,
        std::basic_string_view<CharT,Traits> delimiter,
        std::size_t count = 1) -> decltype((inStream));

    void sendResponse(
        OutStream& outStream,
        StringView decision,
        StringView debugInfo,
        StringView roundData,
        StringView globalData);

    struct Request
    {
        std::vector<String> allRequests;
        std::vector<String> allResponses;
        String roundData;
        String globalData;
    };

    Request receiveRequest(
        InStream& inStream, bool ignorePreviousRequestsAndResponses = true);
} // namespace mahjong::io

#include "utilities/io.ipp"

#endif
