#include <fifo.h>

#include <future>
#include <iostream>
#include <memory>

int main() {
    int result = 0;
    constexpr std::uint8_t len = 128;
    constexpr std::size_t circles = 1024;
    fifo::internal::FifoRaw<std::uint16_t, std::uint8_t, len> buf;
    {
        std::cout << "Test async push and pull\n";
        auto pusher = std::async([&buf]() {
            std::size_t i = 0;
            while (i < len * circles) {
                std::uint16_t x = i;
                if (buf.pushSafe(x)) {
                    i++;
                }
            }
        });

        for (std::size_t i = 0; i < len * circles; ++i) {
            auto [x, res] = buf.popSafe();
            while (!res) {
                auto [x_, res_] = buf.popSafe();
                x = x_;
                res = res_;
            }
            if (static_cast<uint16_t>(i) != x) {
                std::cout << "Iteration " << i << " bad data: " << x << "\n";
                result = 1;
            }
        }
    }
    {
        std::cout << "Test async write and read\n";
        auto pusher = std::async([&buf]() {
            std::size_t i = 0;
            while (i < len * circles) {
                std::uint16_t x[len];
                for (size_t j = 0; j < len; ++j) {
                    x[j] = i + j;
                }
                i += buf.write(
                    x, static_cast<std::uint8_t>(
                           std::min<std::size_t>(len, len * circles - i)));
            }
        });

        std::size_t i = 0;

        while (i < len * circles) {
            std::uint16_t  x[len];
            std::size_t readCount =
                buf.read(x, static_cast<std::uint8_t>(
                                std::min<std::size_t>(len, len * circles - i)));
            for (std::size_t j = 0; j < readCount; ++j) {
                if (static_cast<uint16_t>(i + j) != x[j]) {
                    std::cout << "Iteration " << i << ":" << j
                              << " bad data: " << x[j] << "\n";
                    result = 1;
                }
            }
            i += readCount;
        }
    }

    return result;
}
