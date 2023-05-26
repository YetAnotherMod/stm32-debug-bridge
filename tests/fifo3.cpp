#include <fifo.h>

#include <iostream>

int main() {
    int result = 0;
    constexpr uint8_t len = 32;
    using DataType = int;
    using SizeType = uint8_t;
    fifo::internal::FifoRaw<DataType, SizeType, len> buf;
    const uint8_t * const base = static_cast<uint8_t *>(static_cast<void *>(buf.dmaPush().addr));

    buf.occupy(5);
    if (buf.size() != 5) {
        std::cerr << "Bad size after occupy " << buf.size() << "\n";
        result = 1;
    }
    buf.drop(4);
    if (buf.size() != 1) {
        std::cerr << "Bad size after drop " << buf.size() << "\n";
        result = 1;
    }
    {
        auto [p, l] = buf.dmaPop();
        uint8_t *ret = static_cast<uint8_t *>(static_cast<void *>(p));
        constexpr size_t dataShift =
            (sizeof(SizeType) * 2) / alignof(DataType) * alignof(DataType) +
            ((sizeof(SizeType) * 2) % alignof(DataType) ? alignof(DataType)
                                                        : 0);
        const uint8_t *good = static_cast<uint8_t *>(static_cast<void *>(&buf)) +
                        (dataShift + sizeof(DataType) * 4);
        if (ret != good) {
            std::cerr << "Bad addr in dmaPop (limit size): " << ret - good
                      << "\n";
            result = 1;
        }
        if (l != 1) {
            std::cerr << "Bad len in dmaPop (limit size): "
                      << static_cast<size_t>(len) << "\n";
            result = 1;
        }
    }
    {
        auto [p, l] = buf.dmaPush();
        uint8_t *ret = static_cast<uint8_t *>(static_cast<void *>(p));

        const uint8_t *good = base  + sizeof(DataType) * 5;
        if (ret != good) {
            std::cerr << "Bad addr in dmaPush (limit border): " << ret - good
                      << "\n";
            result = 1;
        }
        if (l != len - 5) {
            std::cerr << "Bad len in dmaPush (limit border): "
                      << static_cast<size_t>(l) << "\n";
            result = 1;
        }
    }

    buf.occupy(len - 4);

    {
        auto [p, l] = buf.dmaPop();
        uint8_t *ret = static_cast<uint8_t *>(static_cast<void *>(p));
        const uint8_t *good = base  + sizeof(DataType) * 4;
        if (ret != good) {
            std::cerr << "Bad addr in dmaPop (limit border): " << ret - good
                      << "\n";
            result = 1;
        }
        if (l != len - 4) {
            std::cerr << "Bad len in dmaPop (limit border): "
                      << static_cast<size_t>(l) << "\n";
            result = 1;
        }
    }
    {
        auto [p, l] = buf.dmaPush();
        uint8_t *ret = static_cast<uint8_t *>(static_cast<void *>(p));
        const uint8_t *good = base  + sizeof(DataType) * 1;
        if (ret != good) {
            std::cerr << "Bad addr in dmaPush (limit capacity): " << ret - good
                      << "\n";
            result = 1;
        }
        if (l != 3) {
            std::cerr << "Bad len in dmaPush (limit capacity): "
                      << static_cast<size_t>(l) << "\n";
            result = 1;
        }
    }
    return result;
}
