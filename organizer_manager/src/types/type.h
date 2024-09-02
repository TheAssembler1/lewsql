#ifndef TYPE_H
#define TYPE_H

#include <stdint.h>
#include <memory>
#include <cassert>

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

    virtual void serialize(uint8_t* dest) = 0;
    virtual Type* deserialize(void* src) = 0;

    virtual const char* type_name() = 0;

    virtual bool is_bool() {
        return false;
    };

    virtual bool is_uint8_t() {
        return false;
    };

    virtual bool is_uint16_t() {
        return false;
    };

    virtual bool is_uint32_t() {
        return false;
    };

    virtual bool is_uint64_t() {
        return false;
    };

    virtual bool is_array() {
        return false;
    };


    virtual bool size() = 0;
};

#endif // TYPE_H
