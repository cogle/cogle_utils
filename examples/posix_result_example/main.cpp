#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

#include <iostream>
#include <string>
#include <utils/result.hxx>

using namespace cogle::utils::result;

namespace main_return_codes {
constexpr auto SUCCESS = 0;
constexpr auto FAILURE = -1;
}  // namespace main_return_codes

Result<int, int> open_file(const std::string& file_name, const int oflag = O_RDWR) {
    auto fd = open(file_name.c_str(), oflag, 0644);

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

    const std::string file_name{"/tmp/example_file.txt"};
    auto file_open_ret = open_file(file_name);

    if (!file_open_ret) {
        const auto& ec = file_open_ret.error();
        std::cerr << "Attempting to open " << file_name << " failed with error " << strerror(ec) << "(" << ec << ")"
                  << std::endl;
    } else {
        std::cerr << "Terminating the example early file should have not open" << std::endl;
        return main_return_codes::FAILURE;
    }

    file_open_ret = open_file(file_name, O_CREAT | O_RDWR | O_TRUNC);

    if (!file_open_ret) {
        std::cerr << "Terminating the example early file should have opened" << std::endl;
        return main_return_codes::FAILURE;
    }

    const auto& fd = file_open_ret.result();
    std::cout << "File " << file_name << " has fd " << fd << std::endl;

    std::cout << "Example has successfully ran" << std::endl;
    return main_return_codes::SUCCESS;
}
