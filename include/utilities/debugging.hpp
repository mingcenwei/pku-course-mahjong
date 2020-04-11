#ifndef DEBUGGING_HPP_20200314_
#define DEBUGGING_HPP_20200314_

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string_view>
#include <utility>


// Use <source_location> in C++20 to rewrite this file.
#if __cplusplus >= 202002L
static_assert(false, "Use <source_location> in C++20 to rewrite this file");
#else
namespace utilities
{
    // Whether or not we are debugging.
    // True if macro DEBUG_ is defined and NDEBUG is not defined.
    #if defined(DEBUG_) && !defined(NDEBUG)
    inline constexpr bool k_isDebugging {true};
    #else
    inline constexpr bool k_isDebugging {false};
    #endif

    // Print messages if and only k_isDebugging is true, i.e. we are debugging.
    template <typename... Printables>
    constexpr void debugPrint(Printables&&... args) noexcept(!k_isDebugging)
    {
        if constexpr (k_isDebugging)
        {
            auto const flags {std::cerr.flags()};
            std::cerr << std::boolalpha;
            (std::cerr << ... << std::forward<Printables>(args));
            std::cerr.flags(flags);
        }
    }

    // Print one additional new line.
    template <typename... Printables>
    constexpr void debugPrintLine(Printables&&... args) noexcept(!k_isDebugging)
    {
        debugPrint(std::forward<Printables>(args)..., "\n");
    }

    // Will print the filename and line number in addition to the given
    // arguments. In C++20 we can replace this macro with a function via
    // <source_location> header.
    #define utilities_DEBUG_PRINT_WITH_SOURCE_LOCATION(...) \
        ::utilities::debugPrintLine( \
            "<---------- File: ", \
            __FILE__, \
            " | Line: ", \
            __LINE__, \
            " ---------->\n", \
            __VA_ARGS__)

    // Use bounds checking only when we are debugging. "Container" should be a
    // container type from the C++ standard library.
    template <typename Container, typename Index>
    constexpr auto getElement(Container&& container, Index&& index)
    {
        if constexpr (k_isDebugging)
        {
            return std::forward<Container>(container).at(
                std::forward<Index>(index));
        }
        else
        {
            return std::forward<Container>(
                container)[std::forward<Index>(index)];
        }
    }

    // An exception indicates that a supposedly unreachable code block is
    // reached.
    class UnreachableCodeError : public std::logic_error
    {
    public:
        using std::logic_error::logic_error;
    };

    // Put this function to locations in other functions where processes should
    // never reach. If "enabledOnlyWhenDebugging" is true, this function has
    // effect only in debugging.
    template <bool enabledOnlyWhenDebugging = true>
    [[noreturn]] inline constexpr void unreachableCodeBlock(
        bool const printErrorMsg = false,
        std::string_view const msg =
            "this code block should have been unreachable") noexcept(!k_isDebugging && enabledOnlyWhenDebugging)
    {
        if constexpr (k_isDebugging || !enabledOnlyWhenDebugging)
        {
            if (printErrorMsg)
            {
                std::cerr << msg << "\n";
            }
            throw UnreachableCodeError {msg.cbegin()};
        }
    }

    // Exit the program safely; perform stack unwinding and necessary cleaning
    // work. Should be "catch"ed in "main" function.
    struct ExitNormally_t
    {
        static constexpr int returnCode {EXIT_SUCCESS};
    };
    struct ExitAbnormally_t
    {
        static constexpr int defaultReturnCode {EXIT_FAILURE};
        int const returnCode {defaultReturnCode};
    };
    [[noreturn]] inline void exitNormally() { throw ExitNormally_t {}; }
    [[noreturn]] inline void exitAbnormally(
        int const returnCode = ExitAbnormally_t::defaultReturnCode)
    {
        throw ExitAbnormally_t {returnCode};
    }
} // namespace utilities
#endif

#endif
