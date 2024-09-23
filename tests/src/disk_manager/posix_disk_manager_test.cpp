#include "../unit_test_framework.hpp"
#include "../tests_helper.h"

#include <iostream>
#include <disk_manager.h>
#include <impl/posix_disk_manager.h>

using namespace Tests;

class PosixDiskTestHelper {
public:
    PosixDiskTestHelper(DiskManager::PosixDiskManager& posix_disk_manager)
        : m_posix_disk_manager{posix_disk_manager}, m_disk_name{Tests::get_next_random_string(Tests::DiskManagerOpts::disk_prefix)} {
        auto disk_id_res = posix_disk_manager.create(m_disk_name);
        m_disk_id = disk_id_res.get_value();
    }

    DiskId get_disk_id() {
        return m_disk_id;
    }

    ~PosixDiskTestHelper() {
        auto del_res = m_posix_disk_manager.destroy(m_disk_id);
    }

private:
    DiskManager::PosixDiskManager& m_posix_disk_manager;
    std::string m_disk_name;
    DiskId m_disk_id;
};

TEST(posix_disk_manager_create) {
    test_begin();

    DiskManager::PosixDiskManager posix_disk_manager{Tests::DiskManagerOpts::temp_storage_dir_path,
                                Tests::DiskManagerOpts::page_size,
                                Tests::DiskManagerOpts::max_disk_size};
}

TEST(posix_disk_manager_disk_create_destroy) {
    test_begin();

    DiskManager::PosixDiskManager posix_disk_manager{Tests::DiskManagerOpts::temp_storage_dir_path,
                                Tests::DiskManagerOpts::page_size,
                                Tests::DiskManagerOpts::max_disk_size};

    auto disk_name = Tests::get_next_random_string(Tests::DiskManagerOpts::disk_prefix);
    ASSERT_FALSE(posix_disk_manager.exists(disk_name).get_value());

    auto disk_id_res = posix_disk_manager.create(disk_name);
    ASSERT_TRUE(disk_id_res.is_value());
    ASSERT_TRUE(posix_disk_manager.exists(disk_name).get_value());
    ASSERT_TRUE(posix_disk_manager.is_loaded(disk_id_res.get_value()).get_value());

    auto del_res = posix_disk_manager.destroy(disk_id_res.get_value());
    ASSERT_TRUE(del_res.is_value());
    ASSERT_FALSE(posix_disk_manager.exists(disk_name).get_value());
    ASSERT_FALSE(posix_disk_manager.is_loaded(disk_id_res.get_value()).get_value());
}

TEST(posix_disk_manager_load_and_unload) {
    test_begin();

    DiskManager::PosixDiskManager posix_disk_manager{Tests::DiskManagerOpts::temp_storage_dir_path,
                                Tests::DiskManagerOpts::page_size,
                                Tests::DiskManagerOpts::max_disk_size};

    auto disk_name = Tests::get_next_random_string(Tests::DiskManagerOpts::disk_prefix);

    auto disk_id_res = posix_disk_manager.create(disk_name);
    ASSERT_TRUE(posix_disk_manager.is_loaded(disk_id_res.get_value()).get_value());

    ASSERT_TRUE(posix_disk_manager.unload(disk_id_res.get_value()).is_value());
    ASSERT_FALSE(posix_disk_manager.is_loaded(disk_id_res.get_value()).get_value());

    auto disk_id = posix_disk_manager.load(disk_name);
    ASSERT_TRUE(disk_id.is_value());
    ASSERT_TRUE(posix_disk_manager.is_loaded(disk_id.get_value()).get_value());

    posix_disk_manager.destroy(disk_id.get_value());
}

TEST(posix_disk_manager_disk_single_page_read_write) {
    test_begin();

    DiskManager::PosixDiskManager posix_disk_manager{Tests::DiskManagerOpts::temp_storage_dir_path,
                                Tests::DiskManagerOpts::page_size,
                                Tests::DiskManagerOpts::max_disk_size};
    PosixDiskTestHelper posix_disk_test_helper{posix_disk_manager};

    auto written_bytes = new uint8_t[Tests::DiskManagerOpts::page_size];
    for(int i = 0; i < Tests::DiskManagerOpts::page_size; i++) {
        written_bytes[i] = i % sizeof(uint8_t);
    }

    ASSERT_TRUE(posix_disk_manager.write(posix_disk_test_helper.get_disk_id(), 0, written_bytes).is_value());

    auto read_bytes = new uint8_t[Tests::DiskManagerOpts::page_size];

    ASSERT_TRUE(posix_disk_manager.read(posix_disk_test_helper.get_disk_id(), 0, read_bytes).is_value());

    for(int i = 0; i < Tests::DiskManagerOpts::page_size; i++) {
        ASSERT_EQUAL(read_bytes[i], i % sizeof(uint8_t));
    }

    ASSERT_EQUAL(posix_disk_manager.disk_size(posix_disk_test_helper.get_disk_id()).get_value(), Tests::DiskManagerOpts::page_size);

    delete[] written_bytes;
    delete[] read_bytes;
}

TEST(posix_disk_manager_disk_multi_page_read_write) {
    test_begin();

    DiskManager::PosixDiskManager posix_disk_manager{Tests::DiskManagerOpts::temp_storage_dir_path,
                                Tests::DiskManagerOpts::page_size,
                                Tests::DiskManagerOpts::max_disk_size};
    PosixDiskTestHelper posix_disk_test_helper{posix_disk_manager};

    auto written_bytes = new uint8_t[Tests::DiskManagerOpts::page_size];
    auto read_bytes = new uint8_t[Tests::DiskManagerOpts::page_size];

    for(int cur_page = 0; cur_page < Tests::DiskManagerOpts::max_disk_size - 1; cur_page++) {
        for(int i = 0; i < Tests::DiskManagerOpts::page_size; i++) {
            written_bytes[i] = i % sizeof(uint8_t);
        }

        ASSERT_TRUE(posix_disk_manager.write(posix_disk_test_helper.get_disk_id(), cur_page, written_bytes).is_value());
        ASSERT_TRUE(posix_disk_manager.read(posix_disk_test_helper.get_disk_id(), cur_page, read_bytes).is_value());

        for(int i = 0; i < Tests::DiskManagerOpts::page_size; i++) {
            ASSERT_EQUAL(read_bytes[i], i % sizeof(uint8_t));
        }

        ASSERT_EQUAL(posix_disk_manager.disk_size(posix_disk_test_helper.get_disk_id()).get_value(), Tests::DiskManagerOpts::page_size * (cur_page + 1));
    }

    delete[] written_bytes;
    delete[] read_bytes;
}

TEST(posix_disk_manager_disk_exceeds_max_size) {
    test_begin();

    DiskManager::PosixDiskManager posix_disk_manager{Tests::DiskManagerOpts::temp_storage_dir_path,
                                Tests::DiskManagerOpts::page_size,
                                Tests::DiskManagerOpts::max_disk_size};
    PosixDiskTestHelper posix_disk_test_helper{posix_disk_manager};

    auto written_bytes = new uint8_t[Tests::DiskManagerOpts::page_size];
    auto write_res = posix_disk_manager.write(posix_disk_test_helper.get_disk_id(), Tests::DiskManagerOpts::max_disk_size / Tests::DiskManagerOpts::page_size, written_bytes);
    ASSERT_TRUE(write_res.is_error());
    ASSERT_EQUAL(write_res.get_error().error_code, DiskManagerErrorCode::DISK_EXCEEDS_MAX_SIZE);

    delete[] written_bytes;
}
