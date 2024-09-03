#ifndef TUPLE_TYPES_H
#define TUPLE_TYPES_H

#include <vector>
#include <string>

#include "type.h"

// NOTE: void pointer to the actuall value to be interpreted by each type
using Cell = void*;

using TupleVals = std::vector<Cell>;
using TupleCols = std::vector<TypeList>;

#endif // TUPLE_TYPES_H
