#include "core/console.h"

namespace core {

// Reconstructed from sub_1018F410
// This is a formatted output function that respects ios width/fill
std::ostream& console_print(std::ostream& os, const char* str) {
    if (!str)
        return os;

    auto len = static_cast<std::streamsize>(strlen(str));
    auto width = os.width();

    if (width > 0 && width > len) {
        auto fill_count = width - len;
        auto flags = os.flags();

        if ((flags & std::ios::adjustfield) != std::ios::left) {
            for (std::streamsize i = 0; i < fill_count; i++)
                os.put(os.fill());
        }

        os.write(str, len);

        if ((flags & std::ios::adjustfield) == std::ios::left) {
            for (std::streamsize i = 0; i < fill_count; i++)
                os.put(os.fill());
        }
    } else {
        os.write(str, len);
    }

    os.width(0);
    return os;
}

std::ostream& console_print(std::ostream& os, int value) {
    os << value;
    return os;
}

}
