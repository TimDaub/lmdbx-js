#include "lmdb-store.h"

#ifdef _WIN32
#define ntohl _byteswap_ulong
#define htonl _byteswap_ulong
#endif

// compare items by 32-bit comparison, a is user provided and assumed to be zero terminated/padded
// which allows us to do the full 32-bit comparisons safely
int compareFast(const MDB_val *a, const MDB_val *b) {
    uint32_t* dataA = (uint32_t*) a->mv_data;
    uint32_t* dataB = (uint32_t*) b->mv_data;
    size_t remaining = b->mv_size;
    uint32_t aVal, bVal;
    while(remaining >= 4) {
        aVal = ntohl(*dataA);
        bVal = ntohl(*dataB);
        if (aVal > bVal)
            return 1;
        if (aVal < bVal)
            return -1;
        /*diff = (int64_t) ntohl(*dataA) - (int64_t) ntohl(*dataB);
        if (diff)
            return diff;*/
        dataA++;
        dataB++;
        remaining -= 4;
    }
    if (remaining) {
        aVal = ntohl(*dataA);
        bVal = ntohl(*dataB & (remaining == 2 ? 0x0000ffff : remaining == 1 ? 0x000000ff : 0x00ffffff));
        if (aVal > bVal)
            return 1;
        if (aVal < bVal)
            return -1;
    }
    return a->mv_size - b->mv_size;
}
