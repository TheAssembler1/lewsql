#ifndef UINT32_T_TYPE_H
#define UINT32_T_TYPE_H

#include <stdint.h>

#include "type.h"

class Uint32TType final : public Type {
    public:
    Uint32TType(uint32_t src) : val{src} {
    }

    virtual TypeList get_type() override;
    virtual void serialize(void* dest) override;
    virtual Type* deserialize(void* src) override;
    virtual bool size() override;

    private:
    uint32_t val;
};

#endif // UINT32_T_TYPE_H
