#include <buffer_manager.h>
#include <disk_manager.h>
#include <impl/posix_disk_manager.h>
#include <replacement/dumb_alg.h>

#include <iostream>
#include <vector>

int main() {
    unsigned int page_size = 512;
    unsigned int num_pages = 8;

    std::shared_ptr<DiskManager> posix_dmanager{new PosixDiskManager("/home/ta1/src/test_dir", page_size)};
    std::cout << "successfully created disk manager" << std::endl;

    auto disk_id = posix_dmanager->d_create();
    std::cout << "successfully created disk" << std::endl;

    std::shared_ptr<ReplacementAlg> replacment_alg{new DumbAlg{num_pages}};
    BufferManager buf_manager{posix_dmanager, replacment_alg, num_pages};
    try {
        std::cout << std::endl;
        buf_manager.pin(0, 0);
        std::cout << std::endl;
        buf_manager.pin(0, 1);
        std::cout << std::endl;

        BufferManager::print_mem_pool_map(std::cout, buf_manager);
        BufferManager::print_bitmap(std::cout, buf_manager) << std::endl;
    } catch(std::exception& e) {
        std::cerr << e.what() << std::endl;

        BufferManager::print_mem_pool_map(std::cout, buf_manager);
        BufferManager::print_bitmap(std::cout, buf_manager) << std::endl;
    }

    posix_dmanager->d_destroy(disk_id);
    std::cout << "successfully destroyed disk" << std::endl;

    return 0;
}
