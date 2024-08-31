#include <buffer_manager.h>
#include <disk_manager.h>
#include <impl/posix_disk_manager.h>
#include <replacement/dumb_alg.h>

#include <iostream>
#include <vector>

int main() {
    std::shared_ptr<DiskManager> posix_dmanager{new PosixDiskManager("/home/ta1/src/test_dir", 512)};
    std::cout << "successfully created disk manager" << std::endl;

    auto disk_id = posix_dmanager->d_create();
    std::cout << "successfully created disk" << std::endl;

    try {
        std::shared_ptr<ReplacementAlg> replacment_alg{new DumbAlg{512}};
        BufferManager buf_manager{posix_dmanager, replacment_alg, 512};

        buf_manager.pin(disk_id, 0);
        buf_manager.unpin(disk_id, 0);

        BufferManager::print_bitmap(std::cout, buf_manager) << std::endl;
    } catch(std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    posix_dmanager->d_destroy(disk_id);
    std::cout << "successfully destroyed disk" << std::endl;

    return 0;
}
