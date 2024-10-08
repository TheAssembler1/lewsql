#ifndef BITMAP_H
#define BITMAP_H

#include <cstring>
#include <memory>
#include <optional>
#include <stdint.h>

namespace Library::Bitmap {

class Bitmap {
    public:
    Bitmap(unsigned int num_bits);

    void serialize(uint8_t* dest) const;
    void deserialize(uint8_t* src);

    unsigned int get_num_free_bits() const;
    unsigned int get_num_taken_bits() const;

    std::optional<unsigned int> get_first_free_bit() const;

    bool get_bit_val(unsigned int bit) const;
    void set_bit_val(unsigned int bit, bool val);

    friend std::ostream& operator<<(std::ostream& os, const Bitmap& bitmap);

    unsigned int get_num_bits() const {
        return num_bits;
    }
    unsigned int get_num_bytes() const {
        return num_bytes;
    }

    void clear_bits() {
        std::fill_n(bitmap.get(), num_bytes, 0);
    }

    private:
    const unsigned int num_bits;
    unsigned int num_bytes;
    std::unique_ptr<uint8_t[]> bitmap;
};

} // namespace Library::Bitmap

using Bitmap = Library::Bitmap::Bitmap;

#endif // BITMAP_H
