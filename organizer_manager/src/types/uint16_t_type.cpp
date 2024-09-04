#include "uint16_t_type.h"

TypeList Uint16TType::get_type() {
    return TypeList::UINT16_T;
};

void Uint16TType::serialize(void* dest) {
    *static_cast<uint16_t*>(dest) = val;
};

Type* Uint16TType::deserialize(void* src) {
    uint16_t val = *static_cast<uint16_t*>(src);
    Uint16TType* instance = new Uint16TType(val);

    return instance;
};

bool Uint16TType::size() {
    return sizeof(uint16_t);
};
