#include <algorithm>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "catch2/catch.hpp"
#include "utils/result.hxx"

namespace {

struct NotSoTrivial {
    NotSoTrivial(std::string s, std::shared_ptr<int> p) : str(s), int_ptr(p) {}

    std::string str;
    std::shared_ptr<int> int_ptr;
};

// TODO tests with this more complex type
struct AnotherNotSoTrivial {
    AnotherNotSoTrivial(NotSoTrivial n, std::unordered_map<int, std::string> n_m, std::vector<int> n_v)
        : not_trivial(n), num_map(n_m), num_vec(n_v) {}

    NotSoTrivial not_trivial;
    std::unordered_map<int, std::string> num_map;
    std::vector<int> num_vec;
};

using namespace cogle::utils::result;

TEST_CASE("ResultStorage Trivial Destruction [result][ResultStorage]") {
    SECTION("ResultStorage<char, int> Ok<char> lvalue construction") {
        constexpr auto test_char_a = 'a';

        Ok<char> ok{test_char_a};
        detail::ResultStorage<char, int> storage{ok};

        REQUIRE(storage.get_tag() == detail::ResultTag::OK);
        REQUIRE(storage.get_result() == test_char_a);
        REQUIRE(ok.get_result() == test_char_a);

        const auto& val = storage.get_result();
        REQUIRE(val == test_char_a);
    }
    SECTION("ResultStorage<char, int> Ok<char> rvalue construction") {
        constexpr auto test_char_a = 'a';

        Ok<char> ok{test_char_a};
        detail::ResultStorage<char, int> storage{std::move(ok)};

        REQUIRE(storage.get_tag() == detail::ResultTag::OK);
        REQUIRE(storage.get_result() == test_char_a);
    }
    SECTION("ResultStorage<char, int> Err<int> lvalue construction") {
        constexpr auto test_int = 1000;

        Err<int> err{test_int};
        detail::ResultStorage<char, int> storage{err};

        REQUIRE(storage.get_tag() == detail::ResultTag::ERR);
        REQUIRE(storage.get_error() == test_int);
        REQUIRE(err.get_error() == test_int);

        const auto& val = storage.get_error();
        REQUIRE(val == test_int);
    }
    SECTION("ResultStorage<char, int> Err<int> rvalue construction") {
        constexpr auto test_int = 100;

        Err<int> err{test_int};
        detail::ResultStorage<char, int> storage{std::move(err)};

        REQUIRE(storage.get_tag() == detail::ResultTag::ERR);
        REQUIRE(storage.get_error() == test_int);
    }
    SECTION("ResultStorage<char, int> copy constructor construction [Ok]") {
        constexpr auto test_char_a = 'a';

        Ok<char> ok{test_char_a};

        detail::ResultStorage<char, int> storage{ok};
        detail::ResultStorage<char, int> storage_cpy{storage};

        REQUIRE(storage.get_tag() == detail::ResultTag::OK);
        REQUIRE(storage.get_result() == test_char_a);

        REQUIRE(storage_cpy.get_tag() == detail::ResultTag::OK);
        REQUIRE(storage_cpy.get_result() == test_char_a);
    }
    SECTION("ResultStorage<char, int> copy constructor construction [Err]") {
        constexpr auto test_int = 100;

        Err<int> err{test_int};

        detail::ResultStorage<char, int> storage{err};
        detail::ResultStorage<char, int> storage_cpy{storage};

        REQUIRE(storage.get_tag() == detail::ResultTag::ERR);
        REQUIRE(storage.get_error() == test_int);

        REQUIRE(storage_cpy.get_tag() == detail::ResultTag::ERR);
        REQUIRE(storage_cpy.get_error() == test_int);
    }
    SECTION("ResultStorage<char, int> move constructor construction [Ok]") {
        constexpr auto test_char_a = 'a';

        Ok<char> ok{test_char_a};

        detail::ResultStorage<char, int> storage{ok};
        detail::ResultStorage<char, int> storage_mv{std::move(storage)};

        REQUIRE(storage.get_tag() == detail::ResultTag::INVALID);

        REQUIRE(storage_mv.get_tag() == detail::ResultTag::OK);
        REQUIRE(storage_mv.get_result() == test_char_a);
    }
    SECTION("ResultStorage<char, int> move constructor construction [Err]") {
        constexpr auto test_int = 100;

        Err<int> err{test_int};

        detail::ResultStorage<char, int> storage{err};
        detail::ResultStorage<char, int> storage_mv{std::move(storage)};

        REQUIRE(storage.get_tag() == detail::ResultTag::INVALID);

        REQUIRE(storage_mv.get_tag() == detail::ResultTag::ERR);
        REQUIRE(storage_mv.get_error() == test_int);
    }
    SECTION("ResultStorage<char, int> copy assignment operator construction [Ok]") {
        constexpr auto test_char_a = 'a';
        constexpr auto test_char_b = 'b';

        Ok<char> ok_a{test_char_a};
        Ok<char> ok_b{test_char_b};

        detail::ResultStorage<char, int> storage_a{ok_a};

        REQUIRE(storage_a.get_tag() == detail::ResultTag::OK);
        REQUIRE(storage_a.get_result() == test_char_a);

        detail::ResultStorage<char, int> storage_b{ok_b};

        REQUIRE(storage_b.get_tag() == detail::ResultTag::OK);
        REQUIRE(storage_b.get_result() == test_char_b);

        storage_b = storage_a;

        REQUIRE(storage_a.get_tag() == detail::ResultTag::OK);
        REQUIRE(storage_a.get_result() == test_char_a);

        REQUIRE(storage_b.get_tag() == detail::ResultTag::OK);
        REQUIRE(storage_b.get_result() == test_char_a);
    }
    SECTION("ResultStorage<char, int> copy assignment operator construction [Err]") {
        constexpr auto test_int_100 = 100;
        constexpr auto test_int_200 = 200;

        Err<int> err_100{test_int_100};
        Err<int> err_200{test_int_200};

        detail::ResultStorage<char, int> storage_100{err_100};

        REQUIRE(storage_100.get_tag() == detail::ResultTag::ERR);
        REQUIRE(storage_100.get_error() == test_int_100);

        detail::ResultStorage<char, int> storage_200{err_200};

        REQUIRE(storage_200.get_tag() == detail::ResultTag::ERR);
        REQUIRE(storage_200.get_error() == test_int_200);

        storage_200 = storage_100;

        REQUIRE(storage_100.get_tag() == detail::ResultTag::ERR);
        REQUIRE(storage_100.get_error() == test_int_100);

        REQUIRE(storage_200.get_tag() == detail::ResultTag::ERR);
        REQUIRE(storage_200.get_error() == test_int_100);
    }
    SECTION("ResultStorage<char, int> move assignment operator construction [Ok]") {
        constexpr auto test_char_a = 'a';
        constexpr auto test_char_b = 'b';

        Ok<char> ok_a{test_char_a};
        Ok<char> ok_b{test_char_b};

        detail::ResultStorage<char, int> storage_a{ok_a};
        detail::ResultStorage<char, int> storage_b{ok_b};

        REQUIRE(storage_a.get_tag() == detail::ResultTag::OK);
        REQUIRE(storage_a.get_result() == test_char_a);

        REQUIRE(storage_b.get_tag() == detail::ResultTag::OK);
        REQUIRE(storage_b.get_result() == test_char_b);

        storage_b = std::move(storage_a);

        REQUIRE(storage_a.get_tag() == detail::ResultTag::INVALID);

        REQUIRE(storage_b.get_tag() == detail::ResultTag::OK);
        REQUIRE(storage_b.get_result() == test_char_a);
    }
    SECTION("ResultStorage<char, int> move assignment operator construction [Err]") {
        constexpr auto test_int_100 = 100;
        constexpr auto test_int_200 = 200;

        Err<int> err_100{test_int_100};
        Err<int> err_200{test_int_200};

        detail::ResultStorage<char, int> storage_100{err_100};
        detail::ResultStorage<char, int> storage_200{err_200};

        REQUIRE(storage_100.get_tag() == detail::ResultTag::ERR);
        REQUIRE(storage_100.get_error() == test_int_100);

        REQUIRE(storage_200.get_tag() == detail::ResultTag::ERR);
        REQUIRE(storage_200.get_error() == test_int_200);

        storage_200 = std::move(storage_100);

        REQUIRE(storage_100.get_tag() == detail::ResultTag::INVALID);

        REQUIRE(storage_200.get_tag() == detail::ResultTag::ERR);
        REQUIRE(storage_200.get_error() == test_int_100);
    }
}

TEST_CASE("ResultStorage Non-Trivial Destruction [result][ResultStorage]") {
    SECTION("ResultStorage<NotSoTrivial, int> Ok<NotSoTrivial> lvalue construction") {
        auto int_ptr             = std::make_shared<int>(100);
        const std::string phrase = "This is a test of template deduction";

        NotSoTrivial not_trivial{phrase, int_ptr};

        Ok<NotSoTrivial> ok{not_trivial};

        REQUIRE_FALSE(std::is_trivially_destructible<NotSoTrivial>::value);
        detail::ResultStorage<NotSoTrivial, int> storage{ok};

        auto& not_trivial_storage = storage.get_result();

        REQUIRE(storage.get_tag() == detail::ResultTag::OK);
        REQUIRE(not_trivial_storage.str == not_trivial.str);
        REQUIRE(not_trivial_storage.int_ptr == not_trivial.int_ptr);
        REQUIRE(*not_trivial_storage.int_ptr == *not_trivial.int_ptr);
    }
    SECTION("ResultStorage<NotSoTrivial, int> Ok<NotSoTrivial> rvalue construction") {
        auto int_ptr             = std::make_shared<int>(100);
        const std::string phrase = "This is a test of template deduction";

        NotSoTrivial not_trivial{phrase, int_ptr};

        Ok<NotSoTrivial> ok{not_trivial};

        REQUIRE_FALSE(std::is_trivially_destructible<NotSoTrivial>::value);
        detail::ResultStorage<NotSoTrivial, int> storage{std::move(ok)};

        auto& not_trivial_storage = storage.get_result();

        REQUIRE(storage.get_tag() == detail::ResultTag::OK);
        REQUIRE(not_trivial_storage.str == not_trivial.str);
        REQUIRE(*not_trivial_storage.int_ptr == *int_ptr);
    }
    SECTION("ResultStorage<int, NotSoTrivial> Err<NotSoTrivial> lvalue construction") {
        auto int_ptr             = std::make_shared<int>(100);
        const std::string phrase = "This is a test of template deduction";

        NotSoTrivial not_trivial{phrase, int_ptr};

        Err<NotSoTrivial> err{not_trivial};

        REQUIRE_FALSE(std::is_trivially_destructible<NotSoTrivial>::value);
        detail::ResultStorage<int, NotSoTrivial> storage{err};

        auto& not_trivial_storage = storage.get_error();

        REQUIRE(storage.get_tag() == detail::ResultTag::ERR);
        REQUIRE(not_trivial_storage.str == not_trivial.str);
        REQUIRE(not_trivial_storage.int_ptr == not_trivial.int_ptr);
        REQUIRE(*not_trivial_storage.int_ptr == *not_trivial.int_ptr);
    }
    SECTION("ResultStorage<int, NotSoTrivial> Err<NotSoTrivial> rvalue construction") {
        auto int_ptr             = std::make_shared<int>(100);
        const std::string phrase = "This is a test of template deduction";

        NotSoTrivial not_trivial{phrase, int_ptr};

        Err<NotSoTrivial> err{not_trivial};

        REQUIRE_FALSE(std::is_trivially_destructible<NotSoTrivial>::value);
        detail::ResultStorage<int, NotSoTrivial> storage{std::move(err)};

        auto& not_trivial_storage = storage.get_error();

        REQUIRE(storage.get_tag() == detail::ResultTag::ERR);
        REQUIRE(not_trivial_storage.str == not_trivial.str);
        REQUIRE(*not_trivial_storage.int_ptr == *int_ptr);
    }
    SECTION("ResultStorage<NotSoTrivial, int> copy construction [Ok<NotSoTrivial>]") {
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
        REQUIRE(not_trivial_storage.int_ptr == not_trivial.int_ptr);
        REQUIRE(*not_trivial_storage.int_ptr == *not_trivial.int_ptr);

        auto& not_trivial_storage_cpy = storage_cpy.get_result();

        REQUIRE(storage_cpy.get_tag() == detail::ResultTag::OK);

        REQUIRE(not_trivial_storage_cpy.str == not_trivial.str);
        REQUIRE(not_trivial_storage_cpy.int_ptr == not_trivial.int_ptr);
        REQUIRE(*not_trivial_storage_cpy.int_ptr == *not_trivial.int_ptr);
    }
    SECTION("ResultStorage<NotSoTrivial, int> copy construction [Err<int>]") {
        auto err_val = 100;
        Err<int> err{err_val};

        detail::ResultStorage<NotSoTrivial, int> storage{err};
        detail::ResultStorage<NotSoTrivial, int> storage_cpy{storage};

        REQUIRE(storage.get_tag() == detail::ResultTag::ERR);
        REQUIRE(storage.get_error() == err_val);

        REQUIRE(storage_cpy.get_tag() == detail::ResultTag::ERR);
        REQUIRE(storage_cpy.get_error() == err_val);
    }
    SECTION("ResultStorage<int, NotSoTrivial> copy construction [Ok<int>]") {
        auto ok_val = 100;
        Ok<int> ok{ok_val};

        detail::ResultStorage<int, NotSoTrivial> storage{ok};
        detail::ResultStorage<int, NotSoTrivial> storage_cpy{storage};

        REQUIRE(storage.get_tag() == detail::ResultTag::OK);
        REQUIRE(storage.get_result() == ok_val);

        REQUIRE(storage_cpy.get_tag() == detail::ResultTag::OK);
        REQUIRE(storage_cpy.get_result() == ok_val);
    }
    SECTION("ResultStorage<int, NotSoTrivial> copy construction [Err<NotSoTrivial>]") {
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
        REQUIRE(not_trivial_storage.int_ptr == not_trivial.int_ptr);
        REQUIRE(*not_trivial_storage.int_ptr == *not_trivial.int_ptr);

        auto& not_trivial_storage_cpy = storage_cpy.get_error();

        REQUIRE(storage_cpy.get_tag() == detail::ResultTag::ERR);

        REQUIRE(not_trivial_storage_cpy.str == not_trivial.str);
        REQUIRE(not_trivial_storage_cpy.int_ptr == not_trivial.int_ptr);
        REQUIRE(*not_trivial_storage_cpy.int_ptr == *not_trivial.int_ptr);
    }
    SECTION("ResultStorage<NotSoTrivial, int> copy assignment [Ok<NotSoTrivial>]") {
        auto int_ptr_foo             = std::make_shared<int>(100);
        const std::string phrase_foo = "This is a test of template deduction foo";

        NotSoTrivial not_trivial_foo{phrase_foo, int_ptr_foo};

        auto int_ptr_bar             = std::make_shared<int>(500);
        const std::string phrase_bar = "This is a test of template deduction bar";

        NotSoTrivial not_trivial_bar{phrase_bar, int_ptr_bar};

        Ok<NotSoTrivial> ok_foo{not_trivial_foo};
        Ok<NotSoTrivial> ok_bar{not_trivial_bar};

        REQUIRE_FALSE(std::is_trivially_destructible<NotSoTrivial>::value);

        detail::ResultStorage<NotSoTrivial, int> storage_foo{ok_foo};
        detail::ResultStorage<NotSoTrivial, int> storage_bar{ok_bar};

        REQUIRE(storage_foo.get_tag() == detail::ResultTag::OK);

        {
            const auto& not_trivial_storage = storage_foo.get_result();

            REQUIRE(not_trivial_storage.str == not_trivial_foo.str);
            REQUIRE(not_trivial_storage.int_ptr == not_trivial_foo.int_ptr);
            REQUIRE(*not_trivial_storage.int_ptr == *not_trivial_foo.int_ptr);
        }

        REQUIRE(storage_bar.get_tag() == detail::ResultTag::OK);

        {
            const auto& not_trivial_storage = storage_bar.get_result();

            REQUIRE(not_trivial_storage.str == not_trivial_bar.str);
            REQUIRE(not_trivial_storage.int_ptr == not_trivial_bar.int_ptr);
            REQUIRE(*not_trivial_storage.int_ptr == *not_trivial_bar.int_ptr);
        }

        storage_bar = storage_foo;

        REQUIRE(storage_foo.get_tag() == detail::ResultTag::OK);

        {
            const auto& not_trivial_storage = storage_foo.get_result();

            REQUIRE(not_trivial_storage.str == not_trivial_foo.str);
            REQUIRE(not_trivial_storage.int_ptr == not_trivial_foo.int_ptr);
            REQUIRE(*not_trivial_storage.int_ptr == *not_trivial_foo.int_ptr);
        }

        REQUIRE(storage_bar.get_tag() == detail::ResultTag::OK);

        {
            const auto& not_trivial_storage = storage_bar.get_result();

            REQUIRE(not_trivial_storage.str == not_trivial_foo.str);
            REQUIRE(not_trivial_storage.int_ptr == not_trivial_foo.int_ptr);
            REQUIRE(*not_trivial_storage.int_ptr == *not_trivial_foo.int_ptr);
        }
    }
    SECTION("ResultStorage<NotSoTrivial, int> copy assignment [Err<int>]") {
        auto err_val_foo = 100;
        Err<int> err_foo{err_val_foo};

        auto err_val_bar = 200;
        Err<int> err_bar{err_val_bar};

        REQUIRE_FALSE(std::is_trivially_destructible<NotSoTrivial>::value);

        detail::ResultStorage<NotSoTrivial, int> storage_foo{err_foo};
        detail::ResultStorage<NotSoTrivial, int> storage_bar{err_bar};

        REQUIRE(storage_foo.get_tag() == detail::ResultTag::ERR);
        REQUIRE(storage_foo.get_error() == err_val_foo);

        REQUIRE(storage_bar.get_tag() == detail::ResultTag::ERR);
        REQUIRE(storage_bar.get_error() == err_val_bar);

        storage_bar = storage_foo;

        REQUIRE(storage_foo.get_tag() == detail::ResultTag::ERR);
        REQUIRE(storage_foo.get_error() == err_val_foo);

        REQUIRE(storage_bar.get_tag() == detail::ResultTag::ERR);
        REQUIRE(storage_bar.get_error() == err_val_foo);
    }
    SECTION("ResultStorage<int, NotSoTrivial> copy assignment [Ok<int>]") {
        auto ok_val_foo = 100;
        Ok<int> ok_foo{ok_val_foo};

        auto ok_val_bar = 200;
        Ok<int> ok_bar{ok_val_bar};

        REQUIRE_FALSE(std::is_trivially_destructible<NotSoTrivial>::value);

        detail::ResultStorage<int, NotSoTrivial> storage_foo{ok_foo};
        detail::ResultStorage<int, NotSoTrivial> storage_bar{ok_bar};

        REQUIRE(storage_foo.get_tag() == detail::ResultTag::OK);
        REQUIRE(storage_foo.get_result() == ok_val_foo);

        REQUIRE(storage_bar.get_tag() == detail::ResultTag::OK);
        REQUIRE(storage_bar.get_result() == ok_val_bar);

        storage_bar = storage_foo;

        REQUIRE(storage_foo.get_tag() == detail::ResultTag::OK);
        REQUIRE(storage_foo.get_result() == ok_val_foo);

        REQUIRE(storage_bar.get_tag() == detail::ResultTag::OK);
        REQUIRE(storage_bar.get_result() == ok_val_foo);
    }
    SECTION("ResultStorage<int, NotSoTrivial> copy assignment [Err<NotSoTrivial>]") {
        auto int_ptr_foo             = std::make_shared<int>(100);
        const std::string phrase_foo = "This is a test of template deduction foo";

        NotSoTrivial not_trivial_foo{phrase_foo, int_ptr_foo};

        auto int_ptr_bar             = std::make_shared<int>(500);
        const std::string phrase_bar = "This is a test of template deduction bar";

        NotSoTrivial not_trivial_bar{phrase_bar, int_ptr_bar};

        Err<NotSoTrivial> err_foo{not_trivial_foo};
        Err<NotSoTrivial> err_bar{not_trivial_bar};

        REQUIRE_FALSE(std::is_trivially_destructible<NotSoTrivial>::value);

        detail::ResultStorage<int, NotSoTrivial> storage_foo{err_foo};
        detail::ResultStorage<int, NotSoTrivial> storage_bar{err_bar};

        REQUIRE(storage_foo.get_tag() == detail::ResultTag::ERR);

        {
            const auto& not_trivial_storage = storage_foo.get_error();

            REQUIRE(not_trivial_storage.str == not_trivial_foo.str);
            REQUIRE(not_trivial_storage.int_ptr == not_trivial_foo.int_ptr);
            REQUIRE(*not_trivial_storage.int_ptr == *not_trivial_foo.int_ptr);
        }

        REQUIRE(storage_bar.get_tag() == detail::ResultTag::ERR);

        {
            const auto& not_trivial_storage = storage_bar.get_error();

            REQUIRE(not_trivial_storage.str == not_trivial_bar.str);
            REQUIRE(not_trivial_storage.int_ptr == not_trivial_bar.int_ptr);
            REQUIRE(*not_trivial_storage.int_ptr == *not_trivial_bar.int_ptr);
        }

        storage_bar = storage_foo;

        REQUIRE(storage_foo.get_tag() == detail::ResultTag::ERR);

        {
            const auto& not_trivial_storage = storage_foo.get_error();

            REQUIRE(not_trivial_storage.str == not_trivial_foo.str);
            REQUIRE(not_trivial_storage.int_ptr == not_trivial_foo.int_ptr);
            REQUIRE(*not_trivial_storage.int_ptr == *not_trivial_foo.int_ptr);
        }

        REQUIRE(storage_bar.get_tag() == detail::ResultTag::ERR);

        {
            const auto& not_trivial_storage = storage_bar.get_error();

            REQUIRE(not_trivial_storage.str == not_trivial_foo.str);
            REQUIRE(not_trivial_storage.int_ptr == not_trivial_foo.int_ptr);
            REQUIRE(*not_trivial_storage.int_ptr == *not_trivial_foo.int_ptr);
        }
    }
    SECTION("ResultStorage<NotSoTrivial, int> move construction [Ok<NotSoTrivial>]") {
        auto int_ptr             = std::make_shared<int>(100);
        const std::string phrase = "This is a test of template deduction";

        NotSoTrivial not_trivial{phrase, int_ptr};

        Ok<NotSoTrivial> ok{not_trivial};

        REQUIRE_FALSE(std::is_trivially_destructible<NotSoTrivial>::value);

        detail::ResultStorage<NotSoTrivial, int> storage{ok};
        detail::ResultStorage<NotSoTrivial, int> storage_mv{std::move(storage)};

        REQUIRE(storage.get_tag() == detail::ResultTag::INVALID);

        auto& not_trivial_storage_mv = storage_mv.get_result();

        REQUIRE(storage_mv.get_tag() == detail::ResultTag::OK);

        REQUIRE(not_trivial_storage_mv.str == not_trivial.str);
        REQUIRE(not_trivial_storage_mv.int_ptr == not_trivial.int_ptr);
        REQUIRE(*not_trivial_storage_mv.int_ptr == *not_trivial.int_ptr);
    }
    SECTION("ResultStorage<NotSoTrivial, int> move construction [Err<int>]") {
        auto err_val = 100;
        Err<int> err{err_val};

        detail::ResultStorage<NotSoTrivial, int> storage{err};
        detail::ResultStorage<NotSoTrivial, int> storage_mv{std::move(storage)};

        REQUIRE(storage.get_tag() == detail::ResultTag::INVALID);

        REQUIRE(storage_mv.get_tag() == detail::ResultTag::ERR);
        REQUIRE(storage_mv.get_error() == err_val);
    }
    SECTION("ResultStorage<int, NotSoTrivial> copy construction [Ok<int>]") {
        auto ok_val = 100;
        Ok<int> ok{ok_val};

        detail::ResultStorage<int, NotSoTrivial> storage{ok};
        detail::ResultStorage<int, NotSoTrivial> storage_mv{std::move(storage)};

        REQUIRE(storage.get_tag() == detail::ResultTag::INVALID);

        REQUIRE(storage_mv.get_tag() == detail::ResultTag::OK);
        REQUIRE(storage_mv.get_result() == ok_val);
    }
    SECTION("ResultStorage<int, NotSoTrivial> move construction [Err<NotSoTrivial>]") {
        auto int_ptr             = std::make_shared<int>(100);
        const std::string phrase = "This is a test of template deduction";

        NotSoTrivial not_trivial{phrase, int_ptr};

        Err<NotSoTrivial> err{not_trivial};

        REQUIRE_FALSE(std::is_trivially_destructible<NotSoTrivial>::value);

        detail::ResultStorage<int, NotSoTrivial> storage{err};
        detail::ResultStorage<int, NotSoTrivial> storage_mv{std::move(storage)};

        REQUIRE(storage.get_tag() == detail::ResultTag::INVALID);

        auto& not_trivial_storage_mv = storage_mv.get_error();

        REQUIRE(storage_mv.get_tag() == detail::ResultTag::ERR);

        REQUIRE(not_trivial_storage_mv.str == not_trivial.str);
        REQUIRE(not_trivial_storage_mv.int_ptr == not_trivial.int_ptr);
        REQUIRE(*not_trivial_storage_mv.int_ptr == *not_trivial.int_ptr);
    }
    SECTION("ResultStorage<NotSoTrivial, int> move assignment [Ok<NotSoTrivial>]") {
        auto int_ptr_foo             = std::make_shared<int>(100);
        const std::string phrase_foo = "This is a test of template deduction foo";

        NotSoTrivial not_trivial_foo{phrase_foo, int_ptr_foo};

        auto int_ptr_bar             = std::make_shared<int>(500);
        const std::string phrase_bar = "This is a test of template deduction bar";

        NotSoTrivial not_trivial_bar{phrase_bar, int_ptr_bar};

        Ok<NotSoTrivial> ok_foo{not_trivial_foo};
        Ok<NotSoTrivial> ok_bar{not_trivial_bar};

        REQUIRE_FALSE(std::is_trivially_destructible<NotSoTrivial>::value);

        detail::ResultStorage<NotSoTrivial, int> storage_foo{ok_foo};
        detail::ResultStorage<NotSoTrivial, int> storage_bar{ok_bar};

        REQUIRE(storage_foo.get_tag() == detail::ResultTag::OK);

        {
            const auto& not_trivial_storage = storage_foo.get_result();

            REQUIRE(not_trivial_storage.str == not_trivial_foo.str);
            REQUIRE(not_trivial_storage.int_ptr == not_trivial_foo.int_ptr);
            REQUIRE(*not_trivial_storage.int_ptr == *not_trivial_foo.int_ptr);
        }

        REQUIRE(storage_bar.get_tag() == detail::ResultTag::OK);

        {
            const auto& not_trivial_storage = storage_bar.get_result();

            REQUIRE(not_trivial_storage.str == not_trivial_bar.str);
            REQUIRE(not_trivial_storage.int_ptr == not_trivial_bar.int_ptr);
            REQUIRE(*not_trivial_storage.int_ptr == *not_trivial_bar.int_ptr);
        }

        storage_bar = std::move(storage_foo);

        REQUIRE(storage_foo.get_tag() == detail::ResultTag::INVALID);
        REQUIRE(storage_bar.get_tag() == detail::ResultTag::OK);
        {
            const auto& not_trivial_storage = storage_bar.get_result();

            REQUIRE(not_trivial_storage.str == not_trivial_foo.str);
            REQUIRE(not_trivial_storage.int_ptr == not_trivial_foo.int_ptr);
            REQUIRE(*not_trivial_storage.int_ptr == *not_trivial_foo.int_ptr);
        }
    }
    SECTION("ResultStorage<NotSoTrivial, int> move assignment [Err<int>]") {
        auto err_val_foo = 100;
        Err<int> err_foo{err_val_foo};

        auto err_val_bar = 200;
        Err<int> err_bar{err_val_bar};

        REQUIRE_FALSE(std::is_trivially_destructible<NotSoTrivial>::value);

        detail::ResultStorage<NotSoTrivial, int> storage_foo{err_foo};
        detail::ResultStorage<NotSoTrivial, int> storage_bar{err_bar};

        REQUIRE(storage_foo.get_tag() == detail::ResultTag::ERR);
        REQUIRE(storage_foo.get_error() == err_val_foo);

        REQUIRE(storage_bar.get_tag() == detail::ResultTag::ERR);
        REQUIRE(storage_bar.get_error() == err_val_bar);

        storage_bar = std::move(storage_foo);

        REQUIRE(storage_foo.get_tag() == detail::ResultTag::INVALID);

        REQUIRE(storage_bar.get_tag() == detail::ResultTag::ERR);
        REQUIRE(storage_bar.get_error() == err_val_foo);
    }
    SECTION("ResultStorage<int, NotSoTrivial> move assignment [Ok<int>]") {
        auto ok_val_foo = 100;
        Ok<int> ok_foo{ok_val_foo};

        auto ok_val_bar = 200;
        Ok<int> ok_bar{ok_val_bar};

        REQUIRE_FALSE(std::is_trivially_destructible<NotSoTrivial>::value);

        detail::ResultStorage<int, NotSoTrivial> storage_foo{ok_foo};
        detail::ResultStorage<int, NotSoTrivial> storage_bar{ok_bar};

        REQUIRE(storage_foo.get_tag() == detail::ResultTag::OK);
        REQUIRE(storage_foo.get_result() == ok_val_foo);

        REQUIRE(storage_bar.get_tag() == detail::ResultTag::OK);
        REQUIRE(storage_bar.get_result() == ok_val_bar);

        storage_bar = std::move(storage_foo);

        REQUIRE(storage_foo.get_tag() == detail::ResultTag::INVALID);

        REQUIRE(storage_bar.get_tag() == detail::ResultTag::OK);
        REQUIRE(storage_bar.get_result() == ok_val_foo);
    }
    SECTION("ResultStorage<int, NotSoTrivial> move assignment [Err<NotSoTrivial>]") {
        auto int_ptr_foo             = std::make_shared<int>(100);
        const std::string phrase_foo = "This is a test of template deduction foo";

        NotSoTrivial not_trivial_foo{phrase_foo, int_ptr_foo};

        auto int_ptr_bar             = std::make_shared<int>(500);
        const std::string phrase_bar = "This is a test of template deduction bar";

        NotSoTrivial not_trivial_bar{phrase_bar, int_ptr_bar};

        Err<NotSoTrivial> err_foo{not_trivial_foo};
        Err<NotSoTrivial> err_bar{not_trivial_bar};

        REQUIRE_FALSE(std::is_trivially_destructible<NotSoTrivial>::value);

        detail::ResultStorage<int, NotSoTrivial> storage_foo{err_foo};
        detail::ResultStorage<int, NotSoTrivial> storage_bar{err_bar};

        REQUIRE(storage_foo.get_tag() == detail::ResultTag::ERR);

        {
            const auto& not_trivial_storage = storage_foo.get_error();

            REQUIRE(not_trivial_storage.str == not_trivial_foo.str);
            REQUIRE(not_trivial_storage.int_ptr == not_trivial_foo.int_ptr);
            REQUIRE(*not_trivial_storage.int_ptr == *not_trivial_foo.int_ptr);
        }

        REQUIRE(storage_bar.get_tag() == detail::ResultTag::ERR);

        {
            const auto& not_trivial_storage = storage_bar.get_error();

            REQUIRE(not_trivial_storage.str == not_trivial_bar.str);
            REQUIRE(not_trivial_storage.int_ptr == not_trivial_bar.int_ptr);
            REQUIRE(*not_trivial_storage.int_ptr == *not_trivial_bar.int_ptr);
        }

        storage_bar = std::move(storage_foo);

        REQUIRE(storage_foo.get_tag() == detail::ResultTag::INVALID);
        REQUIRE(storage_bar.get_tag() == detail::ResultTag::ERR);

        {
            const auto& not_trivial_storage = storage_bar.get_error();

            REQUIRE(not_trivial_storage.str == not_trivial_foo.str);
            REQUIRE(not_trivial_storage.int_ptr == not_trivial_foo.int_ptr);
            REQUIRE(*not_trivial_storage.int_ptr == *not_trivial_foo.int_ptr);
        }
    }
}

TEST_CASE("ResultStorage Trivial Destruction [result][ResultStorage] void Result specialization") {
    SECTION("ResultStorage<void, int> Ok<void> lvalue construction") {
        Ok<void> ok{};
        detail::ResultStorage<void, int> storage{ok};

        REQUIRE(storage.get_tag() == detail::ResultTag::OK);
    }
    SECTION("ResultStorage<void, int> Ok<void> rvalue construction") {
        Ok<void> ok{};
        detail::ResultStorage<void, int> storage{std::move(ok)};

        REQUIRE(storage.get_tag() == detail::ResultTag::OK);
    }
    SECTION("ResultStorage<void, int> Err<int> lvalue construction") {
        constexpr auto test_int = 1000;

        Err<int> err{test_int};
        detail::ResultStorage<void, int> storage{err};

        REQUIRE(storage.get_tag() == detail::ResultTag::ERR);
        REQUIRE(storage.get_error() == test_int);
        REQUIRE(err.get_error() == test_int);
    }
    SECTION("ResultStorage<void, int> Err<int> rvalue construction") {
        constexpr auto test_int = 100;

        Err<int> err{test_int};
        detail::ResultStorage<void, int> storage{std::move(err)};

        REQUIRE(storage.get_tag() == detail::ResultTag::ERR);
        REQUIRE(storage.get_error() == test_int);
    }
    SECTION("ResultStorage<void, int> copy constructor construction [Ok]") {
        Ok<void> ok{};

        detail::ResultStorage<void, int> storage{ok};
        detail::ResultStorage<void, int> storage_cpy{storage};

        REQUIRE(storage.get_tag() == detail::ResultTag::OK);
        REQUIRE(storage_cpy.get_tag() == detail::ResultTag::OK);
    }
    SECTION("ResultStorage<void, int> copy constructor construction [Err]") {
        constexpr auto test_int = 100;

        Err<int> err{test_int};

        detail::ResultStorage<void, int> storage{err};
        detail::ResultStorage<void, int> storage_cpy{storage};

        REQUIRE(storage.get_tag() == detail::ResultTag::ERR);
        REQUIRE(storage.get_error() == test_int);

        REQUIRE(storage_cpy.get_tag() == detail::ResultTag::ERR);
        REQUIRE(storage_cpy.get_error() == test_int);
    }
    SECTION("ResultStorage<void, int> move constructor construction [Ok]") {
        Ok<void> ok{};

        detail::ResultStorage<void, int> storage{ok};
        detail::ResultStorage<void, int> storage_mv{std::move(storage)};

        REQUIRE(storage.get_tag() == detail::ResultTag::INVALID);

        REQUIRE(storage_mv.get_tag() == detail::ResultTag::OK);
    }
    SECTION("ResultStorage<void, int> move constructor construction [Err]") {
        constexpr auto test_int = 100;

        Err<int> err{test_int};

        detail::ResultStorage<char, int> storage{err};
        detail::ResultStorage<char, int> storage_mv{std::move(storage)};

        REQUIRE(storage.get_tag() == detail::ResultTag::INVALID);

        REQUIRE(storage_mv.get_tag() == detail::ResultTag::ERR);
        REQUIRE(storage_mv.get_error() == test_int);
    }
    SECTION("ResultStorage<void, int> copy assignment operator construction [Ok]") {
        Ok<void> ok{};

        detail::ResultStorage<void, int> storage{ok};
        detail::ResultStorage<void, int> storage_cpy = storage;

        REQUIRE(storage.get_tag() == detail::ResultTag::OK);
        REQUIRE(storage_cpy.get_tag() == detail::ResultTag::OK);
    }
    SECTION("ResultStorage<void, int> copy assignment operator construction [Err]") {
        constexpr auto test_int = 100;

        Err<int> err{test_int};

        detail::ResultStorage<void, int> storage{err};
        detail::ResultStorage<void, int> storage_cpy = storage;

        REQUIRE(storage.get_tag() == detail::ResultTag::ERR);
        REQUIRE(storage.get_error() == test_int);

        REQUIRE(storage_cpy.get_tag() == detail::ResultTag::ERR);
        REQUIRE(storage_cpy.get_error() == test_int);
    }
    SECTION("ResultStorage<void, int> move assignment operator construction [Ok]") {
        Ok<void> ok{};

        detail::ResultStorage<void, int> storage{ok};
        detail::ResultStorage<void, int> storage_mv = std::move(storage);

        REQUIRE(storage.get_tag() == detail::ResultTag::INVALID);
        REQUIRE(storage_mv.get_tag() == detail::ResultTag::OK);
    }
    SECTION("ResultStorage<void, int> move assignment operator construction [Err]") {
        constexpr auto test_int = 100;

        Err<int> err{test_int};

        detail::ResultStorage<void, int> storage{err};
        detail::ResultStorage<void, int> storage_mv = std::move(storage);

        REQUIRE(storage.get_tag() == detail::ResultTag::INVALID);

        REQUIRE(storage_mv.get_tag() == detail::ResultTag::ERR);
        REQUIRE(storage_mv.get_error() == test_int);
    }
}

TEST_CASE("ResultStorage Non-Trivial Destruction [result][ResultStorage] void specialization") {
    SECTION("ResultStorage<void, NotSoTrivial> Err<NotSoTrivial> lvalue construction") {
        auto int_ptr             = std::make_shared<int>(100);
        const std::string phrase = "This is a test of template deduction";

        NotSoTrivial not_trivial{phrase, int_ptr};

        Err<NotSoTrivial> err{not_trivial};

        REQUIRE_FALSE(std::is_trivially_destructible<NotSoTrivial>::value);
        detail::ResultStorage<void, NotSoTrivial> storage{err};

        auto& not_trivial_storage = storage.get_error();

        REQUIRE(storage.get_tag() == detail::ResultTag::ERR);
        REQUIRE(not_trivial_storage.str == not_trivial.str);
        REQUIRE(not_trivial_storage.int_ptr == not_trivial.int_ptr);
        REQUIRE(*not_trivial_storage.int_ptr == *not_trivial.int_ptr);
    }
    SECTION("ResultStorage<void, NotSoTrivial> Err<NotSoTrivial> rvalue construction") {
        auto int_ptr             = std::make_shared<int>(100);
        const std::string phrase = "This is a test of template deduction";

        NotSoTrivial not_trivial{phrase, int_ptr};

        Err<NotSoTrivial> err{not_trivial};

        REQUIRE_FALSE(std::is_trivially_destructible<NotSoTrivial>::value);
        detail::ResultStorage<void, NotSoTrivial> storage{std::move(err)};

        auto& not_trivial_storage = storage.get_error();

        REQUIRE(storage.get_tag() == detail::ResultTag::ERR);
        REQUIRE(not_trivial_storage.str == not_trivial.str);
        REQUIRE(*not_trivial_storage.int_ptr == *int_ptr);
    }
    SECTION("ResultStorage<void, NotSoTrivial> copy construction [Ok<void>]") {
        Ok<void> ok{};

        detail::ResultStorage<void, NotSoTrivial> storage{ok};
        detail::ResultStorage<void, NotSoTrivial> storage_cpy{storage};

        REQUIRE(storage.get_tag() == detail::ResultTag::OK);
        REQUIRE(storage_cpy.get_tag() == detail::ResultTag::OK);
    }
    SECTION("ResultStorage<void, NotSoTrivial> copy construction [Err<NotSoTrivial>]") {
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
        REQUIRE(not_trivial_storage.int_ptr == not_trivial.int_ptr);
        REQUIRE(*not_trivial_storage.int_ptr == *not_trivial.int_ptr);

        auto& not_trivial_storage_cpy = storage_cpy.get_error();

        REQUIRE(storage_cpy.get_tag() == detail::ResultTag::ERR);

        REQUIRE(not_trivial_storage_cpy.str == not_trivial.str);
        REQUIRE(not_trivial_storage_cpy.int_ptr == not_trivial.int_ptr);
        REQUIRE(*not_trivial_storage_cpy.int_ptr == *not_trivial.int_ptr);
    }
    SECTION("ResultStorage<void, NotSoTrivial> copy assignment [Ok<void>]") {
        Ok<void> ok{};

        detail::ResultStorage<void, NotSoTrivial> storage{ok};
        detail::ResultStorage<void, NotSoTrivial> storage_cpy = storage;

        REQUIRE(storage.get_tag() == detail::ResultTag::OK);
        REQUIRE(storage_cpy.get_tag() == detail::ResultTag::OK);
    }
    SECTION("ResultStorage<void, NotSoTrivial> copy assignment [Err<NotSoTrivial>]") {
        auto int_ptr             = std::make_shared<int>(100);
        const std::string phrase = "This is a test of template deduction";

        NotSoTrivial not_trivial{phrase, int_ptr};

        Err<NotSoTrivial> err{not_trivial};

        REQUIRE_FALSE(std::is_trivially_destructible<NotSoTrivial>::value);

        detail::ResultStorage<void, NotSoTrivial> storage{err};
        detail::ResultStorage<void, NotSoTrivial> storage_cpy = storage;

        auto& not_trivial_storage = storage.get_error();

        REQUIRE(storage.get_tag() == detail::ResultTag::ERR);
        REQUIRE(not_trivial_storage.str == not_trivial.str);
        REQUIRE(not_trivial_storage.int_ptr == not_trivial.int_ptr);
        REQUIRE(*not_trivial_storage.int_ptr == *not_trivial.int_ptr);

        auto& not_trivial_storage_cpy = storage_cpy.get_error();

        REQUIRE(storage_cpy.get_tag() == detail::ResultTag::ERR);

        REQUIRE(not_trivial_storage_cpy.str == not_trivial.str);
        REQUIRE(not_trivial_storage_cpy.int_ptr == not_trivial.int_ptr);
        REQUIRE(*not_trivial_storage_cpy.int_ptr == *not_trivial.int_ptr);
    }
    SECTION("ResultStorage<void, NotSoTrivial> move assignment [Ok<void>]") {
        Ok<void> ok{};

        detail::ResultStorage<void, NotSoTrivial> storage{ok};
        detail::ResultStorage<void, NotSoTrivial> storage_mv = std::move(storage);

        REQUIRE(storage.get_tag() == detail::ResultTag::INVALID);

        REQUIRE(storage_mv.get_tag() == detail::ResultTag::OK);
    }
    SECTION("ResultStorage<void, NotSoTrivial> move assignment [Err<NotSoTrivial>]") {
        auto int_ptr             = std::make_shared<int>(100);
        const std::string phrase = "This is a test of template deduction";

        NotSoTrivial not_trivial{phrase, int_ptr};
        Err<NotSoTrivial> err{not_trivial};

        REQUIRE_FALSE(std::is_trivially_destructible<NotSoTrivial>::value);

        detail::ResultStorage<void, NotSoTrivial> storage{err};
        detail::ResultStorage<void, NotSoTrivial> storage_mv = std::move(storage);

        REQUIRE(storage.get_tag() == detail::ResultTag::INVALID);

        auto& not_trivial_storage_mv = storage_mv.get_error();

        REQUIRE(storage_mv.get_tag() == detail::ResultTag::ERR);

        REQUIRE(not_trivial_storage_mv.str == not_trivial.str);
        REQUIRE(not_trivial_storage_mv.int_ptr == not_trivial.int_ptr);
        REQUIRE(*not_trivial_storage_mv.int_ptr == *not_trivial.int_ptr);
    }
}

TEST_CASE("ResultStorage Non-Trivial multiple assignment [result][ResultStorage] void specialization") {
    SECTION("ResultStorage<NotSoTrivial, AnotherNotSoTrivial> multiple assignment") {
        std::string s{"This is a very large string for testing purposes :)"};
        auto int_ptr = std::make_shared<int>(10000);

        NotSoTrivial nst{s, int_ptr};
        detail::ResultStorage<NotSoTrivial, AnotherNotSoTrivial> initial_storage{Ok<NotSoTrivial>{nst}};

        REQUIRE(initial_storage.get_tag() == detail::ResultTag::OK);

        auto& initial_value = initial_storage.get_result();

        REQUIRE(initial_value.str == s);
        REQUIRE(initial_value.int_ptr == int_ptr);
        REQUIRE(*initial_value.int_ptr == *int_ptr);
        REQUIRE(int_ptr.use_count() == 3);

        std::string a{"This is a very large string for testing purposes :), but is not the original"};
        auto int_ptr_err = std::make_shared<int>(-1);

        NotSoTrivial nst_err{a, int_ptr_err};

        constexpr auto VEC_SIZE = 100;

        std::vector<int> vec(VEC_SIZE);
        std::unordered_map<int, std::string> kv_map;

        std::generate(vec.begin(), vec.end(), [start = 0]() mutable { return start++; });
        std::for_each(vec.begin(), vec.end(), [&kv_map](const int i) { kv_map[i] = std::to_string(i); });

        AnotherNotSoTrivial anst{nst_err, kv_map, vec};
        detail::ResultStorage<NotSoTrivial, AnotherNotSoTrivial> initial_storage_err{Err<AnotherNotSoTrivial>{anst}};

        REQUIRE(initial_storage_err.get_tag() == detail::ResultTag::ERR);

        auto& initial_err = initial_storage_err.get_error();

        REQUIRE(initial_err.not_trivial.str == a);
        REQUIRE(initial_err.not_trivial.int_ptr == int_ptr_err);
        REQUIRE(*initial_err.not_trivial.int_ptr == *int_ptr_err);

        {
            REQUIRE(initial_err.num_vec.size() == VEC_SIZE);
            std::for_each(
                initial_err.num_vec.begin(), initial_err.num_vec.end(),
                [m = initial_err.num_map, ref_vec = vec, ref_map = kv_map, count = 0ul](const auto& i) mutable {
                    REQUIRE(i == ref_vec[count]);
                    REQUIRE_FALSE(m.count(i) == 0);
                    REQUIRE(m[i] == ref_map[i]);
                    count++;
                });
        }

        REQUIRE(int_ptr_err.use_count() == 4);

        initial_storage = initial_storage_err;

        REQUIRE(initial_storage.get_tag() == detail::ResultTag::ERR);

        REQUIRE(int_ptr.use_count() == 2);
        REQUIRE(int_ptr_err.use_count() == 5);

        auto& initial_storage_now_err = initial_storage.get_error();

        REQUIRE(initial_storage_now_err.not_trivial.str == a);
        REQUIRE(initial_storage_now_err.not_trivial.int_ptr == int_ptr_err);
        REQUIRE(*initial_storage_now_err.not_trivial.int_ptr == *int_ptr_err);

        {
            auto& initial_err_chk = initial_storage_err.get_error();

            REQUIRE(initial_err_chk.num_vec.size() == VEC_SIZE);
            std::for_each(
                initial_err_chk.num_vec.begin(), initial_err_chk.num_vec.end(),
                [m = initial_err.num_map, ref_vec = vec, ref_map = kv_map, count = 0ul](const auto& i) mutable {
                    REQUIRE(i == ref_vec[count]);
                    REQUIRE_FALSE(m.count(i) == 0);
                    REQUIRE(m[i] == ref_map[i]);
                    count++;
                });

            REQUIRE(initial_storage_now_err.num_vec.size() == VEC_SIZE);
            std::for_each(initial_storage_now_err.num_vec.begin(), initial_storage_now_err.num_vec.end(),
                          [m = initial_storage_now_err.num_map, ref_vec = vec, ref_map = kv_map,
                           count = 0ul](const auto& i) mutable {
                              REQUIRE(i == ref_vec[count]);
                              REQUIRE_FALSE(m.count(i) == 0);
                              REQUIRE(m[i] == ref_map[i]);
                              count++;
                          });
        }

        std::string s_mv{"This is a very large string for testing purposes :) Ok maybe very large is an exaggeration"};
        auto int_ptr_mv = std::make_shared<int>(35);

        NotSoTrivial nst_mv{s_mv, int_ptr_mv};
        detail::ResultStorage<NotSoTrivial, AnotherNotSoTrivial> initial_storage_mv{Ok<NotSoTrivial>{nst_mv}};

        REQUIRE(initial_storage_mv.get_tag() == detail::ResultTag::OK);

        auto& initial_value_mv = initial_storage_mv.get_result();

        REQUIRE(initial_value_mv.str == s_mv);
        REQUIRE(initial_value_mv.int_ptr == int_ptr_mv);
        REQUIRE(*initial_value_mv.int_ptr == *int_ptr_mv);
        REQUIRE(int_ptr_mv.use_count() == 3);

        initial_storage = std::move(initial_storage_mv);

        REQUIRE(initial_storage_mv.get_tag() == detail::ResultTag::INVALID);

        {
            auto& initial_value_scope = initial_storage.get_result();

            REQUIRE(initial_value_scope.str == s_mv);
            REQUIRE(initial_value_scope.int_ptr == int_ptr_mv);
            REQUIRE(*initial_value_scope.int_ptr == *int_ptr_mv);
            REQUIRE(int_ptr_mv.use_count() == 3);
        }

        {
            REQUIRE(initial_storage_err.get_tag() == detail::ResultTag::ERR);

            auto& initial_err_scope = initial_storage_err.get_error();

            REQUIRE(initial_err_scope.not_trivial.str == a);
            REQUIRE(initial_err_scope.not_trivial.int_ptr == int_ptr_err);
            REQUIRE(*initial_err_scope.not_trivial.int_ptr == *int_ptr_err);

            REQUIRE(initial_err_scope.num_vec.size() == VEC_SIZE);
            std::for_each(
                initial_err_scope.num_vec.begin(), initial_err_scope.num_vec.end(),
                [m = initial_err_scope.num_map, ref_vec = vec, ref_map = kv_map, count = 0ul](const auto& i) mutable {
                    REQUIRE(i == ref_vec[count]);
                    REQUIRE_FALSE(m.count(i) == 0);
                    REQUIRE(m[i] == ref_map[i]);
                    count++;
                });
        }
    }
}

}  // namespace