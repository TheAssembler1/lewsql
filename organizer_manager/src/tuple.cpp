#include "tuple.h"

void Tuple::serialize(uint8_t* dest) {
    uint8_t* cursor = dest;
    for(auto cell: cells) {
        cell->serialize(cursor);
        cursor += cell->size();
    }
}

unsigned int Tuple::size() {
    unsigned int size = 0;
    for(auto cell: cells) {
        size += cell->size();
    }

    return size;
}

void Tuple::deserailize(uint8_t* src) {

}
