#include "utilities/io.hpp"

#include <cstddef>
#include <limits>
#include <utility>

namespace mahjong::io
{
    void sendResponse(
        OutStream& outStream,
        StringView const decision,
        StringView const debugInfo,
        StringView const roundData,
        StringView const globalData)
    {
        outStream << decision << "\n"
                  << debugInfo << "\n"
                  << roundData << "\n"
                  << globalData << std::endl;
    }

    Request receiveRequest(
        InStream& inStream, bool const ignorePreviousRequestsAndResponses)
    {
        Request request {};
        auto& [allRequests, allResponses, roundData, globalData] {request};

        std::size_t round;
        inStream >> round;
        ignoreUntil(inStream, '\n');
        if (ignorePreviousRequestsAndResponses)
        {
            ignoreUntil(inStream, '\n', 2 * round - 2);
            String buffer;
            std::getline(inStream, buffer);
            allRequests.emplace_back(std::move(buffer));
        }
        else
        {
            allRequests.reserve(round);
            allResponses.reserve(round - 1);
            constexpr std::size_t initialBufferCapacity {100};
            String buffer;
            buffer.reserve(initialBufferCapacity);
            for (std::size_t index {0}; index < round - 1; ++index)
            {
                std::getline(inStream, buffer);
                allRequests.emplace_back(std::move(buffer));
                std::getline(inStream, buffer);
                allResponses.emplace_back(std::move(buffer));
            }
            std::getline(inStream, buffer);
            allRequests.emplace_back(std::move(buffer));
        }
        std::getline(inStream, roundData);
        std::getline(inStream, globalData);

        return request;
    }
} // namespace mahjong::io
