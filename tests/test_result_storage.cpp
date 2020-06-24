#include <memory>
#include <string>

#include "catch2/catch.hpp"
#include "utils/result.hxx"

struct NotSoTrivial {
    std::string str;
    std::shared_ptr<int> not_trivial;
};

namespace {
using namespace cogle::utils::result;

TEST_CASE("ResultStorage Trivial Destruction [result][ResultStorage]") {
    SECTION("ResultStorage<char, int> Ok<char> copy creation") {
        constexpr auto test_char_a = 'a';

        Ok<char> ok{test_char_a};
        detail::ResultStorage<char, int> storage{ok};

        REQUIRE(storage.get_result() == test_char_a);
        REQUIRE(ok.get_result() == test_char_a);
    }
}

TEST_CASE("ResultStorage Non-Trivial Destruction [result][ResultStorage]") {
    SECTION("ResultStorage<NotSoTrivial, int> Ok<NotSoTrivial> copy creation") {
        auto int_ptr       = std::make_shared<int>(100);
        std::string phrase = "This is a test of template deduction";

        NotSoTrivial not_trivial{phrase, int_ptr};

        Ok<NotSoTrivial> ok{not_trivial};

        REQUIRE_FALSE(std::is_trivially_destructible<NotSoTrivial>::value);
        detail::ResultStorage<NotSoTrivial, int> storage{ok};

        auto& not_trivial_storage = storage.get_result();

        REQUIRE(not_trivial_storage.str == not_trivial.str);
        REQUIRE(not_trivial_storage.not_trivial == not_trivial.not_trivial);
        REQUIRE(*not_trivial_storage.not_trivial == *not_trivial.not_trivial);
    }
}

}  // namespace