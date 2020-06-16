#include "catch2/catch.hpp"
#include "utils/result.hxx"

namespace {

using namespace cogle::utils::result;

TEST_CASE("Ok Struct Primitive Types", "[result][ok]") {
    constexpr char ok_char_val = 'C';
    constexpr short ok_short_val = -2031;
    constexpr int ok_int_val = 93011;
    constexpr long ok_long_val = -5001132;
    constexpr float ok_float_val = 430.50131;
    constexpr double ok_double_val = 851045.10021;

    // Tests with the primitive types
    Ok<char> ok_char{ok_char_val};
    REQUIRE(ok_char.get_result() == ok_char_val);

    Ok<short> ok_short{ok_short_val};
    REQUIRE(ok_short.get_result() == ok_short_val);

    Ok<int> ok_int{ok_int_val};
    REQUIRE(ok_int.get_result() == ok_int_val);

    Ok<long> ok_long{ok_long_val};
    REQUIRE(ok_long.get_result() == ok_long_val);

    Ok<float> ok_float{ok_float_val};
    REQUIRE(ok_float.get_result() == ok_float_val);

    Ok<double> ok_double{ok_double_val};
    REQUIRE(ok_double.get_result() == ok_double_val);
}

TEST_CASE("Ok Struct Equality and Inequality Primitive Types", "[result][ok]") {
    constexpr char ok_char_val_a = 'a';
    constexpr char ok_char_val_z = 'z';

    Ok<char> ok_char_a{ok_char_val_a};
    REQUIRE(ok_char_a.get_result() == ok_char_val_a);

    Ok<char> ok_char_z{ok_char_val_z};
    REQUIRE(ok_char_z.get_result() == ok_char_val_z);

    REQUIRE(ok_char_a != ok_char_z);

    Ok<char> ok_char_a_dup{ok_char_val_a};
    REQUIRE(ok_char_a_dup.get_result() == ok_char_val_a);

    REQUIRE(ok_char_a == ok_char_a_dup);
}

}  // namespace
