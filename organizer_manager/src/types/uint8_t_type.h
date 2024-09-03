#ifndef UINT8_T_TYPE_H
#define UINT8_T_TYPE_H

#include <stdint.h>

#include "type.h"

class Uint8TType: public Type {
public:
    Uint8TType(void* src): val{*static_cast<uint8_t*>(src)} {}

    virtual const char* type_name() override;

    virtual void serialize(uint8_t* dest) override;
    virtual Type* deserialize(void* src) override;

    virtual bool is_uint8_t() override;
    virtual bool size() override;

private:
    uint8_t val;
};

#endif // UINT8_T_TYPE_H
