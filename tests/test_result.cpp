#include "catch2/catch.hpp"
#include "utils/result.hxx"

namespace {

using namespace cogle::utils::result;

TEST_CASE("Result Copy Construct Ok", "[result]") {
    SECTION("Result<char, int> copy constrcut") {
        constexpr char a = 'a';
        Ok<char> ok_char{a};
        Result<char, int> result{ok_char};

        REQUIRE(result.is_ok());
        REQUIRE_FALSE(result.is_err());
    }
    SECTION("Result<char, int> move construct") {
        constexpr char a = 'a';
        Result<char, int> result{Ok<char>{a}};

        REQUIRE(result.is_ok());
        REQUIRE_FALSE(result.is_err());
    }
    SECTION("Result<char, int> copy assignment") {
        constexpr char a = 'a';
        Result<char, int> ok{Ok<char>{a}};

        Result<char, int> result_cpy = ok;

        REQUIRE(result_cpy.is_ok());
        REQUIRE_FALSE(result_cpy.is_err());
    }
    SECTION("Result<char, int> move assignment") {
        constexpr char a = 'a';
        Result<char, int> result{Ok<char>{a}};

        Result<char, int> result_cpy = std::move(result);

        REQUIRE(result_cpy.is_ok());
        REQUIRE_FALSE(result_cpy.is_err());
    }
}

TEST_CASE("Result and_then()", "[result]") {
    SECTION("Result<Ok<char>> -> and_then(Ok<string>)") {
        constexpr char a = 'a';
        Ok<char> ok_char{a};
        Result<char, int> result{ok_char};

        REQUIRE(result.is_ok());

        SECTION("Result<char, int> and_then() -> Result<std::string, int> valid") {
            std::string test_str = "Testing";
            auto string_result =
                result.and_then([=](const char) { return Result<std::string, int>{Ok<std::string>{test_str}}; });

            REQUIRE(string_result.is_ok());
            REQUIRE(string_result.result() == test_str);
        }
    }
    SECTION("Result<Ok<char>> -> and_then()[Ok<string>] -> and_then()[Ok<int>]") {
        constexpr char a               = 'a';
        constexpr auto expected_string = "test";
        constexpr auto final_ret       = 1;

        constexpr auto INVALID = -1;

        auto ret = Result<char, int>(Ok<char>{a})
                       .and_then([=](const char c) {
                           using result_t = std::string;
                           using error_t  = int;
                           using ret_t    = Result<result_t, error_t>;

                           if (c == a) {
                               return ret_t{Ok<result_t>{expected_string}};
                           } else {
                               return ret_t{Err<error_t>{INVALID}};
                           }
                       })
                       .and_then([=](const std::string str) {
                           using result_t = int;
                           using error_t  = int;
                           using ret_t    = Result<result_t, error_t>;

                           if (str == expected_string) {
                               return ret_t{Ok<result_t>{final_ret}};
                           } else {
                               return ret_t{Err<error_t>{INVALID}};
                           }
                       });

        REQUIRE(ret.is_ok());
        REQUIRE_FALSE(ret.is_err());
    }
}

TEST_CASE("Result map()", "[result]") {
    SECTION("Result<Ok<char>> -> map(Ok<string>)") {
        constexpr char a = 'a';
        Ok<char> ok_char{a};
        Result<char, int> result{ok_char};

        REQUIRE(result.is_ok());

        SECTION("Result<char, int> and_then() -> Result<std::string, int> valid") {
            std::string test_str = "Testing";
            auto string_result   = result.map([=](const char) { return test_str; });

            REQUIRE(string_result.is_ok());
            REQUIRE(string_result.result() == test_str);
        }
    }
}

}  // namespace
