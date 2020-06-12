#include "catch2/catch.hpp"
#include "utils/result.hxx"

namespace {

using namespace cogle::utils::result;

/*
unsigned char 	at least 1 byte 	0 to 255
short 	at least 2 bytes 	-32768 to 32767
unsigned short 	at least 2 bytes 	0 to 65535
int 	at least 2 bytes 	-32768 to 32767
unsigned int 	at least 2 bytes 	0 to 65535
long 	at least 4 bytes 	-2,147,483,648 to 2,147,483,647
unsigned long 	at least 4 bytes 	0 to 4,294,967,295
float 	at least 2 bytes 	3.4e-038 to 3.4e+038
double 	at least 8 bytes 	1.7e-308 to 1.7e+308
long double 	at least 10 bytes 	1.7e-4932 to 1.7e+4932
*/

TEST_CASE("Ok Struct Primitive Types", "[result][ok]") {
    constexpr char ok_char_val = 'a';
    constexpr int ok_int_val = 9;

    // Tests with the primitive types
    Ok<char> ok_char{ok_char_val};
    REQUIRE(ok_char.get_result() == ok_char_val);

    Ok<int> ok_int{ok_int_val};
    REQUIRE(ok_int.get_result() == ok_int_val);
}

}  // namespace
