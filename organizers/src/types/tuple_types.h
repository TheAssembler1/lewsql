#ifndef TUPLE_TYPES_H
#define TUPLE_TYPES_H

#include <string>
#include <vector>
#include <stdint.h>

#include "type.h"

using TupleCols = std::vector<TypeList>;
using TupleVals = std::vector<std::unique_ptr<Type>>;
using TupleId = uint32_t;

#endif // TUPLE_TYPES_H
