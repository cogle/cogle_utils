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

}  // namespace