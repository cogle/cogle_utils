#include <example_helpers.hxx>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>
#include <utils/result.hxx>

using namespace cogle::utils::result;
namespace fs  = std::filesystem;
namespace dir = examples::directory;

namespace main_return_codes {
constexpr auto SUCCESS = 0;
constexpr auto FAILURE = -1;
}  // namespace main_return_codes

Result<fs::path, std::error_code> create_temp_file() {
    auto dir_path = fs::path(dir::EXAMPLES_DIR_PATH) / "filesystem_result_example";
    std::cout << "Attempting to create: " << dir_path << std::endl;

    std::error_code ec{};
    if (!fs::create_directory(dir_path, ec) && ec != ec.default_error_condition()) {
        return Err<std::error_code>(ec);
    }

    return Ok<fs::path>{std::move(dir_path)};
}

int main(int argc, char const* argv[]) {
    // This is done to prevent compiler error of unused.
    (void)argc;
    (void)argv;

    auto create_ret = create_temp_file();

    if (!create_ret) {
        std::cerr << "Unable to create temporary file error code " << create_ret.error().message() << std::endl;
        return main_return_codes::FAILURE;
    }

    const auto& file = *create_ret;

    std::cout << "Created: " << file << std::endl;

    return main_return_codes::SUCCESS;
}