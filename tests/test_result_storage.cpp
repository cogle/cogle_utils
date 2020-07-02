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
    SECTION("ResultStorage<char, int> Ok<char> move creation") {
        constexpr auto test_char_a = 'a';

        Ok<char> ok{test_char_a};
        detail::ResultStorage<char, int> storage{std::move(ok)};

        REQUIRE(storage.get_result() == test_char_a);
    }
    SECTION("ResultStorage<char, int> Err<int> copy creation") {
        constexpr auto test_int = 1000;

        Err<int> err{test_int};
        detail::ResultStorage<char, int> storage{err};

        REQUIRE(storage.get_error() == test_int);
        REQUIRE(err.get_error() == test_int);
    }
    SECTION("ResultStorage<char, int> Err<int> move creation") {
        constexpr auto test_int = 100;

        Err<int> err{test_int};
        detail::ResultStorage<char, int> storage{std::move(err)};

        REQUIRE(storage.get_error() == test_int);
    }
}

TEST_CASE("ResultStorage Non-Trivial Destruction [result][ResultStorage]") {
    SECTION("ResultStorage<NotSoTrivial, int> Ok<NotSoTrivial> copy creation") {
        auto int_ptr             = std::make_shared<int>(100);
        const std::string phrase = "This is a test of template deduction";

        NotSoTrivial not_trivial{phrase, int_ptr};

        Ok<NotSoTrivial> ok{not_trivial};

        REQUIRE_FALSE(std::is_trivially_destructible<NotSoTrivial>::value);
        detail::ResultStorage<NotSoTrivial, int> storage{ok};

        auto& not_trivial_storage = storage.get_result();

        REQUIRE(not_trivial_storage.str == not_trivial.str);
        REQUIRE(not_trivial_storage.not_trivial == not_trivial.not_trivial);
        REQUIRE(*not_trivial_storage.not_trivial == *not_trivial.not_trivial);
    }
    SECTION("ResultStorage<NotSoTrivial, int> Ok<NotSoTrivial> move creation") {
        auto int_ptr             = std::make_shared<int>(100);
        const std::string phrase = "This is a test of template deduction";

        NotSoTrivial not_trivial{phrase, int_ptr};

        Ok<NotSoTrivial> ok{not_trivial};

        REQUIRE_FALSE(std::is_trivially_destructible<NotSoTrivial>::value);
        detail::ResultStorage<NotSoTrivial, int> storage{std::move(ok)};

        auto& not_trivial_storage = storage.get_result();

        REQUIRE(not_trivial_storage.str == not_trivial.str);
        REQUIRE(*not_trivial_storage.not_trivial == *int_ptr);
    }
    SECTION("ResultStorage<int, NotSoTrivial> Err<NotSoTrivial> copy creation") {
        auto int_ptr             = std::make_shared<int>(100);
        const std::string phrase = "This is a test of template deduction";

        NotSoTrivial not_trivial{phrase, int_ptr};

        Err<NotSoTrivial> err{not_trivial};

        REQUIRE_FALSE(std::is_trivially_destructible<NotSoTrivial>::value);
        detail::ResultStorage<int, NotSoTrivial> storage{err};

        auto& not_trivial_storage = storage.get_error();

        REQUIRE(not_trivial_storage.str == not_trivial.str);
        REQUIRE(not_trivial_storage.not_trivial == not_trivial.not_trivial);
        REQUIRE(*not_trivial_storage.not_trivial == *not_trivial.not_trivial);
    }
    SECTION("ResultStorage<int, NotSoTrivial> Err<NotSoTrivial> move creation") {
        auto int_ptr             = std::make_shared<int>(100);
        const std::string phrase = "This is a test of template deduction";

        NotSoTrivial not_trivial{phrase, int_ptr};

        Err<NotSoTrivial> err{not_trivial};

        REQUIRE_FALSE(std::is_trivially_destructible<NotSoTrivial>::value);
        detail::ResultStorage<int, NotSoTrivial> storage{std::move(err)};

        auto& not_trivial_storage = storage.get_error();

        REQUIRE(not_trivial_storage.str == not_trivial.str);
        REQUIRE(*not_trivial_storage.not_trivial == *int_ptr);
    }
}

}  // namespace