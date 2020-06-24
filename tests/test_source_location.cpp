#include <sstream>
#include <string>

#include "catch2/catch.hpp"
#include "utils/result.hxx"

namespace {
using namespace cogle::utils::location;

TEST_CASE("SourceLocation Tests [SourceLocation]") {
    SECTION("SourceLocation Default Construction") {
        SourceLocation loc{};

        REQUIRE(loc.line() == 0);
        REQUIRE(loc.column() == 0);

        // TODO not having string here fails in Catch2 only on clang
        REQUIRE(std::string(loc.file_name()) == std::string(detail::DEFAULT_LOC_CHAR_ARR_VAL));
        REQUIRE(std::string(loc.function_name()) == std::string(detail::DEFAULT_LOC_CHAR_ARR_VAL));

        std::ostringstream expected_os{};
        expected_os << "File: " << detail::DEFAULT_LOC_CHAR_ARR_VAL << "\tFunc: " << detail::DEFAULT_LOC_CHAR_ARR_VAL
                    << "\tLine: " << 0 << "\tCol: " << 0;

        std::ostringstream out{};
        out << loc;

        REQUIRE(expected_os.str() == out.str());
    }

    SECTION("SourceLocation current() creation") {
        auto file    = __builtin_FILE();
        auto func    = __builtin_FUNCTION();
        auto line    = __builtin_LINE() + 1;  // If you move this line you better move ::current()
        auto current = SourceLocation::current();

        REQUIRE(current.line() == line);
        REQUIRE(current.column() == 0);

        // TODO not having string here fails in Catch2 only on clang
        REQUIRE(std::string(current.file_name()) == std::string(file));
        REQUIRE(std::string(current.function_name()) == std::string(func));

        std::ostringstream expected_os{};
        expected_os << "File: " << file << "\tFunc: " << func << "\tLine: " << line << "\tCol: " << 0;

        std::ostringstream out{};
        out << current;

        REQUIRE(expected_os.str() == out.str());
    }
}

}  // namespace