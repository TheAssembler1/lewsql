#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <stdint.h>

constexpr inline uint32_t HEAP_PAGE_START_STAMP{0x01};

constexpr inline uint8_t LAST_TUPLE{0x00};
constexpr inline uint8_t NOT_LAST_TUPLE{0x01};
constexpr inline uint8_t LAST_TUPLE_SIZE{sizeof(LAST_TUPLE)};

#endif // CONSTANTS_H
