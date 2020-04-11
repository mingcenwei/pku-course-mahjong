#ifndef METAPROGRAMMING_HPP_20200314_
#define METAPROGRAMMING_HPP_20200314_

#include <type_traits>

namespace utilities
{
    // Always false. Can be used in static_assert statements (we cannot use
    // "false" directly).
    template <typename AnyType>
    inline constexpr bool falseForStaticAssert_v {false};

    // Enable "TypeToEnable" if and only all "assertions" are true. Useful for
    // SFINAE.
    template <typename TypeToEnable = bool, bool... assertions>
    using TemplateAssert_t = std::enable_if<(assertions && ...), TypeToEnable>;
    // Convenient form of "TemplateAssert_t" which lets "TypeToEnable" be bool.
    template <bool... assertions>
    using TemplateAssert0_t = TemplateAssert_t<bool, assertions...>;

    // Enabled if and only if "Integral" is a integral type.
    template <typename Integral>
    using AssertIntegral_t =
        TemplateAssert_t<bool, std::is_integral_v<Integral>>;

    //Add lvalue reference to T if condition is true.
    template <typename T, bool condition>
    using LvalueRefIf_t = std::
        conditional_t<condition, std::add_lvalue_reference_t<T>, T>;

    // Add lvalue reference to T if T is not of a scalar type.
    template <typename T>
    using LvalueRefIfNotScalar_t = LvalueRefIf_t<T, !std::is_scalar_v<T>>;
} // namespace utilities

#endif
