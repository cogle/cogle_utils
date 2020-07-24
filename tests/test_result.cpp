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
        Result<char, int> result_ok{Ok<char>{a}};

        Result<char, int> result_cpy = result_ok;

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

    SECTION("Result<void, int> copy constrcut") {
        Ok<void> ok_void{};
        Result<void, int> result{ok_void};

        REQUIRE(result.is_ok());
        REQUIRE_FALSE(result.is_err());
    }
    SECTION("Result<void, int> move construct") {
        Result<void, int> result{Ok<void>{}};

        REQUIRE(result.is_ok());
        REQUIRE_FALSE(result.is_err());
    }
    SECTION("Result<void, int> copy assignment") {
        Result<void, int> ok{Ok<void>{}};

        Result<void, int> result_cpy = ok;

        REQUIRE(result_cpy.is_ok());
        REQUIRE_FALSE(result_cpy.is_err());
    }
    SECTION("Result<void, int> move assignment") {
        Result<void, int> result{Ok<void>{}};

        Result<void, int> result_cpy = std::move(result);

        REQUIRE(result_cpy.is_ok());
        REQUIRE_FALSE(result_cpy.is_err());
    }
}

TEST_CASE("Result and_then()", "[result]") {
    SECTION("Result<char, int>{Ok} -> and_then(char)[Result<string, int>]") {
        constexpr char a = 'a';
        Ok<char> ok_char{a};
        Result<char, int> result{ok_char};

        REQUIRE(result.is_ok());

        SECTION("Result<char, int>{Ok} and_then() -> Result<std::string, int> valid") {
            std::string test_str = "Testing";
            auto string_result =
                result.and_then([=](const char) { return Result<std::string, int>{Ok<std::string>{test_str}}; });

            REQUIRE(string_result.is_ok());
            REQUIRE(string_result.result() == test_str);
        }
    }
    SECTION("Result<char, int>{Ok} -> and_then(char)[Result<string, int>] -> and_then(string)[Result<int, int>]") {
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
    SECTION("Result<void, int>{Ok} -> and_then()[Ok<string>]") {
        Ok<void> ok_void{};
        Result<void, int> result{ok_void};

        REQUIRE(result.is_ok());

        SECTION("Result<void, int> and_then() -> Result<std::string, int> valid") {
            std::string test_str = "Testing";
            int inc              = 1;

            auto string_result = result.and_then([=, &inc]() {
                inc += 1;
                return Result<std::string, int>{Ok<std::string>{test_str}};
            });

            REQUIRE(string_result.is_ok());
            REQUIRE(string_result.result() == test_str);
            REQUIRE(inc == 2);
        }
    }
    SECTION("Result<void, int> -> and_then()[Result<string, int>] -> and_then(string)[Result<int, int>]") {
        constexpr auto expected_string = "test";
        constexpr auto final_ret       = 1;

        constexpr auto INVALID = -1;

        int inc = 1;

        auto ret = Result<void, int>(Ok<void>{})
                       .and_then([=, &inc]() {
                           using result_t = std::string;
                           using error_t  = int;
                           using ret_t    = Result<result_t, error_t>;

                           inc += 1;

                           return ret_t{Ok<result_t>{expected_string}};
                       })
                       .and_then([=, &inc](const std::string str) {
                           using result_t = int;
                           using error_t  = int;
                           using ret_t    = Result<result_t, error_t>;

                           inc += 1;
                           if (str == expected_string) {
                               return ret_t{Ok<result_t>{final_ret}};
                           } else {
                               return ret_t{Err<error_t>{INVALID}};
                           }
                       });

        REQUIRE(ret.is_ok());
        REQUIRE_FALSE(ret.is_err());
        REQUIRE(inc == 3);
    }
}

TEST_CASE("Result map()", "[result]") {
    SECTION("Result<char, int>{Ok} -> map(char)[string]") {
        constexpr char a = 'a';
        Ok<char> ok_char{a};
        Result<char, int> result{ok_char};

        REQUIRE(result.is_ok());

        SECTION("Result<char, int> map(char) -> Result<std::string, int> valid") {
            std::string test_str = "Testing";
            auto string_result   = result.map([=](const char) { return test_str; });

            REQUIRE(string_result.is_ok());
            REQUIRE(string_result.result() == test_str);
        }
    }
    SECTION("Result<void, int>{Ok} -> map()[string]") {
        Ok<void> ok_void{};
        Result<void, int> result{ok_void};

        REQUIRE(result.is_ok());

        SECTION("Result<void, int> and_then() -> Result<std::string, int> valid") {
            std::string test_str = "Testing";
            auto string_result   = result.map([=]() { return test_str; });

            REQUIRE(string_result.is_ok());
            REQUIRE(string_result.result() == test_str);
        }
    }
}

}  // namespace
