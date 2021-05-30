#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

#include <example_helpers.hxx>
#include <filesystem>
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

Result<void, int> create_dir(const std::string_view dir_path) {
    // https://man7.org/linux/man-pages/man7/inode.7.html
    auto ret = mkdir(dir_path.data(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

    if (ret != 0) {
        const auto err = errno;
        if (err != EEXIST) {
            return Err<int>{err};
        }
    }

    return Ok<void>{};
}

Result<int, int> open_file(const std::string_view file_name, const int oflag = O_RDWR) {
    auto fd = open(file_name.data(), oflag, 0644);

    if (fd == -1) {
        const auto err = errno;
        return Err<int>{err};
    }

    return Ok<int>{fd};
}

int main(int argc, char const* argv[]) {
    // This is done to prevent compiler error of unused.
    (void)argc;
    (void)argv;

    auto dir_name       = fs::path(dir::EXAMPLES_DIR_PATH) / "posix_result_example";
    auto create_dir_ret = create_dir(dir_name.native());

    if (!create_dir_ret) {
        const auto& ec = create_dir_ret.error();
        std::cerr << "Attempting to create " << dir_name << " failed with error " << strerror(ec) << "(" << ec << ")"
                  << std::endl;
    }

    auto file_name     = dir_name / "test.tmp";
    auto file_open_ret = open_file(file_name.native());

    if (!file_open_ret) {
        const auto& ec = file_open_ret.error();
        std::cerr << "Attempting to open " << file_name << " failed with error " << strerror(ec) << "(" << ec << ")"
                  << std::endl;
    } else {
        std::cerr << "Terminating the example early file should have not open" << std::endl;
        return main_return_codes::FAILURE;
    }

    file_open_ret = open_file(file_name.native(), O_CREAT | O_RDWR | O_TRUNC);

    if (!file_open_ret) {
        std::cerr << "Terminating the example early file should have opened" << std::endl;
        return main_return_codes::FAILURE;
    }

    const auto& fd = file_open_ret.result();
    std::cout << "File " << file_name << " has fd " << fd << std::endl;

    std::cout << "Example has successfully ran" << std::endl;
    return main_return_codes::SUCCESS;
}
