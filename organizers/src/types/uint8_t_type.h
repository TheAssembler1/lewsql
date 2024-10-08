#ifndef UINT8_T_TYPE_H
#define UINT8_T_TYPE_H

#include <stdint.h>

#include "type.h"

class Uint8TType final : public Type {
    public:
    Uint8TType(uint8_t src) : val{src} {
    }

    virtual TypeList get_type() override;
    virtual void serialize(void* dest) override;
    virtual Type* deserialize(void* src) override;
    virtual bool size() override;

    private:
    uint8_t val;
};

#endif // UINT8_T_TYPE_H
