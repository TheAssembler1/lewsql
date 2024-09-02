#ifndef RECORD_H
#define RECORD_H

#include <vector>
#include <memory>
#include <cassert>

#include "types/type.h"

class Tuple {
public:
    Tuple(std::vector<TypeList> cols, std::vector<void*> params, unsigned int page_size): cols{cols} {
        assert(params.size() == cols.size());

        for(int i = 0; i < params.size(); i++) {
            cells.push_back(Type::get_instance(cols[i], params[i]));
        }

        assert(page_size < size());
    }

    unsigned int size();
    void serialize(uint8_t* dest);
    void deserailize(uint8_t* src);

private:
    std::vector<TypeList> cols;
    std::vector<Type*> cells;
};

#endif // RECORD_H
