#pragma once

#include <algorithm>
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
    SizeType capacity(void) { return N; }
    void clear(void) {
        head_ = 0;
        tail_ = 0;
    }
    void drop(SizeType len) { head_ += len; }
    void occupy(SizeType len) { tail_ += len; }
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
    SizeType write(DataType *v,
                   SizeType count) requires(std::movable<DataType>) {
        SizeType t = tail_;
        SizeType toPlace = std::min(count, static_cast<SizeType>(N - size()));
        for (SizeType left = toPlace; left > 0; --left) {
            data_[t++ % N] = std::move(*v++);
        }
        tail_ = t;
        return toPlace;
    }
    SizeType write(const DataType *v, SizeType count) requires(
        std::constructible_from<DataType, const DataType &>
            &&std::convertible_to<const DataType &, DataType>
                &&std::constructible_from<DataType, const DataType>
                    &&std::convertible_to<const DataType, DataType>) {
        SizeType t = tail_;
        SizeType toPlace = std::min(count, static_cast<SizeType>(N - size()));
        for (SizeType left = toPlace; left > 0; --left) {
            data_[t++ % N] = *v++;
        }
        tail_ = t;
        return toPlace;
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
    SizeType read(DataType *v,
                  SizeType count) requires(std::movable<DataType>) {
        SizeType h = head_;
        SizeType toPlace = std::min(count, size());
        for (SizeType left = toPlace; left > 0; --left) {
            *v++ = std::move(data_[h++ % N]);
        }
        head_ = h;
        return toPlace;
    }
    SizeType read(DataType *v,
                  SizeType count) requires(!std::movable<DataType>) {
        SizeType h = head_;
        SizeType toPlace = std::min(count, size());
        for (SizeType left = toPlace; left > 0; --left) {
            *v++ = data_[h++ % N];
        }
        head_ = h;
        return toPlace;
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
    struct PopSafeReturnType {
        DataType data;
        bool result;
    };
    PopSafeReturnType popSafe() {
        if (isEmpty()) {
            return {DataType(), false};
        }
        return {pop(), true};
    }
    struct DmaReturnType{
        DataType *addr;
        SizeType len;
    };
    DmaReturnType dmaPop(void) requires(std::copyable<DataType>){
        return {&data_[head_%N],std::min(static_cast<SizeType>(N-head_%N),size())};
    }
    void dmaPopApply(SizeType len){
        drop(len);
    }
    DmaReturnType dmaPush() requires(std::copyable<DataType>){
        SizeType len_ = std::min(static_cast<SizeType>(N-tail_%N),static_cast<SizeType>(N-size()));
        return {&data_[tail_%N],len_};
    }
    void dmaPushApply(SizeType len){
        occupy(len);
    }
};
} // namespace internal
template <typename DataType, std::size_t N>
using Fifo = internal::FifoRaw<DataType, std::size_t, N>;
} // namespace fifo
