#include "catch2/catch.hpp"
#include "utils/result.hxx"

namespace {

using namespace cogle::utils::result;

constexpr char func_ok_val = 'z';

TEST_CASE("Result Copy Construct Ok", "[result]") {
    SECTION("Result<char, int> copy constrcut") {
        constexpr char a = 'a';
        Ok<char> ok_char{a};
        Result<char, int> result{ok_char};

        REQUIRE(result.is_ok() == true);
        REQUIRE(result.is_err() == false);
    }
    SECTION("Result<char, int> move construct") {
        constexpr char a = 'a';
        Result<char, int> result{Ok<char>{a}};

        REQUIRE(result.is_ok() == true);
        REQUIRE(result.is_err() == false);
    }
    SECTION("Result<char, int> move assignment") {
        /*
        constexpr char a = 'a';
        Result<char, int> result{Ok<char>{a}};

        Result<char, int> result_move = std::move(result);
        */
    }
}

TEST_CASE("Result and_then()", "[result]") {
    SECTION("Result<char, int> copy construct") {
        constexpr char a = 'a';
        Ok<char> ok_char{a};
        Result<char, int> result{ok_char};

        REQUIRE(result.is_ok() == true);

        /*
        SECTION("Result<char, int> and_then() -> Result<std::string, int> valid") {
            // TODO extract this lambda out more and apply it here to assert
            auto string_result =
                result.and_then([](const char) { return Result<std::string, int>{Ok<std::string>{"Testing"}}; });

            REQUIRE(string_result.is_ok() == true);
        }
        */
    }
}

}  // namespace
