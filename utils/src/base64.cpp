#include "utils/base64.h"

namespace algue::utils {

std::string base64(std::string_view input)
{
    constexpr std::string_view kEncodeLookup = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    constexpr char kPadCharacter = '=';

    std::string encoded;
    encoded.reserve(((input.size() / 3) + (input.size() % 3 > 0)) * 4);

    uint32_t temp{};
    const char* it = input.data();

    for (size_t i = 0; i < input.size() / 3; ++i) {
        temp = (*it++) << 16;
        temp += (*it++) << 8;
        temp += (*it++);
        encoded.append(1, kEncodeLookup[(temp & 0x00FC0000) >> 18]);
        encoded.append(1, kEncodeLookup[(temp & 0x0003F000) >> 12]);
        encoded.append(1, kEncodeLookup[(temp & 0x00000FC0) >> 6]);
        encoded.append(1, kEncodeLookup[(temp & 0x0000003F)]);
    }

    switch (input.size() % 3) {
    case 1:
        temp = (*it++) << 16;
        encoded.append(1, kEncodeLookup[(temp & 0x00FC0000) >> 18]);
        encoded.append(1, kEncodeLookup[(temp & 0x0003F000) >> 12]);
        encoded.append(2, kPadCharacter);
        break;
    case 2:
        temp = (*it++) << 16;
        temp += (*it++) << 8;
        encoded.append(1, kEncodeLookup[(temp & 0x00FC0000) >> 18]);
        encoded.append(1, kEncodeLookup[(temp & 0x0003F000) >> 12]);
        encoded.append(1, kEncodeLookup[(temp & 0x00000FC0) >> 6]);
        encoded.append(1, kPadCharacter);
        break;
    }

    return encoded;
}

}  // namespace algue::utils
