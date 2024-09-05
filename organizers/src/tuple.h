#ifndef RECORD_H
#define RECORD_H

#include <cassert>
#include <memory>
#include <vector>

#include "types/tuple_types.h"
#include "types/type.h"

class Tuple {
    public:
    Tuple(TupleVals cells) : cells{std::move(cells)} {}

    unsigned int size() const;
    void serialize(uint8_t* dest) const;
    void deserailize(uint8_t* src);

    const char* get_cell_type(int index) const {
        return Type::get_type_name(cells[index]->get_type());
    }

    unsigned int num_cells() const {
        return cells.size();
    }

    private:
    TupleVals cells;
};

#endif // RECORD_H
