#include <static_map.h>

#include <iostream>

int main() {
    int result = 0;
    {

        staticMap::StaticMap<int, int, 4> map;

        if (map.size() != 0) {
            std::cout << "map not empty\n";
            result = 1;
        }
        if (map.capacity() != 4) {
            std::cout << "bad capacity: " << map.capacity() << "\n";
            result = 1;
        }

        map.insert({13, 7});

        if (map.size() != 1) {
            std::cout << "operator [] not insert element\n";
            result = 1;
        }
        if (map.capacity() != 4) {
            std::cout << "bad capacity: " << map.capacity() << "\n";
            result = 1;
        }

        for (int i = -1024; i < 1024; i++) {
            auto p = map.find(i);
            if ((i != 13) && (p != nullptr)) {
                std::cout << "Exist not inserted object in map: " << i << "\n";
                result = 1;
            } else if ((i == 13) && (p == nullptr)) {
                std::cout << "Not exist inserted object in map\n";
                result = 1;
            } else {
                ;
            }
        }

        map[13]++;

        if (map[13] != 8) {
            std::cout << "operator ++ write bad value: " << map[13] << "\n";
            result = 1;
        }

        map[14] = 7;

        if (map[14] != 7) {
            std::cout << "operator [] insert bad value: " << map[13] << "\n";
            result = 1;
        }
    }
    {
        // мы объявили ёмкость 4, при этом задали 5 элементов. при этом
        // последний элемент должен быть не добавлен
        staticMap::StaticMap<int, int, 4, 2> map(
            {{0, 0}, {1, 1}, {2, 2}, {3, 3}, {5, 5}});
        if (map.size() != 4) {
            std::cout << "size greater then capacity\n";
            result = 1;
        }
        for (int i = 0; i < 4; i++) {
            auto ind = map.find(i);
            if (ind == nullptr) {
                std::cout << "Element " << i << " not found\n";
                result = 1;
            } else if (ind->second != i) {
                std::cout << "Element " << i << " bad data: " << ind->second
                          << "\n";
                result = 1;
            }
        }
    }

    return result;
}
