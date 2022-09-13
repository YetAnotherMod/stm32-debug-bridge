#include <fifo.h>

#include <future>
#include <iostream>
#include <memory>

class NoMove {
  private:
    uint16_t data_;

  public:
    NoMove(uint16_t v = 0) : data_(v){};
    uint16_t operator=(uint16_t v) { return data_ = v; }
    bool operator==(uint16_t v) { return data_ == v; }
    operator uint16_t() { return data_; }
};

int main() {
    int result = 0;
    constexpr std::uint8_t len = 128;
    constexpr std::size_t circles = 1024 * 1024;
    fifo::internal::FifoRaw<NoMove, std::uint8_t, len> buf;
    std::cout << "Test async push and pull\n";
    auto pusher = std::async([&buf]() {
        std::size_t i = 0;
        while (i < len * circles) {
            NoMove x = i;
            if (buf.pushSafe(x)) {
                i++;
            }
        }
    });

    for (std::size_t i = 0; i < len * circles; ++i) {
        while (buf.isEmpty())
            ;
        uint16_t x = buf.pop();
        if (static_cast<uint16_t>(i) != x) {
            std::cout << "Iteration " << i << " bad data: " << x << "\n";
            result = 1;
        }
    }
    return result;
}