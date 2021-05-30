#include <memory>
#include <string>
#include <vector>

#include "catch2/catch.hpp"
#include "utils/result.hxx"

namespace {

using namespace cogle::utils::result;

struct ResultStruct {
    ResultStruct(int a, std::string s, std::vector<std::shared_ptr<int>> v) : num(a), str(s), ptr_vec(v) {}

    int num;
    std::string str;
    std::vector<std::shared_ptr<int>> ptr_vec;
};

struct ErrorStruct {
    std::shared_ptr<int> err;
    std::string error_msg;
};

TEST_CASE("Result Construct Ok", "[result]") {
    SECTION("Result<char, int> Ok lvalue constrcut") {
        constexpr char a = 'a';

        Ok<char> ok_char{a};
        Result<char, int> result{ok_char};

        REQUIRE(result.is_ok());
        REQUIRE_FALSE(result.is_err());
    }
    SECTION("Result<char, int> Ok rvalue construct") {
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

        REQUIRE(result_ok.is_ok());
        REQUIRE_FALSE(result_ok.is_err());
    }
    SECTION("Result<char, int> move assignment") {
        constexpr char a = 'a';

        Result<char, int> result{Ok<char>{a}};
        Result<char, int> result_cpy = std::move(result);

        REQUIRE(result_cpy.is_ok());
        REQUIRE_FALSE(result_cpy.is_err());

        REQUIRE_FALSE(result.is_ok());
        REQUIRE_FALSE(result.is_err());
    }

    SECTION("Result<void, int> Ok lvalue constrcut") {
        Ok<void> ok_void{};
        Result<void, int> result{ok_void};

        REQUIRE(result.is_ok());
        REQUIRE_FALSE(result.is_err());
    }
    SECTION("Result<void, int> Ok rvalue construct") {
        Result<void, int> result{Ok<void>{}};

        REQUIRE(result.is_ok());
        REQUIRE_FALSE(result.is_err());
    }
    SECTION("Result<void, int> copy assignment") {
        Result<void, int> result{Ok<void>{}};
        Result<void, int> result_cpy = result;

        REQUIRE(result_cpy.is_ok());
        REQUIRE_FALSE(result_cpy.is_err());

        REQUIRE(result.is_ok());
        REQUIRE_FALSE(result.is_err());
    }
    SECTION("Result<void, int> move assignment") {
        Result<void, int> result{Ok<void>{}};
        Result<void, int> result_cpy = std::move(result);

        REQUIRE(result_cpy.is_ok());
        REQUIRE_FALSE(result_cpy.is_err());

        REQUIRE_FALSE(result.is_ok());
        REQUIRE_FALSE(result.is_err());
    }
}

TEST_CASE("Result Construct Err", "[result]") {
    SECTION("Result<char, int> Err lvalue constrcut") {
        constexpr int a = 1;

        Err<int> err_int{a};
        Result<char, int> result{err_int};

        REQUIRE_FALSE(result.is_ok());
        REQUIRE(result.is_err());
    }
    SECTION("Result<char, int> Err rvalue construct") {
        constexpr int a = 1;

        Result<char, int> result{Err<int>{a}};

        REQUIRE_FALSE(result.is_ok());
        REQUIRE(result.is_err());
    }
    SECTION("Result<char, int> copy assignment") {
        constexpr int a = 1;

        Result<char, int> result_err{Err<int>{a}};
        Result<char, int> result_cpy = result_err;

        REQUIRE_FALSE(result_cpy.is_ok());
        REQUIRE(result_cpy.is_err());

        REQUIRE_FALSE(result_err.is_ok());
        REQUIRE(result_err.is_err());
    }
    SECTION("Result<char, int> move assignment") {
        constexpr int a = 1;

        Result<char, int> result{Err<int>{a}};
        Result<char, int> result_cpy = std::move(result);

        REQUIRE_FALSE(result_cpy.is_ok());
        REQUIRE(result_cpy.is_err());

        REQUIRE_FALSE(result.is_ok());
        REQUIRE_FALSE(result.is_err());
    }
    SECTION("Result<void, int> Err lvalue constrcut") {
        constexpr int a = 1;

        Err<int> err_int{a};
        Result<void, int> result{err_int};

        REQUIRE_FALSE(result.is_ok());
        REQUIRE(result.is_err());
    }
    SECTION("Result<void, int> Err rvalue construct") {
        constexpr int a = 1;

        Result<void, int> result{Err<int>{a}};

        REQUIRE_FALSE(result.is_ok());
        REQUIRE(result.is_err());
    }
    SECTION("Result<void, int> copy assignment") {
        constexpr int a = 1;

        Result<void, int> result{Err<int>{a}};
        Result<void, int> result_cpy = result;

        REQUIRE_FALSE(result_cpy.is_ok());
        REQUIRE(result_cpy.is_err());

        REQUIRE_FALSE(result.is_ok());
        REQUIRE(result.is_err());
    }
    SECTION("Result<void, int> move assignment") {
        constexpr int a = 1;

        Result<void, int> result{Err<int>{a}};
        Result<void, int> result_cpy = std::move(result);

        REQUIRE_FALSE(result_cpy.is_ok());
        REQUIRE(result_cpy.is_err());

        REQUIRE_FALSE(result.is_ok());
        REQUIRE_FALSE(result.is_err());
    }
}

TEST_CASE("Result and_then(...)", "[result]") {
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

TEST_CASE("Result map(...)", "[result]") {
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

TEST_CASE("Result match(...)", "[result]") {
    SECTION("Result<char, int>{Ok} match -> int") {
        constexpr char a = 'a';
        Ok<char> ok_char{a};
        Result<char, int> result{ok_char};

        REQUIRE(result.is_ok());

        SECTION("Result<char, int> match(char) -> int valid") {
            constexpr auto EXPECTED     = 100;
            constexpr auto NOT_EXPECTED = -100;

            auto ret = result.match(
                [ret = EXPECTED](char c) {
                    std::cout << "In good match with " << c << std::endl;
                    return ret;
                },
                [ret = NOT_EXPECTED](int) { return ret; });

            REQUIRE(ret == EXPECTED);
        }
    }
    SECTION("Result<void, int>{Ok} match -> int") {
        Result<void, int> result{Ok<void>{}};

        REQUIRE(result.is_ok());

        SECTION("Result<void, int> match(char) -> int valid") {
            constexpr auto EXPECTED     = 100;
            constexpr auto NOT_EXPECTED = -100;

            auto ret = result.match([ret = EXPECTED]() { return ret; }, [ret = NOT_EXPECTED](int) { return ret; });

            REQUIRE(ret == EXPECTED);
        }
    }
    SECTION("Result<char, int>{Err} match -> int") {
        constexpr int a = -1;
        Err<int> ok_char{a};
        Result<char, int> result{ok_char};

        REQUIRE(result.is_err());

        SECTION("Result<char, int> match(char) -> int") {
            constexpr auto NOT_EXPECTED = 100;
            constexpr auto EXPECTED     = -100;

            auto ret = result.match(
                [ret = NOT_EXPECTED](char c) {
                    std::cout << "In good match with " << c << std::endl;
                    return ret;
                },
                [ret = EXPECTED](int) { return ret; });

            REQUIRE(ret == EXPECTED);
        }
    }
    SECTION("Result<void, int>{Err} match -> int") {
        constexpr auto err = -1;
        Result<void, int> result{Err<int>{err}};

        REQUIRE(result.is_err());

        SECTION("Result<void, int> match(void) -> int ") {
            constexpr auto NOT_EXPECTED = 100;
            constexpr auto EXPECTED     = -100;

            auto ret = result.match([ret = NOT_EXPECTED]() { return ret; }, [ret = EXPECTED](int) { return ret; });

            REQUIRE(ret == EXPECTED);
        }
    }
}

TEST_CASE("Result Non-POD Types [result]") {
    // struct ResultStruct {
    //    int num;
    //    std::string str;
    //    std::vector<std::shared_ptr<int>> ptr_vec;
    //};
    //
    // struct ErrorStruct {
    //    std::shared_ptr<int> err;
    //    std::string error_msg;
    //};
    SECTION("Result<std::unique_ptr<ResultStruct>, ErrorStruct>>") {
        auto result_ptr = std::make_unique<ResultStruct>(
            10, "This is a good string",
            std::vector<std::shared_ptr<int>>{std::make_shared<int>(0), std::make_shared<int>(1),
                                              std::make_shared<int>(2), std::make_shared<int>(3),
                                              std::make_shared<int>(4), std::make_shared<int>(5)});
        Ok<std::unique_ptr<ResultStruct>> ok{std::move(result_ptr)};

        Result<std::unique_ptr<ResultStruct>, ErrorStruct> result{std::move(ok)};

        REQUIRE(result.is_ok());
    }
}

TEST_CASE("Result >> Operator") {
    SECTION("Result<char, int> Ok >> void func") {
        constexpr char a   = 'a';
        constexpr auto ret = 1;

        std::size_t counter = 0;

        Ok<char> ok_char{a};
        Result<char, int> result{ok_char};

        REQUIRE(result.is_ok());
        REQUIRE_FALSE(result.is_err());

        auto func = [a, &counter](char c) {
            REQUIRE(c == a);
            counter += 1;
        };

        REQUIRE(counter == 0);
        result >> func;
        REQUIRE(counter == ret);
    }
    SECTION("Result<char, int> Ok >> int func") {
        constexpr char a   = 'a';
        constexpr auto ret = 1;

        std::size_t counter = 0;

        Ok<char> ok_char{a};
        Result<char, int> result{ok_char};

        REQUIRE(result.is_ok());
        REQUIRE_FALSE(result.is_err());

        auto func = [a, &counter](char c) {
            REQUIRE(c == a);
            counter += 1;
            return static_cast<int>(c);
        };

        REQUIRE(counter == 0);
        auto val = result >> func;
        REQUIRE(counter == ret);
        REQUIRE(val == static_cast<int>(a));
    }
    SECTION("Result<void, int> Ok >> void func") {
        constexpr auto ret = 1;

        std::size_t counter = 0;

        Ok<void> ok_void{};
        Result<void, int> result{ok_void};

        REQUIRE(result.is_ok());
        REQUIRE_FALSE(result.is_err());

        auto func = [&counter]() { counter += 1; };

        REQUIRE(counter == 0);
        result >> func;
        REQUIRE(counter == ret);
    }
    SECTION("Result<void, int> Ok >> int func") {
        constexpr char a   = 'a';
        constexpr auto ret = 1;

        std::size_t counter = 0;

        Ok<void> ok_void{};
        Result<void, int> result{ok_void};

        REQUIRE(result.is_ok());
        REQUIRE_FALSE(result.is_err());

        auto func = [&counter]() {
            counter += 1;
            return static_cast<int>(a);
        };

        REQUIRE(counter == 0);
        auto val = result >> func;
        REQUIRE(counter == ret);
        REQUIRE(val == static_cast<int>(a));
    }
}

TEST_CASE("Result * Operator") {
    SECTION("Result<char, int> Ok >>") {
        constexpr char a = 'a';
        Ok<char> ok_char{a};

        Result<char, int> result{ok_char};
        REQUIRE(result.is_ok());
        REQUIRE(*result == a);
    }
}

}  // namespace
