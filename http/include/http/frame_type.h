#pragma once

#include <cstdint>

namespace algue::http {

enum class FrameType : uint8_t {
    settings = 0x4
};

}
