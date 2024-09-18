#ifndef HEAP_H
#define HEAP_H

#include <buffer_manager.h>
#include <disk_manager.h>
#include <disk_manager_error.h>
#include <disk_manager_types.h>
#include <memory>
#include <optional>
#include <organizer.h>
#include <tuple.h>
#include <types/tuple_types.h>

// NOTE: (DiskId, DiskCursor) = (0, 0) bytes 0-3 are heap stage stamp
//       each tuple starts with whether it is the last in the heap
class Heap final : Organizer {
    public:
    Heap(const Heap&) = delete;
    Heap& operator=(const Heap&) = delete;
    Heap(const Heap&&) = delete;
    Heap&& operator=(const Heap&&) = delete;

    Heap(std::shared_ptr<DiskManager::DiskManager> disk_manager,
    std::shared_ptr<BufferManager> buffer_manager,
    std::string table_name,
    TupleCols cols,
    unsigned int page_size);

    ~Heap() {
        m_disk_manager->unload(m_disk_id);
    }

    virtual void get_next_tuple() const override;
    virtual void insert_tuple(const Tuple& tuple) override;
    virtual void delete_tuple() override;

    // FIXME: actually validate tuple
    virtual bool is_valid_tuple(const Tuple& tuple) const override {
        return true;
    }

    private:
    std::optional<unsigned int> find_free_page();
    void append_to_full_pages(DiskPageCursor disk_page_cursor);

    // NOTE: this is the disk name on fs
    const std::string m_table_name;
    // NOTE: this is the disk name on fs for bitmap
    const std::string m_table_bitmap_name;

    DiskId m_disk_id;

    const std::shared_ptr<DiskManager::DiskManager> m_disk_manager;
    const std::shared_ptr<BufferManager> m_buffer_manager;

    // NOTE: specifies the types stored by the table
    TupleCols cols;
    unsigned int page_size;
};

#endif // HEAP_H
