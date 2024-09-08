#include "bitmap.h"

#define IS_BIT_SET(byte, bit) ((byte) & (1 << (bit)))
#define SET_BIT(byte, bit) ((byte) |= (1 << (bit)))
#define UNSET_BIT(byte, bit) ((byte) &= ~(1 << (bit)))

Bitmap::Bitmap(unsigned int num_bits): num_bytes{num_bits / 8}, num_bits{num_bits} {
    if(num_bits % 2 != 0) {
        num_bytes++;
    }

    bitmap = std::make_unique<uint8_t[]>(num_bits / 8);
    std::fill_n(bitmap.get(), num_bytes, 0);
}

void Bitmap::serialize(uint8_t* dest) const {
    std::memcpy(dest, bitmap.get(), num_bytes);
}

void Bitmap::deserialize(uint8_t* src) {
    std::memcpy(bitmap.get(), src, num_bytes);
}

unsigned int Bitmap::num_free_bits() const {
    unsigned int free_bits = 0;
    for(int i = 0; i < num_bits; i++) {
        if(!IS_BIT_SET(bitmap[i / 8], i % 8)) {
            free_bits++;
        }
    }

    return free_bits;
}

unsigned int Bitmap::num_taken_bits() const {
    return num_bits - num_free_bits();
}

std::optional<unsigned int> Bitmap::get_first_free_bit() const {
    for(int i = 0; i < num_bits; i++) {
        if(!IS_BIT_SET(bitmap[i / 8], i % 8)) {
            return i;
        }
    }

    return std::nullopt;
}

bool Bitmap::is_bit_set(unsigned int bit) const {
    return IS_BIT_SET(bitmap[bit / 8], bit % 8);
}

void Bitmap::set_bit_val(unsigned int bit, bool val) {
    if (val) {
        SET_BIT(bitmap[bit / 8], bit % 8);
    } else {
        UNSET_BIT(bitmap[bit / 8], bit % 8);
    }
}
