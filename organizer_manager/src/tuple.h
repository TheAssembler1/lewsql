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
    void serialize(uint8_t* dest);
    void deserailize(uint8_t* src);

    const char* get_cell_type(int index) {
        return Type::get_type_name(cells[index]->get_type());
    }

    unsigned int num_cells() const {
        return cells.size();
    }

    private:
    TupleVals cells;
};

#endif // RECORD_H
