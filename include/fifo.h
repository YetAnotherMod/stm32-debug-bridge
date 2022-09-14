#pragma once

#include <concepts>
#include <cstdint>
#include <limits>
#include <utility>

namespace fifo {

namespace internal {
template <typename DataType, typename SizeType, SizeType N>
requires(std::unsigned_integral<SizeType> && (N > 0) &&
         ((N & (N - 1)) == 0)) class FifoRaw {
  private:
    SizeType head_;
    SizeType tail_;
    DataType data_[N];

  public:
    FifoRaw(void) : head_(0), tail_(0) {}
    SizeType size(void) const { return tail_ - head_; }
    bool isEmpty(void) const { return tail_ == head_; }
    bool empty(void) const { return isEmpty(); }
    bool isFull(void) const { return size() == N; }
    void push(DataType v) requires(std::movable<DataType>) {
        SizeType t = tail_;
        data_[t % N] = std::move(v);
        tail_ = t + 1;
    }
    void push(DataType v) requires(!std::movable<DataType>) {
        SizeType t = tail_;
        data_[t % N] = v;
        tail_ = t + 1;
    }
    DataType pop(void) requires(std::movable<DataType>) {
        SizeType h = head_;
        auto x = std::move(data_[h % N]);
        head_ = h + 1;
        return x;
    }
    DataType pop(void) requires(!std::movable<DataType>) {
        SizeType h = head_;
        auto x = data_[h % N];
        head_ = h + 1;
        return x;
    }
    bool pushSafe(const DataType &v) {
        if (isFull()) {
            return false;
        }
        push(v);
        return true;
    }
    bool pushSafe(DataType &&v) requires(std::movable<DataType>) {
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
