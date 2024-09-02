#ifndef RECORD_H
#define RECORD_H

#include <vector>
#include <memory>

#include "types/type.h"

class Tuple {
public:
    void serialize(uint8_t* dest);
    void deserailize(uint8_t* src);

private:
    std::vector<std::unique_ptr<Type>> cells;
};

#endif // RECORD_H
