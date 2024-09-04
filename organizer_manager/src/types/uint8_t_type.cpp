#include "uint8_t_type.h"

TypeList Uint8TType::get_type() {
    return TypeList::UINT8_T;
};

void Uint8TType::serialize(void* dest) {
    *static_cast<uint8_t*>(dest) = val;
};

Type* Uint8TType::deserialize(void* src) {
    uint8_t val = *static_cast<uint8_t*>(src);
    Uint8TType* instance = new Uint8TType(val);

    return instance;
};

bool Uint8TType::size() {
    return sizeof(uint8_t);
};
