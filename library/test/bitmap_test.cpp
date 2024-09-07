#include <bitmap.h>
#include <register_test.h>
#include <cassert>

#define NUM_BITS 512
#define NUM_BITS_TO_MANIP 13

void bitmap_test() {
    Bitmap bitmap{NUM_BITS};

    assert(bitmap.num_free_bits() == NUM_BITS);
    assert(bitmap.num_taken_bits() == 0);

    for(int i = 0; i < NUM_BITS_TO_MANIP; i++) {
        bitmap.set_bit_val(i, true);
    }

    assert(bitmap.num_taken_bits() == NUM_BITS_TO_MANIP);
    assert(bitmap.num_free_bits() == NUM_BITS - NUM_BITS_TO_MANIP);

    for(int i = 0; i < NUM_BITS_TO_MANIP; i++) {
        bitmap.set_bit_val(i, false);
    }

    assert(bitmap.num_free_bits() == NUM_BITS);
    assert(bitmap.num_taken_bits() == 0);

    bitmap.set_bit_val(5, true);
    assert(bitmap.is_bit_set(5));

    bitmap.set_bit_val(5, false);
    assert(!bitmap.is_bit_set(5));

    uint8_t* test_buffer = static_cast<uint8_t*>(std::malloc(bitmap.get_num_bytes()));

    for(int i = 0; i < NUM_BITS; i++) {
        if(i % 2) {
            bitmap.set_bit_val(i, true);
        } else {
            bitmap.set_bit_val(i, false);
        }
    }

    bitmap.serialize(test_buffer);

    bitmap.clear_bits();

    assert(bitmap.num_free_bits() == NUM_BITS);
    assert(bitmap.num_taken_bits() == 0);

    bitmap.deserialize(test_buffer);

    for(int i = 0; i < NUM_BITS; i++) {
        if(i % 2) {
            assert(bitmap.is_bit_set(i));
        } else {
            assert(!bitmap.is_bit_set(i));
        }
    }

    free(test_buffer);
}
REGISTER_TEST(bitmap_test);
