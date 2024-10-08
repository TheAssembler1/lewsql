#include "type.h"

#include "uint16_t_type.h"
#include "uint32_t_type.h"
#include "uint8_t_type.h"

const char* Type::get_type_name(TypeList type_list) {
    switch(type_list) {
    case(TypeList::BOOL): return "bool"; break;
    case(TypeList::UINT8_T): return "uint8_t"; break;
    case(TypeList::UINT16_T): return "uint16_t"; break;
    case(TypeList::UINT32_T): return "uint32_t"; break;
    case(TypeList::ARRAY): return "array"; break;
    default: assert(0); break;
    }
}

Type* Type::get_instance(TypeList type_list, void* param) {
    switch(type_list) {
    case(TypeList::UINT8_T): return new Uint8TType(*static_cast<uint8_t*>(param)); break;
    case(TypeList::UINT16_T): return new Uint16TType(*static_cast<uint16_t*>(param)); break;
    case(TypeList::UINT32_T): return new Uint32TType(*static_cast<uint32_t*>(param)); break;
    default: assert(0); break;
    }
}
