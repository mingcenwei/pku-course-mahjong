#include "utilities/debugging.hpp"

#include <cstddef>

namespace
{
    int main_impl(int const argc, char const* const* const argv)
    {
        static_cast<void>(argc);
        static_cast<void>(argv);

        return 0;
    }
} // namespace

int main(int const argc, char const* const* const argv)
{
    try
    {
        return main_impl(argc, argv);
    }
    catch (utilities::ExitNormally_t const& signal)
    {
        return signal.returnCode;
    }
    catch (utilities::ExitAbnormally_t const& signal)
    {
        return signal.returnCode;
    }
}
