#ifndef UINT16_T_TYPE_H
#define UINT16_T_TYPE_H

#include <stdint.h>

#include "type.h"

class Uint16TType final : public Type {
    public:
    Uint16TType(uint16_t src) : val{src} {}

    virtual TypeList get_type() override;
    virtual void serialize(void* dest) override;
    virtual Type* deserialize(void* src) override;
    virtual bool size() override;

    private:
    uint16_t val;
};


#endif // UINT16_T_TYPE_H
