#ifndef HEAP_H
#define HEAP_H

#include <buffer_manager.h>
#include <disk_manager.h>
#include <disk_manager_error.h>
#include <memory>
#include <organizer.h>
#include <tuple.h>
#include <types/tuple_types.h>

// NOTE: (DiskId, DiskCursor) = (0, 0) bytes 0-3 are heap stage stamp
//       each tuple starts with whether it is the last in the heap
class Heap : Organizer {
    public:
    Heap(std::shared_ptr<DiskManager> disk_manager,
    std::shared_ptr<BufferManager> buffer_manager,
    std::string table_name,
    TupleCols cols,
    unsigned int page_size);

    virtual void push_back_record(Tuple tuple) override;

    virtual bool valid_record(const Tuple& tuple) const override;

    private:
    // NOTE: this is the disk name on fs
    const std::string table_name;
    DiskId disk_id;

    const std::shared_ptr<DiskManager> disk_manager;
    const std::shared_ptr<BufferManager> buffer_manager;

    // NOTE: specifies the types stored by the table
    TupleCols cols;
    unsigned int page_size;
};

#endif // HEAP_H
