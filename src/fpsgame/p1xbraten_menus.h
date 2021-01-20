#include "cube.h"

namespace game {
    struct embeddedfile {
        string name;
        const uint crc;
        const size_t len;
        const uchar contents[];
    };
}