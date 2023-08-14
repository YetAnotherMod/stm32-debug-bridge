#pragma once

#include <bit>
#include <cstdint>
#include <functional>
#include <initializer_list>
#include <utility>
#include <array>
#include <stdexcept>

namespace staticMap {
namespace internal {
template <bool a, bool... x> bool And() { return a && And<x...>(); }
template <bool a> bool And() { return a; }
} // namespace internal
template <typename KeyType, typename ValueType, std::size_t Capacity,
          std::size_t TableSize = (1 << (std::bit_width(Capacity) - 1)),
          typename Hash = std::hash<KeyType>>
class StaticMap {
  public:
    using storageType = std::pair<KeyType, ValueType>;

    ValueType &operator[](const KeyType &k) {
        auto ind = find(k);
        if (ind == nullptr) {
            ind = insert({k, ValueType()});
        }
        if ( ind == nullptr ){
            throw std::out_of_range("");
        }
        return ind->second;
    }

    storageType *insert(storageType v) {
        if (push(std::move(v))) {
            return &values_[size_-1];
        }else{
            return nullptr;
        }
    }

    constexpr StaticMap(std::initializer_list<storageType> e = {},
                        Hash hash = Hash())
        : hash_(hash), size_(0) {
        for (auto &i : hashTable_) {
            i = -1;
        }
        for (auto &i : valuesTable_) {
            i = -1;
        }
        for (auto &i : e) {
            if ( !push(std::move(i)) ){
                throw std::bad_exception ();
            }
        }
    }

    std::size_t size() const { return size_; }
    std::size_t capacity() const { return Capacity; }

    storageType *find(const KeyType &k) {
        std::size_t hash = hash_(k) % TableSize;
        std::size_t p = hashTable_[hash];
        while ((p != size_t(-1)) && (values_[p].first != k)) {
            p = valuesTable_[p];
        }
        return p == std::size_t(-1) ? nullptr : &values_[p];
    }

    const storageType *find(const KeyType &k) const {
        std::size_t hash = hash_(k) % TableSize;
        std::size_t p = hashTable_[hash];
        while ((p != std::size_t(-1)) && (values_[p].first != k)) {
            p = valuesTable_[p];
        }
        return p == std::size_t(-1) ? nullptr : &values_[p];
    }

    const storageType * begin() const {
        return values_.begin();
    }

    const storageType * end() const {
        return values_.begin()+size_;
    }

  private:
    constexpr bool push (storageType v){
        if (size_ >= Capacity) {
            return false;
        }
        std::size_t hash = hash_(v.first) % TableSize;
        std::size_t p = hashTable_[hash];
        if ( p == size_t(-1) ){
            hashTable_[hash] = size_;
        }else{
            if ( values_[p].first == v.first )
                return false;
            while ( valuesTable_[p] != size_t(-1) ){
                if ( values_[valuesTable_[p]].first == v.first ){
                    return false;
                }
                p = valuesTable_[p];
            }
            valuesTable_[p] = size_;
        }
        values_[size_++] = std::move(v);
        return true;
    }
    std::array<std::size_t, TableSize> hashTable_;
    std::array<std::size_t, Capacity> valuesTable_;
    std::array<storageType, Capacity> values_;
    Hash hash_;
    std::size_t size_;
};
} // namespace staticMap
