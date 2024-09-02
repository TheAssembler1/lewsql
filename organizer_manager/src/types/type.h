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
    static const char* get_type_name(TypeList type_list) {
        switch(type_list) {
            case(TypeList::BOOL):
                return "bool";
                break;
            case(TypeList::UINT8_T):
                return "uint8_t";
                break;
            case(TypeList::UINT16_T):
                return "uint16_t";
                break;
            case(TypeList::UINT32_T):
                return "uint32_t";
                break;
            case(TypeList::ARRAY):
                return "array";
                break;
            default:
                assert(0);
                break;
        }
    }

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
