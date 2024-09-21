#include "tests_helper.h"

#include <logger.h>

void Tests::test_begin() {
    std::lock_guard<std::mutex> lock(init_lock);

    if(!init) {
        Logger::init({&std::cerr});

        init = true;
    }
}
