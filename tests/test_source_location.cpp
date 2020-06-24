#include "catch2/catch.hpp"
#include "utils/result.hxx"

namespace {
using namespace cogle::utils::location;

TEST_CASE("ResultStorage [result][ResultStorage]") {
    SECTION("ResultStorage<char, int> Ok<char> copy creation") {
        constexpr auto test_char_a = 'a';

        Ok<char>{test_char_a} ok_a;
        detail::ResultStorage<char, int> storage{ok_a};

        REQUIRE(storage.get_result() == test_char_a);
    }
}

}  // namespace