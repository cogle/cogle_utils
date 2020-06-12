#include "catch2/catch.hpp"
#include "utils/result.hxx"

namespace {

using namespace cogle::utils::result;

TEST_CASE("Ok Struct is well formed", "[result][ok]") {
    constexpr int ok_int_val = 9;

    // Tests with the basic types
    Ok<int> ok_int{ok_int_val};
    REQUIRE(ok_int.get_result() == ok_int_val); 
}

}
