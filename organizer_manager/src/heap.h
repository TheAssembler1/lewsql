#ifndef HEAP_H
#define HEAP_H

#include <disk_manager.h>
#include <disk_manager_error.h>
#include <buffer_manager.h>
#include <memory>
#include <tuple.h>
#include <types/tuple_types.h>
#include <organizer.h>

// NOTE: (DiskId, DiskCursor) = (0, 0) byte 0 is heap stage stamp
//       each tuple starts with whether it is the last in the heap
class Heap: Organizer {
public:
    Heap(std::shared_ptr<DiskManager> disk_manager, std::shared_ptr<BufferManager> buffer_manager,
         std::string table_name, TupleCols cols, unsigned int page_size)
        : disk_manager{disk_manager}, buffer_manager{buffer_manager},
          table_name{table_name}, cols{cols}, page_size{page_size} {

        try {
            disk_id = disk_manager->d_create(table_name);

            std::cout << "created table with name: " << table_name << std::endl;
        } catch(DiskManagerError& e) {
            assert(e.error_code == DiskManagerErrorCode::DISK_ALREADY_EXISTS);

            disk_id = disk_manager->d_load(table_name);

            std::cout << "table already existed loading table with name: " << table_name << std::endl;
        }
    };

    virtual void push_back_record(Tuple tuple) override;

private:
    std::shared_ptr<DiskManager> disk_manager;
    std::shared_ptr<BufferManager> buffer_manager;
    TupleCols cols;
    std::string table_name;
    DiskId disk_id;
    std::vector<Tuple> tuples;
    unsigned int page_size;
};

#endif // HEAP_H
