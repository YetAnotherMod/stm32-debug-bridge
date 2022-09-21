#include <fifo.h>

#include <iostream>
#include <memory>

int main() {
    int result = 0;
    constexpr std::uint8_t len = 128;
    constexpr std::size_t circles = 1024;
    fifo::internal::FifoRaw<std::unique_ptr<std::uint16_t>, std::uint8_t, len>
        buf;
    std::cout << "Test single\n";
    for (std::size_t i = 0; i < len * circles; ++i) {
        if (!buf.empty()) {
            std::cout << "Iteration " << i << " buf not empty\n";
            result = 1;
        }
        buf.push(std::make_unique<std::uint16_t>(i));

        if (buf.empty()) {
            std::cout << "Iteration " << i << " buf empty\n";
            result = 1;
        }
        std::uint16_t x = *buf.pop();
        if (x != static_cast<std::uint16_t>(i)) {
            std::cout << "Iteration " << i << " bad value in buf: " << x
                      << "\n";
            result = 1;
        }
    }
    if (result) {
        return result;
    }
    std::cout << "Test full len\n";
    for (std::size_t i = 0; i < circles; ++i) {
        if (!buf.empty()) {
            std::cout << "Iteration " << i << " buf not empty\n";
            result = 1;
        }
        if (buf.isFull()) {
            std::cout << "Iteration " << i << " buf full\n";
            result = 1;
        }
        for (std::size_t j = 0; j < len; ++j) {
            if (buf.isFull()) {
                std::cout << "Iteration up " << i << ":" << j << " buf full\n";
                result = 1;
            }
            buf.push(std::make_unique<std::uint16_t>(j));

            if (buf.empty()) {
                std::cout << "Iteration up " << i << ":" << j << " buf empty\n";
                result = 1;
            }
        }

        if (!buf.isFull()) {
            std::cout << "Iteration " << i << " buf not Full\n";
            result = 1;
        }
        for (std::size_t j = 0; j < len; ++j) {
            if (buf.empty()) {
                std::cout << "Iteration down " << i << ":" << j
                          << " buf empty\n";
                result = 1;
            }
            std::uint16_t x = *buf.pop();
            if (x != j) {
                std::cout << "Iteration down " << i << ":" << j
                          << " bad value in buf: " << x << "\n";
                result = 1;
            }
            if (buf.isFull()) {
                std::cout << "Iteration down " << i << ":" << j
                          << " buf full\n";
                result = 1;
            }
        }
        if (!buf.empty()) {
            std::cout << "Iteration " << i << " buf not empty\n";
            result = 1;
        }
    }
    if (result) {
        return result;
    }
    std::cout << "Test safe push and pull\n";
    for (std::size_t i = 0; i < circles; ++i) {
        if (!buf.empty()) {
            std::cout << "Iteration " << i << " buf not empty\n";
            result = 1;
        }
        if (buf.isFull()) {
            std::cout << "Iteration " << i << " buf full\n";
            result = 1;
        }
        for (std::size_t j = 0; j < len; ++j) {
            if (buf.isFull()) {
                std::cout << "Iteration up " << i << ":" << j << " buf full\n";
                result = 1;
            }
            if (!buf.pushSafe(std::make_unique<std::uint16_t>(j))) {
                std::cout << "Iteration up " << i << ":" << j
                          << " push false\n";
                result = 1;
            }

            if (buf.empty()) {
                std::cout << "Iteration up " << i << ":" << j << " buf empty\n";
                result = 1;
            }
        }

        if (!buf.isFull()) {
            std::cout << "Iteration " << i << " buf not Full\n";
            result = 1;
        }

        auto y = std::make_unique<std::uint16_t>(len);
        if (buf.pushSafe(std::move(y))) {
            std::cout << "Iteration " << i << " pushSafe into full buf true\n";
            result = 1;
        }
        if (!y || *y != len) {
            std::cout << "Iteration " << i
                      << " pushSafe into full buf corrupt input data\n";
            result = 1;
        }

        for (std::size_t j = 0; j < len; ++j) {
            if (buf.empty()) {
                std::cout << "Iteration down " << i << ":" << j
                          << " buf empty\n";
                result = 1;
            }
            auto [u, r] = buf.popSafe();
            if (!r) {
                {
                    std::cout << "Iteration down " << i << ":" << j
                              << " popSafe from non empty buf false\n";
                    result = 1;
                }
            }
            if (u) {
                auto x = *u;
                if (x != j) {
                    std::cout << "Iteration down " << i << ":" << j
                              << " bad value in buf: " << x << "\n";
                    result = 1;
                }
            } else {
                std::cout << "Iteration down " << i << ":" << j
                          << " popSafe from non empty return bad data\n";
                result = 1;
            }
            if (buf.isFull()) {
                std::cout << "Iteration down " << i << ":" << j
                          << " buf full\n";
                result = 1;
            }
        }
        if (!buf.empty()) {
            std::cout << "Iteration " << i << " buf not empty\n";
            result = 1;
        }
        auto [u, r] = buf.popSafe();
        if (r) {
            std::cout << "Iteration " << i << " popSafe from empty buf true\n";
            result = 1;
        }
        if (u) {
            std::cout << "Iteration " << i
                      << " popSafe from empty buf has data\n";
            result = 1;
        }
    }
    return result;
}
