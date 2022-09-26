#pragma once

#include <bit>
#include <cstdint>
#include <functional>
#include <initializer_list>
#include <utility>

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
    struct listElem {
        listElem *next;
        storageType *v;
        listElem() : next(nullptr), v(nullptr) {}
    };

    ValueType &operator[](const KeyType &k) {
        auto ind = find(k);
        if (ind == nullptr) {
            ind = insert({k, ValueType()});
        }
        return ind->second;
    }

    storageType *insert(storageType v) {
        if (size_ >= Capacity) {
            return nullptr;
        }
        std::size_t ind = size_++;
        values_[ind] = std::move(v);
        valuesTable_[ind].v = &values_[ind];
        std::size_t hash = hash_(values_[ind].first) % TableSize;
        listElem **p = &hashTable_[hash];
        while (*p) {
            p = &((*p)->next);
        }
        *p = &valuesTable_[ind];
        return (*p)->v;
    }

    constexpr StaticMap(std::initializer_list<storageType> e = {},
                        Hash hash = Hash())
        : hash_(hash), size_(0) {
        for (auto &i : hashTable_) {
            i = nullptr;
        }
        for (auto &i : e) {
            insert(std::move(i));
        }
    }

    std::size_t size() const { return size_; }
    std::size_t capacity() const { return Capacity; }

    storageType *find(const KeyType &k) {
        std::size_t hash = hash_(k) % TableSize;
        listElem *p = hashTable_[hash];
        while ((p != nullptr) && (p->v->first != k)) {
            p = p->next;
        }
        return p == nullptr ? nullptr : p->v;
    }

    const storageType *find(const KeyType &k) const {
        std::size_t hash = hash_(k) % TableSize;
        listElem *p = hashTable_[hash];
        while ((p != nullptr) && (p->v->first != k)) {
            p = p->next;
        }
        return p->v;
    }

  private:
    listElem *hashTable_[TableSize];
    listElem valuesTable_[Capacity];
    storageType values_[Capacity];
    Hash hash_;
    std::size_t size_;
};
} // namespace staticMap
