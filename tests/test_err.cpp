#include "catch2/catch.hpp"
#include "utils/result.hxx"

namespace {

using namespace cogle::utils::result;

TEST_CASE("Err Struct Primitive Types", "[result][err]") {
    constexpr char err_char_val = 'C';
    constexpr short err_short_val = -2031;
    constexpr int err_int_val = 93011;
    constexpr long err_long_val = -5001132;
    constexpr float err_float_val = 430.50131;
    constexpr double err_double_val = 851045.10021;

    // Tests with the primitive types
    SECTION("Err Struct char") {
        Err<char> err_char{err_char_val};
        REQUIRE(err_char.get_error() == err_char_val);
    }

    SECTION("Err Struct short") {
        Err<short> err_short{err_short_val};
        REQUIRE(err_short.get_error() == err_short_val);
    }

    SECTION("Err Struct int") {
        Err<int> err_int{err_int_val};
        REQUIRE(err_int.get_error() == err_int_val);
    }

    SECTION("Err Struct long") {
        Err<long> err_long{err_long_val};
        REQUIRE(err_long.get_error() == err_long_val);
    }

    SECTION("Err Struct float") {
        Err<float> err_float{err_float_val};
        REQUIRE(err_float.get_error() == err_float_val);
    }

    SECTION("Err Struct double") {
        Err<double> err_double{err_double_val};
        REQUIRE(err_double.get_error() == err_double_val);
    }
}

}  // namespace