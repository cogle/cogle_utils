#include <memory>
#include <string>

#include "catch2/catch.hpp"
#include "utils/result.hxx"

struct NotSoTrivial {
    NotSoTrivial(std::string s, std::shared_ptr<int> p) : str(s), not_trivial(p) {}

    std::string str;
    std::shared_ptr<int> not_trivial;
};

namespace {
using namespace cogle::utils::result;

TEST_CASE("ResultStorage Trivial Destruction [result][ResultStorage]") {
    SECTION("ResultStorage<char, int> Ok<char> lvalue creation") {
        constexpr auto test_char_a = 'a';

        Ok<char> ok{test_char_a};
        detail::ResultStorage<char, int> storage{ok};

        REQUIRE(storage.get_tag() == detail::ResultTag::OK);
        REQUIRE(storage.get_result() == test_char_a);
        REQUIRE(ok.get_result() == test_char_a);
    }
    SECTION("ResultStorage<char, int> Ok<char> rvalue creation") {
        constexpr auto test_char_a = 'a';

        Ok<char> ok{test_char_a};
        detail::ResultStorage<char, int> storage{std::move(ok)};

        REQUIRE(storage.get_tag() == detail::ResultTag::OK);
        REQUIRE(storage.get_result() == test_char_a);
    }
    SECTION("ResultStorage<char, int> Err<int> lvalue creation") {
        constexpr auto test_int = 1000;

        Err<int> err{test_int};
        detail::ResultStorage<char, int> storage{err};

        REQUIRE(storage.get_tag() == detail::ResultTag::ERR);
        REQUIRE(storage.get_error() == test_int);
        REQUIRE(err.get_error() == test_int);
    }
    SECTION("ResultStorage<char, int> Err<int> rvalue creation") {
        constexpr auto test_int = 100;

        Err<int> err{test_int};
        detail::ResultStorage<char, int> storage{std::move(err)};

        REQUIRE(storage.get_tag() == detail::ResultTag::ERR);
        REQUIRE(storage.get_error() == test_int);
    }
    SECTION("ResultStorage<char, int> copy constructor creation [Ok]") {
        constexpr auto test_char_a = 'a';

        Ok<char> ok{test_char_a};

        detail::ResultStorage<char, int> storage{ok};
        detail::ResultStorage<char, int> storage_cpy{storage};

        REQUIRE(storage.get_tag() == detail::ResultTag::OK);
        REQUIRE(storage.get_result() == test_char_a);

        REQUIRE(storage_cpy.get_tag() == detail::ResultTag::OK);
        REQUIRE(storage_cpy.get_result() == test_char_a);
    }
    SECTION("ResultStorage<char, int> copy constructor creation [Err]") {
        constexpr auto test_int = 100;

        Err<int> err{test_int};

        detail::ResultStorage<char, int> storage{err};
        detail::ResultStorage<char, int> storage_cpy{storage};

        REQUIRE(storage.get_tag() == detail::ResultTag::ERR);
        REQUIRE(storage.get_error() == test_int);

        REQUIRE(storage_cpy.get_tag() == detail::ResultTag::ERR);
        REQUIRE(storage_cpy.get_error() == test_int);
    }
    SECTION("ResultStorage<char, int> move constructor creation [Ok]") {
        constexpr auto test_char_a = 'a';

        Ok<char> ok{test_char_a};

        detail::ResultStorage<char, int> storage{ok};
        detail::ResultStorage<char, int> storage_mv{std::move(storage)};

        REQUIRE(storage.get_tag() == detail::ResultTag::INVALID);

        REQUIRE(storage_mv.get_tag() == detail::ResultTag::OK);
        REQUIRE(storage_mv.get_result() == test_char_a);
    }
    SECTION("ResultStorage<char, int> move constructor creation [Err]") {
        constexpr auto test_int = 100;

        Err<int> err{test_int};

        detail::ResultStorage<char, int> storage{err};
        detail::ResultStorage<char, int> storage_mv{std::move(storage)};

        REQUIRE(storage.get_tag() == detail::ResultTag::INVALID);

        REQUIRE(storage_mv.get_tag() == detail::ResultTag::ERR);
        REQUIRE(storage_mv.get_error() == test_int);
    }
    SECTION("ResultStorage<char, int> copy assignment operator creation [Ok]") {
        constexpr auto test_char_a = 'a';

        Ok<char> ok{test_char_a};

        detail::ResultStorage<char, int> storage{ok};
        detail::ResultStorage<char, int> storage_cpy = storage;

        REQUIRE(storage.get_tag() == detail::ResultTag::OK);
        REQUIRE(storage.get_result() == test_char_a);

        REQUIRE(storage_cpy.get_tag() == detail::ResultTag::OK);
        REQUIRE(storage_cpy.get_result() == test_char_a);
    }
    SECTION("ResultStorage<char, int> copy assignment operator creation [Err]") {
        constexpr auto test_int = 100;

        Err<int> err{test_int};

        detail::ResultStorage<char, int> storage{err};
        detail::ResultStorage<char, int> storage_cpy = storage;

        REQUIRE(storage.get_tag() == detail::ResultTag::ERR);
        REQUIRE(storage.get_error() == test_int);

        REQUIRE(storage_cpy.get_tag() == detail::ResultTag::ERR);
        REQUIRE(storage_cpy.get_error() == test_int);
    }
    SECTION("ResultStorage<char, int> move assignment operator creation [Ok]") {
        constexpr auto test_char_a = 'a';

        Ok<char> ok{test_char_a};

        detail::ResultStorage<char, int> storage{ok};
        detail::ResultStorage<char, int> storage_mv = std::move(storage);

        REQUIRE(storage.get_tag() == detail::ResultTag::INVALID);

        REQUIRE(storage_mv.get_tag() == detail::ResultTag::OK);
        REQUIRE(storage_mv.get_result() == test_char_a);
    }
    SECTION("ResultStorage<char, int> move assignment operator creation [Err]") {
        constexpr auto test_int = 100;

        Err<int> err{test_int};

        detail::ResultStorage<char, int> storage{err};
        detail::ResultStorage<char, int> storage_mv = std::move(storage);

        REQUIRE(storage.get_tag() == detail::ResultTag::INVALID);

        REQUIRE(storage_mv.get_tag() == detail::ResultTag::ERR);
        REQUIRE(storage_mv.get_error() == test_int);
    }
}

TEST_CASE("ResultStorage Non-Trivial Destruction [result][ResultStorage]") {
    SECTION("ResultStorage<NotSoTrivial, int> Ok<NotSoTrivial> lvalue creation") {
        auto int_ptr             = std::make_shared<int>(100);
        const std::string phrase = "This is a test of template deduction";

        NotSoTrivial not_trivial{phrase, int_ptr};

        Ok<NotSoTrivial> ok{not_trivial};

        REQUIRE_FALSE(std::is_trivially_destructible<NotSoTrivial>::value);
        detail::ResultStorage<NotSoTrivial, int> storage{ok};

        auto& not_trivial_storage = storage.get_result();

        REQUIRE(storage.get_tag() == detail::ResultTag::OK);
        REQUIRE(not_trivial_storage.str == not_trivial.str);
        REQUIRE(not_trivial_storage.not_trivial == not_trivial.not_trivial);
        REQUIRE(*not_trivial_storage.not_trivial == *not_trivial.not_trivial);
    }
    SECTION("ResultStorage<NotSoTrivial, int> Ok<NotSoTrivial> rvalue creation") {
        auto int_ptr             = std::make_shared<int>(100);
        const std::string phrase = "This is a test of template deduction";

        NotSoTrivial not_trivial{phrase, int_ptr};

        Ok<NotSoTrivial> ok{not_trivial};

        REQUIRE_FALSE(std::is_trivially_destructible<NotSoTrivial>::value);
        detail::ResultStorage<NotSoTrivial, int> storage{std::move(ok)};

        auto& not_trivial_storage = storage.get_result();

        REQUIRE(storage.get_tag() == detail::ResultTag::OK);
        REQUIRE(not_trivial_storage.str == not_trivial.str);
        REQUIRE(*not_trivial_storage.not_trivial == *int_ptr);
    }
    SECTION("ResultStorage<int, NotSoTrivial> Err<NotSoTrivial> lvalue creation") {
        auto int_ptr             = std::make_shared<int>(100);
        const std::string phrase = "This is a test of template deduction";

        NotSoTrivial not_trivial{phrase, int_ptr};

        Err<NotSoTrivial> err{not_trivial};

        REQUIRE_FALSE(std::is_trivially_destructible<NotSoTrivial>::value);
        detail::ResultStorage<int, NotSoTrivial> storage{err};

        auto& not_trivial_storage = storage.get_error();

        REQUIRE(storage.get_tag() == detail::ResultTag::ERR);
        REQUIRE(not_trivial_storage.str == not_trivial.str);
        REQUIRE(not_trivial_storage.not_trivial == not_trivial.not_trivial);
        REQUIRE(*not_trivial_storage.not_trivial == *not_trivial.not_trivial);
    }
    SECTION("ResultStorage<int, NotSoTrivial> Err<NotSoTrivial> rvalue creation") {
        auto int_ptr             = std::make_shared<int>(100);
        const std::string phrase = "This is a test of template deduction";

        NotSoTrivial not_trivial{phrase, int_ptr};

        Err<NotSoTrivial> err{not_trivial};

        REQUIRE_FALSE(std::is_trivially_destructible<NotSoTrivial>::value);
        detail::ResultStorage<int, NotSoTrivial> storage{std::move(err)};

        auto& not_trivial_storage = storage.get_error();

        REQUIRE(storage.get_tag() == detail::ResultTag::ERR);
        REQUIRE(not_trivial_storage.str == not_trivial.str);
        REQUIRE(*not_trivial_storage.not_trivial == *int_ptr);
    }
    SECTION("ResultStorage<NotSoTrivial, int> copy creation [Ok<NotSoTrivial>]") {
        auto int_ptr             = std::make_shared<int>(100);
        const std::string phrase = "This is a test of template deduction";

        NotSoTrivial not_trivial{phrase, int_ptr};

        Ok<NotSoTrivial> ok{not_trivial};

        REQUIRE_FALSE(std::is_trivially_destructible<NotSoTrivial>::value);

        detail::ResultStorage<NotSoTrivial, int> storage{ok};
        detail::ResultStorage<NotSoTrivial, int> storage_cpy{storage};

        auto& not_trivial_storage = storage.get_result();

        REQUIRE(storage.get_tag() == detail::ResultTag::OK);
        REQUIRE(not_trivial_storage.str == not_trivial.str);
        REQUIRE(not_trivial_storage.not_trivial == not_trivial.not_trivial);
        REQUIRE(*not_trivial_storage.not_trivial == *not_trivial.not_trivial);

        auto& not_trivial_storage_cpy = storage_cpy.get_result();

        REQUIRE(storage_cpy.get_tag() == detail::ResultTag::OK);

        REQUIRE(not_trivial_storage_cpy.str == not_trivial.str);
        REQUIRE(not_trivial_storage_cpy.not_trivial == not_trivial.not_trivial);
        REQUIRE(*not_trivial_storage_cpy.not_trivial == *not_trivial.not_trivial);
    }
    SECTION("ResultStorage<NotSoTrivial, int> copy creation [Err<int>]") {
        auto err_val = 100;
        Err<int> err{err_val};

        detail::ResultStorage<NotSoTrivial, int> storage{err};
        detail::ResultStorage<NotSoTrivial, int> storage_cpy{storage};

        REQUIRE(storage.get_tag() == detail::ResultTag::ERR);
        REQUIRE(storage.get_error() == err_val);

        REQUIRE(storage_cpy.get_tag() == detail::ResultTag::ERR);
        REQUIRE(storage_cpy.get_error() == err_val);
    }
    SECTION("ResultStorage<int, NotSoTrivial> copy creation [Ok<int>]") {
        auto ok_val = 100;
        Ok<int> ok{ok_val};

        detail::ResultStorage<int, NotSoTrivial> storage{ok};
        detail::ResultStorage<int, NotSoTrivial> storage_cpy{storage};

        REQUIRE(storage.get_tag() == detail::ResultTag::OK);
        REQUIRE(storage.get_result() == ok_val);

        REQUIRE(storage_cpy.get_tag() == detail::ResultTag::OK);
        REQUIRE(storage_cpy.get_result() == ok_val);
    }
    SECTION("ResultStorage<NotSoTrivial, int> copy creation [Err<NotSoTrivial>]") {
        auto int_ptr             = std::make_shared<int>(100);
        const std::string phrase = "This is a test of template deduction";

        NotSoTrivial not_trivial{phrase, int_ptr};

        Err<NotSoTrivial> err{not_trivial};

        REQUIRE_FALSE(std::is_trivially_destructible<NotSoTrivial>::value);

        detail::ResultStorage<int, NotSoTrivial> storage{err};
        detail::ResultStorage<int, NotSoTrivial> storage_cpy{storage};

        auto& not_trivial_storage = storage.get_error();

        REQUIRE(storage.get_tag() == detail::ResultTag::ERR);
        REQUIRE(not_trivial_storage.str == not_trivial.str);
        REQUIRE(not_trivial_storage.not_trivial == not_trivial.not_trivial);
        REQUIRE(*not_trivial_storage.not_trivial == *not_trivial.not_trivial);

        auto& not_trivial_storage_cpy = storage_cpy.get_error();

        REQUIRE(storage_cpy.get_tag() == detail::ResultTag::ERR);

        REQUIRE(not_trivial_storage_cpy.str == not_trivial.str);
        REQUIRE(not_trivial_storage_cpy.not_trivial == not_trivial.not_trivial);
        REQUIRE(*not_trivial_storage_cpy.not_trivial == *not_trivial.not_trivial);
    }
}

TEST_CASE("ResultStorage Trivial Destruction [result][ResultStorage] void Result specialization") {
    SECTION("ResultStorage<void, int> Ok<void> copy creation") {
        Ok<void> ok{};
        detail::ResultStorage<void, int> storage{ok};

        REQUIRE(storage.get_tag() == detail::ResultTag::OK);
    }
}

// TODO MORE TESTS HERE

}  // namespace