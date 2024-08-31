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
        for(unsigned int i = 0; i < num_pages; i++) {
            buf_manager.pin(disk_id, i);
        }

        buf_manager.unpin(disk_id, 32);
        buf_manager.free_avail_pages();
    } catch(std::exception& e) {
        std::cerr << e.what() << std::endl;

        BufferManager::print_mem_pool_map(std::cout, buf_manager);
        BufferManager::print_bitmap(std::cout, buf_manager) << std::endl;
    }

    posix_dmanager->d_destroy(disk_id);
    std::cout << "successfully destroyed disk" << std::endl;

    return 0;
}
