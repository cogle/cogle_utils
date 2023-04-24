#include <memory>

#include "catch2/catch_test_macros.hpp"
#include "utils/result.hxx"

namespace {

using namespace cogle::utils::result;

TEST_CASE("Ok Struct Primitive Types", "[result][ok]") {
    // Tests with the primitive types

    SECTION("Ok Struct char") {
        constexpr char ok_char_val = 'C';
        Ok<char> ok_char{ok_char_val};
        REQUIRE(ok_char.get_result() == ok_char_val);
    }

    SECTION("Ok struct short") {
        constexpr short ok_short_val = -2031;
        Ok<short> ok_short{ok_short_val};
        REQUIRE(ok_short.get_result() == ok_short_val);
    }

    SECTION("Ok struct int") {
        constexpr int ok_int_val = 93011;
        Ok<int> ok_int{ok_int_val};
        REQUIRE(ok_int.get_result() == ok_int_val);
    }

    SECTION("Ok struct long") {
        constexpr long ok_long_val = -5001132;
        Ok<long> ok_long{ok_long_val};
        REQUIRE(ok_long.get_result() == ok_long_val);
    }

    SECTION("Ok struct float") {
        constexpr float ok_float_val = 430.50131f;
        Ok<float> ok_float{ok_float_val};
        REQUIRE(ok_float.get_result() == ok_float_val);
    }

    SECTION("Ok struct double") {
        constexpr double ok_double_val = 851045.10021;
        Ok<double> ok_double{ok_double_val};
        REQUIRE(ok_double.get_result() == ok_double_val);
    }
}

TEST_CASE("Ok Struct Primitive Types constexpr", "[result][ok]") {
    // Tests with the primitive types

    SECTION("Ok Struct char") {
        constexpr char ok_char_val = 'C';
        constexpr Ok<char> ok_char{ok_char_val};
        static_assert(ok_char.get_result() == ok_char_val);

        REQUIRE(ok_char.get_result() == ok_char_val);
    }

    SECTION("Ok struct short") {
        constexpr short ok_short_val = -2031;
        constexpr Ok<short> ok_short{ok_short_val};
        static_assert(ok_short.get_result() == ok_short_val);

        REQUIRE(ok_short.get_result() == ok_short_val);
    }

    SECTION("Ok struct int") {
        constexpr int ok_int_val = 93011;
        constexpr Ok<int> ok_int{ok_int_val};
        static_assert(ok_int.get_result() == ok_int_val);

        REQUIRE(ok_int.get_result() == ok_int_val);
    }

    SECTION("Ok struct long") {
        constexpr long ok_long_val = -5001132;
        constexpr Ok<long> ok_long{ok_long_val};
        static_assert(ok_long.get_result() == ok_long_val);

        REQUIRE(ok_long.get_result() == ok_long_val);
    }

    SECTION("Ok struct float") {
        constexpr float ok_float_val = 430.50131f;
        constexpr Ok<float> ok_float{ok_float_val};
        static_assert(ok_float.get_result() == ok_float_val);

        REQUIRE(ok_float.get_result() == ok_float_val);
    }

    SECTION("Ok struct double") {
        constexpr double ok_double_val = 851045.10021;
        constexpr Ok<double> ok_double{ok_double_val};
        static_assert(ok_double.get_result() == ok_double_val);

        REQUIRE(ok_double.get_result() == ok_double_val);
    }
}

TEST_CASE("Ok Struct Equality and Inequality Primitive Types", "[result][ok]") {
    SECTION("Ok struct char Equality and Inequality") {
        constexpr char ok_char_val_a = 'a';
        constexpr char ok_char_val_z = 'z';

        SECTION("Sanity Check") { REQUIRE(ok_char_val_a != ok_char_val_z); }

        Ok<char> ok_char_a{ok_char_val_a};
        SECTION("Assert Equality of result with value") { REQUIRE(ok_char_a.get_result() == ok_char_val_a); }

        Ok<char> ok_char_z{ok_char_val_z};
        SECTION("Assert Equality of result with value") { REQUIRE(ok_char_z.get_result() == ok_char_val_z); }

        SECTION("Assert inequality of two different valued Ok containers") { REQUIRE(ok_char_a != ok_char_z); }

        Ok<char> ok_char_a_dup{ok_char_val_a};
        SECTION("Assert Equality of two same valued Ok containers") {
            REQUIRE(ok_char_a_dup.get_result() == ok_char_val_a);
            REQUIRE(ok_char_a == ok_char_a_dup);
        }

        SECTION("Assert Ok and Err container inequality") {
            Err<char> err_char_a{ok_char_val_a};
            Err<char> err_char_z{ok_char_val_z};

            REQUIRE_FALSE(ok_char_a == err_char_a);
            REQUIRE((ok_char_a != err_char_a));

            REQUIRE_FALSE(ok_char_z == err_char_z);
            REQUIRE((ok_char_z != err_char_z));
        }
    }
}

TEST_CASE("Ok NonPod Types", "[result][ok]") {
    SECTION("std::unique_ptr<...> Test") {
        constexpr auto ptr_value = 100;
        auto ptr                 = std::make_unique<int>(ptr_value);
        Ok<std::unique_ptr<int>> ok{std::move(ptr)};

        REQUIRE(*ok.get_result() == ptr_value);
    }
}

}  // namespace
