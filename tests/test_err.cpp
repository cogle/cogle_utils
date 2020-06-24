#include "catch2/catch.hpp"
#include "utils/result.hxx"

namespace {

using namespace cogle::utils::result;

TEST_CASE("Err Struct Primitive Types", "[result][err]") {
    // Tests with the primitive types
    SECTION("Err Struct char") {
        constexpr char err_char_val = 'E';
        Err<char> err_char{err_char_val};
        REQUIRE(err_char.get_error() == err_char_val);
    }

    SECTION("Err Struct short") {
        constexpr short err_short_val = -2031;
        Err<short> err_short{err_short_val};
        REQUIRE(err_short.get_error() == err_short_val);
    }

    SECTION("Err Struct int") {
        constexpr int err_int_val = 93011;
        Err<int> err_int{err_int_val};
        REQUIRE(err_int.get_error() == err_int_val);
    }

    SECTION("Err Struct long") {
        constexpr long err_long_val = -5001132;
        Err<long> err_long{err_long_val};
        REQUIRE(err_long.get_error() == err_long_val);
    }

    SECTION("Err Struct float") {
        constexpr float err_float_val = 430.50131f;
        Err<float> err_float{err_float_val};
        REQUIRE(err_float.get_error() == err_float_val);
    }

    SECTION("Err Struct double") {
        constexpr double err_double_val = 851045.10021;
        Err<double> err_double{err_double_val};
        REQUIRE(err_double.get_error() == err_double_val);
    }
}

TEST_CASE("Err Struct Primitive Types constexpr", "[result][err]") {
    // Tests with the primitive types
    SECTION("Err Struct char") {
        constexpr char err_char_val = 'E';
        constexpr Err<char> err_char{err_char_val};
        static_assert(err_char.get_error() == err_char_val);

        REQUIRE(err_char.get_error() == err_char_val);
    }

    SECTION("Err Struct short") {
        constexpr short err_short_val = -2031;
        constexpr Err<short> err_short{err_short_val};
        static_assert(err_short.get_error() == err_short_val);

        REQUIRE(err_short.get_error() == err_short_val);
    }

    SECTION("Err Struct int") {
        constexpr int err_int_val = 93011;
        constexpr Err<int> err_int{err_int_val};
        static_assert(err_int.get_error() == err_int_val);

        REQUIRE(err_int.get_error() == err_int_val);
    }

    SECTION("Err Struct long") {
        constexpr long err_long_val = -5001132;
        constexpr Err<long> err_long{err_long_val};
        static_assert(err_long.get_error() == err_long_val);

        REQUIRE(err_long.get_error() == err_long_val);
    }

    SECTION("Err Struct float") {
        constexpr float err_float_val = 430.50131f;
        constexpr Err<float> err_float{err_float_val};
        static_assert(err_float.get_error() == err_float_val);

        REQUIRE(err_float.get_error() == err_float_val);
    }

    SECTION("Err Struct double") {
        constexpr double err_double_val = 851045.10021;
        constexpr Err<double> err_double{err_double_val};
        static_assert(err_double.get_error() == err_double_val);

        REQUIRE(err_double.get_error() == err_double_val);
    }
}

TEST_CASE("Err Struct Equality and Inequality Primitive Types", "[result][err]") {
    SECTION("Err struct char Equality and Inequality") {
        constexpr char err_char_val_a = 'a';
        constexpr char err_char_val_z = 'z';

        SECTION("Sanity Check") { REQUIRE(err_char_val_a != err_char_val_z); }

        Err<char> err_char_a{err_char_val_a};
        SECTION("Assert Equality of result with value") { REQUIRE(err_char_a.get_error() == err_char_val_a); }

        Err<char> err_char_z{err_char_val_z};
        SECTION("Assert Equality of result with value") { REQUIRE(err_char_z.get_error() == err_char_val_z); }

        SECTION("Assert inequality of two different valued Err containers") { REQUIRE(err_char_a != err_char_z); }

        Err<char> err_char_a_dup{err_char_val_a};
        SECTION("Assert Equality of two same valued Ok containers") {
            REQUIRE(err_char_a_dup.get_error() == err_char_val_a);
            REQUIRE(err_char_a == err_char_a_dup);
        }

        SECTION("Assert Err and Okcontainer inequality") {
            Ok<char> ok_char_a{err_char_val_a};
            Ok<char> ok_char_z{err_char_val_z};

            REQUIRE_FALSE(err_char_a == ok_char_a);
            REQUIRE(err_char_a != ok_char_a);

            REQUIRE_FALSE(err_char_z == ok_char_z);
            REQUIRE(err_char_z != ok_char_z);
        }
    }
}
}  // namespace