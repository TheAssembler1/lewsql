#include "uint32_t_type.h"

TypeList Uint32TType::get_type() {
    return TypeList::UINT32_T;
};

void Uint32TType::serialize(void* dest) {
    *static_cast<uint32_t*>(dest) = val;
};

Type* Uint32TType::deserialize(void* src) {
    uint32_t val = *static_cast<uint32_t*>(src);
    Uint32TType* instance = new Uint32TType(val);

    return instance;
};

bool Uint32TType::size() {
    return sizeof(uint32_t);
};
