#include <cstdint>
#include <limits>
#include <utility>

namespace fifo {

namespace internal {
template <typename DataType, typename SizeType, SizeType N> class FifoRaw {
    static_assert(!std::numeric_limits<SizeType>::is_signed,
                  "SizeType must be unsigned");
    static_assert(N > 0, "N must be greater zero");
    static_assert((N & (N - 1)) == 0, "N must be power of 2");

  private:
    volatile SizeType head_;
    volatile SizeType tail_;
    DataType data_[N];

  public:
    FifoRaw(void) : head_(0), tail_(0) {}
    SizeType size(void) { return tail_ - head_; }
    bool isEmpty(void) { return tail_ == head_; }
    bool empty(void) { return isEmpty(); }
    bool isFull(void) { return size() == N; }
    void push(DataType v) {
        SizeType t = tail_ % N;
        data_[t] = std::move(v);
        tail_++;
    }
    DataType pop(void) {
        SizeType h = head_ % N;
        auto x = std::move(data_[h]);
        head_++;
        return x;
    }
    bool pushSafe(const DataType &v) {
        if (isFull()) {
            return false;
        }
        push(v);
        return true;
    }
    bool pushSafe(DataType &&v) {
        if (isFull()) {
            return false;
        }
        push(std::move(v));
        return true;
    }
    struct SafeReturnType {
        DataType data;
        bool result;
    };
    SafeReturnType popSafe() {
        if (isEmpty()) {
            return {DataType(), false};
        }
        return {pop(), true};
    }
};
} // namespace internal
template <typename DataType, std::size_t N>
using Fifo = internal::FifoRaw<DataType, std::size_t, N>;
} // namespace fifo
