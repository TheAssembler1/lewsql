#ifndef RECORD_H
#define RECORD_H

#include <cassert>
#include <memory>
#include <vector>

#include "types/type.h"
#include "types/tuple_types.h"

class Tuple {
    public:
    Tuple(TupleVals cells, unsigned int page_size): cells{std::move(cells)} {
        assert(page_size > size());
    }

    unsigned int size();
    void serialize(uint8_t* dest, bool last);
    void deserailize(uint8_t* src);

    private:
    TupleVals cells;
};

#endif // RECORD_H
