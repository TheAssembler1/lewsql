#include "tests_helper.h"

#include <logger.h>
#include <unistd.h>
#include <thread>
#include <ostream>
#include <fstream>

void Tests::test_begin() {
    std::lock_guard<std::mutex> lock(init_lock);

    if(!init) {
        Logger::init({&std::cerr});

        init = true;
    }

    pid_t pid = getpid();
    LOG(LogLevel::INFO) << "pid: " << pid << std::endl;

    std::thread::id thread_id = std::this_thread::get_id();
    LOG(LogLevel::INFO) << "thread_id: " << thread_id << std::endl;
}

std::string Tests::get_next_random_string(const std::string& prefix) {
    std::lock_guard<std::mutex> lock(cur_file_index_lock);

    auto rand_string = prefix + std::to_string(Tests::cur_file_index);
    LOG(LogLevel::INFO) << "created rand string: " << rand_string << std::endl;

    Tests::cur_file_index++;

    return rand_string;
}
