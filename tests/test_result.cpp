#include "catch2/catch.hpp"
#include "utils/result.hxx"

namespace {

using namespace cogle::utils::result;

TEST_CASE("Result Copy Construct Ok", "[result]") {
    SECTION("Result<char, int> copy") {
        constexpr char a = 'a';
        Ok<char> ok_char{a};
        Result<char, int> result{ok_char};

        REQUIRE(result.is_ok() == true);
        REQUIRE(result.is_err() == false);
    }
    SECTION("Result<char, int> move") {
        constexpr char a = 'a';
        Result<char, int> result{Ok<char>{a}};

        REQUIRE(result.is_ok() == true);
        REQUIRE(result.is_err() == false);
    }
}

}  // namespace
