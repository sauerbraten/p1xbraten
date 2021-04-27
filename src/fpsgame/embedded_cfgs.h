#include "cube.h"

namespace game {
    template<size_t N>
    struct embeddedfile {
        string name;
        const uint crc;
        const uchar contents[N];
    };
}