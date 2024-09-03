#include "tuple.h"
#include "constants.h"

void Tuple::serialize(uint8_t* dest, bool last) {
    uint8_t* cursor = dest;

    // NOTE writing last status
    if(last) {
        *cursor = LAST_TUPLE;
    } else {
        *cursor = NOT_LAST_TUPLE;
    }

    cursor += sizeof(LAST_TUPLE_SIZE);

    for(auto cell: cells) {
        cell->serialize(cursor);
        cursor += cell->size();
    }
}

unsigned int Tuple::size() {
    unsigned int size = 0;
    size += sizeof(LAST_TUPLE_SIZE);

    for(auto cell: cells) {
        size += cell->size();
    }

    return size;
}

void Tuple::deserailize(uint8_t* src) {

}
