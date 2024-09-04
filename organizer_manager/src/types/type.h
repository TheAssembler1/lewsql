#ifndef TYPE_H
#define TYPE_H

#include <cassert>
#include <memory>
#include <stdint.h>

enum class TypeList {
    BOOL,
    UINT8_T,
    UINT16_T,
    UINT32_T,
    ARRAY,
};

class Type {
    public:
    static const char* get_type_name(TypeList type_list);
    static Type* get_instance(TypeList type_list, void* param);

    virtual void serialize(void* dest) = 0;
    virtual Type* deserialize(void* src) = 0;

    virtual bool size() = 0;
    virtual TypeList get_type() = 0;
};

#endif // TYPE_H
