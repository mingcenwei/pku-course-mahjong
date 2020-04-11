#ifndef METAPROGRAMMING_HPP_20200314_
#define METAPROGRAMMING_HPP_20200314_

#include <type_traits>

namespace utilities
{
    // Always false. Can be used in static_assert statements (we cannot use
    // "false" directly).
    template <typename AnyType>
    inline constexpr bool falseForStaticAssert_v {false};

    // Enable "TypeToEnable" if and only all "Assertions" are true. Useful for
    // SFINAE.
    template <typename TypeToEnable = bool, bool... Assertions>
    using TemplateAssert_t = std::enable_if<(Assertions && ...), TypeToEnable>;

    template <typename T>
    using RefIfNotScalar_t = std::
        conditional_t<std::is_scalar_v<T>, T, std::add_lvalue_reference_t<T>>;
} // namespace utilities

#endif
