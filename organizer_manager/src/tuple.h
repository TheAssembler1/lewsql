#ifndef RECORD_H
#define RECORD_H

#include <cassert>
#include <memory>
#include <vector>

#include "types/tuple_types.h"
#include "types/type.h"

class Tuple {
    public:
    Tuple(TupleCols cols, TupleVals vals, unsigned int page_size) : cols{cols} {
        assert(vals.size() == cols.size());

        for(int i = 0; i < vals.size(); i++) {
            cells.push_back(Type::get_instance(cols[i], vals[i]));
        }

        assert(page_size > size());
    }

    unsigned int size();
    void serialize(uint8_t* dest, bool last);
    void deserailize(uint8_t* src);

    private:
    std::vector<TypeList> cols;
    std::vector<Type*> cells;
};

#endif // RECORD_H
