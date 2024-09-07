#ifndef BITMAP_H
#define BITMAP_H

#include <stdint.h>
#include <memory>
#include <cstring>
#include <optional>

class Bitmap
{
public:
    Bitmap(unsigned int num_bits);

    void serialize(uint8_t* dest) const;
    void deserialize(uint8_t* src);

    unsigned int num_free_bits() const;
    unsigned int num_taken_bits() const;

    std::optional<unsigned int> get_first_free_bit() const;

    bool is_bit_set(unsigned int bit) const;
    void set_bit_val(unsigned int bit, bool val);

    unsigned int get_num_bits() const { return num_bits; }
    unsigned int get_num_bytes() const { return num_bytes; }

    void clear_bits() { std::fill_n(bitmap.get(), num_bytes, 0); }
private:
    const unsigned int num_bits;
    unsigned int num_bytes;
    std::unique_ptr<uint8_t[]> bitmap;
};

#endif // BITMAP_H
