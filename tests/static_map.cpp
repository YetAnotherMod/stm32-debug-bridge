#include <static_map.h>
#include <iostream>

 class hasher {
    public:
        unsigned operator () (unsigned x) const {return x;}
    };

int function (staticMap::StaticMap<int, int, 4, 2, hasher> map)
{
    if (map.size() != 4) {
            std::cout << "Incorrect size for";
            for (const auto &i:map){
                std::cout << " " << i.first << ":" << i.second;
            }
            std::cout<<std::endl;
            return 1; 
        }

        int a = 0;
        int b = 0;
        for (int i = 0; i < 4; i++) {
            auto x1 = map.find(a);
            if (x1 == nullptr) {
                std::cout << "Element not found\n";
                return 1;
            } else if (x1->second != b) {
                std::cout << "Incorrect element found\n";
                return 1;
            }
            a = a+2;
            b++;
        }
    return 0;
}
        
int main() {
    int result = 0;
    {
        staticMap::StaticMap<int, int, 4> map;

        if (map.size() != 0) {
            std::cout << "Map not empty\n";
            result = 1;
        }
        if (map.capacity() != 4) {
            std::cout << "Bad capacity: " << map.capacity() << "\n";
            result = 1;
        }
        map[13]= 1;
        map[13]= 7;
        if (map.size() != 1) {
            std::cout << "Operator [] not insert element\n";
            result = 1;
        }
        map.insert({13,7});
        if (map.size() != 1) {
            std::cout << "Insert dublicate element changed size\n";
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

        if (map[13] != 7) {
            std::cout << "Insert dublicate element changed data\n";
            result = 1;
        }

        if (map.capacity() != 4) {
            std::cout << "Bad capacity: " << map.capacity() << "\n";
            result = 1;
        }

        map[1] = 10;
        map[5] = 12;
        auto p = map.insert({1,10});
        if (p != nullptr) {
            std::cout << "Insert dublicate element changed data\n";
            result = 1;
        }
        if (map.size() != 3) {
            std::cout << "Insert dublicate element changed size\n";
            result = 1;
        }
        if ((map[1] != 10) || (map[5] != 12) || (map[13] != 7)) {
            std::cout << "Insert dublicate element changed data\n";
            result = 1;
        }

        auto pp = map.insert({3,11});
        if (pp->first != 3 || pp->second != 11) {
            std::cout << "Insert element bad data\n";
            result = 1;
        }

        map[13]++;
        if (map[13] != 8) {
            std::cout << "Operator ++ write bad value: " << map[13] << "\n";
            result = 1;
        }

        try {
            map[14] = 7;
            std::cout << "Element added\n";
            result = 1;
        } catch (const std::out_of_range &) {;}
    } 
    {
        try{
            staticMap::StaticMap<int, int, 4, 2> map(
                {{0, 0}, {1, 1}, {2, 2}, {3, 3}, {5, 5}});
            result = 1;
            std::cout << "exception not generated on constructor overflow" << std::endl;
        }
        catch (std::bad_exception &e){
        }
    }
    {
        // мы объявили ёмкость 4, при этом задали 5 элементов. при этом
        // последний элемент должен быть не добавлен
        staticMap::StaticMap<int, int, 4, 2> map(
            {{0, 0}, {1, 1}, {2, 2}, {3, 3}});
        if (map.size() != 4) {
            std::cout << "Size greater then capacity\n";
            std::cout << map.size() << " " << map.capacity();
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
        auto ind = map.find(4);
        if (ind != nullptr) {
            std::cout << "Element 5 found\n";
            result = 1;
        }

        int a = 0;
        for (const auto &i:map) {
            if (i.first != a || i.second != a)
                result = 1;
            std::cout << " " << i.first << ":" << i.second;
            a++;
        }
    }
    {
        const staticMap::StaticMap<int, int, 4, 2> map(
            {{0, 0}, {1, 1}, {2, 2}, {3, 3}});
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
        auto ind = map.find(4);
        if (ind != nullptr) {
            std::cout << "Element 5 found\n";
            result = 1;
        }
    }
    {
        using namespace std;
        class DT{
            private:
            int x;
            bool i;
            public:
            DT():x(0),i(false){}
            DT(int v):x(v),i(true){}
            DT(const DT & v):x(v.x),i(v.i){}
            DT(DT && v){x=v.x;i=v.i;v.i=false;}
            DT & operator = (int v){x=v;i=true; return *this;}
            DT & operator = (const DT & v){x=v.x;i=v.i; return *this;}
            DT & operator = (DT && v){x=v.x;i=v.i;v.i=false; return *this;}
            bool operator == (const DT & v)const{if(!i || !v.i) return false; return x==v.x;}
            bool operator != (const DT & v)const{return !(*this == v);}
            bool empty() const {return !i;}
            unsigned hash()const {if (i) return x; else return 0;}
            ~DT (){i=false;}
        };
        class hasher_{
            public:
            unsigned operator () (const DT &v) const {return v.hash();}
        };
        staticMap::StaticMap<DT, DT, 4, 4, hasher_> map;
        std::pair<DT,DT> v {4,4};
        map.insert(move(v));
        if (map.size() != 1) {
            cout << "Element not in map\n";
            result = 1;
        }
        auto x = map.find(4);
        if (x == nullptr) {
            cout << "Added value not found\n";
            result = 1;
        } else if (x->second != 4) {
            cout << "Incorrect data\n";
            result = 1;
        }
        if (!v.first.empty() || !v.second.empty()) {
            cout << "Data from v don't move\n";
            result = 1;
        } 
        v = {4,5};
        map.insert(move(v));

        if (map.size() != 1) {
            cout << "Element added in map\n";
            result = 1;
        }

        auto y = map.find(4);
        if (y == nullptr) {
            cout << "Added value not found\n";
            result = 1;
        } else if (y->second != 4) {
            cout << "Value changed\n";
            result = 1;
        }
        if (!v.first.empty() || !v.second.empty()) {
            cout << "Data from v don't move\n";
            result = 1;
        }  
    }
    {
        if (function(staticMap::StaticMap<int, int, 4, 2, hasher> ({{0, 0}, {2, 1}, {4, 2}, {6, 3}})) != 0) {result = 1;}
        if (function(staticMap::StaticMap<int, int, 4, 2, hasher> ({{0, 0}, {2, 1}, {6, 3}, {4, 2}})) != 0) {result = 1;}
        if (function(staticMap::StaticMap<int, int, 4, 2, hasher> ({{0, 0}, {4, 2}, {2, 1}, {6, 3}})) != 0) {result = 1;}
        if (function(staticMap::StaticMap<int, int, 4, 2, hasher> ({{0, 0}, {4, 2}, {6, 3}, {2, 1}})) != 0) {result = 1;}
        if (function(staticMap::StaticMap<int, int, 4, 2, hasher> ({{0, 0}, {6, 3}, {2, 1}, {4, 2}})) != 0) {result = 1;}
        if (function(staticMap::StaticMap<int, int, 4, 2, hasher> ({{0, 0}, {6, 3}, {4, 2}, {2, 1}})) != 0) {result = 1;}
        if (function(staticMap::StaticMap<int, int, 4, 2, hasher> ({{2, 1}, {0, 0}, {4, 2}, {6, 3}})) != 0) {result = 1;}
        if (function(staticMap::StaticMap<int, int, 4, 2, hasher> ({{2, 1}, {0, 0}, {6, 3}, {4, 2}})) != 0) {result = 1;}
        if (function(staticMap::StaticMap<int, int, 4, 2, hasher> ({{2, 1}, {4, 2}, {0, 0}, {6, 3}})) != 0) {result = 1;}
        if (function(staticMap::StaticMap<int, int, 4, 2, hasher> ({{2, 1}, {4, 2}, {6, 3}, {0, 0}})) != 0) {result = 1;}
        if (function(staticMap::StaticMap<int, int, 4, 2, hasher> ({{2, 1}, {6, 3}, {0, 0}, {4, 2}})) != 0) {result = 1;}
        if (function(staticMap::StaticMap<int, int, 4, 2, hasher> ({{2, 1}, {6, 3}, {4, 2}, {0, 0}})) != 0) {result = 1;}
        if (function(staticMap::StaticMap<int, int, 4, 2, hasher> ({{4, 2}, {0, 0}, {2, 1}, {6, 3}})) != 0) {result = 1;}
        if (function(staticMap::StaticMap<int, int, 4, 2, hasher> ({{4, 2}, {0, 0}, {6, 3}, {2, 1}})) != 0) {result = 1;}
        if (function(staticMap::StaticMap<int, int, 4, 2, hasher> ({{4, 2}, {2, 1}, {0, 0}, {6, 3}})) != 0) {result = 1;}
        if (function(staticMap::StaticMap<int, int, 4, 2, hasher> ({{4, 2}, {2, 1}, {6, 3}, {0, 0}})) != 0) {result = 1;}
        if (function(staticMap::StaticMap<int, int, 4, 2, hasher> ({{4, 2}, {6, 3}, {0, 0}, {2, 1}})) != 0) {result = 1;}
        if (function(staticMap::StaticMap<int, int, 4, 2, hasher> ({{4, 2}, {6, 3}, {2, 1}, {0, 0}})) != 0) {result = 1;}
        if (function(staticMap::StaticMap<int, int, 4, 2, hasher> ({{6, 3}, {0, 0}, {2, 1}, {4, 2}})) != 0) {result = 1;}
        if (function(staticMap::StaticMap<int, int, 4, 2, hasher> ({{6, 3}, {0, 0}, {4, 2}, {2, 1}})) != 0) {result = 1;}
        if (function(staticMap::StaticMap<int, int, 4, 2, hasher> ({{6, 3}, {2, 1}, {0, 0}, {4, 2}})) != 0) {result = 1;}
        if (function(staticMap::StaticMap<int, int, 4, 2, hasher> ({{6, 3}, {2, 1}, {4, 2}, {0, 0}})) != 0) {result = 1;}
        if (function(staticMap::StaticMap<int, int, 4, 2, hasher> ({{6, 3}, {4, 2}, {0, 0}, {2, 1}})) != 0) {result = 1;}
        if (function(staticMap::StaticMap<int, int, 4, 2, hasher> ({{6, 3}, {4, 2}, {2, 1}, {0, 0}})) != 0) {result = 1;}
    }
    {
        staticMap::StaticMap<int, int, 4, 2, hasher> map(
        {{1, 0}, {3, 1}, {5, 2}, {7, 3}});
        
        if (map.size() != 4) {
            std::cout << "Incorrect size\n";
            result = 1; 
        }

        int a = 0;
        int b = 0;
        for (int i = 0; i < 4; i++) {
            auto x1 = map.find(a);
            if (x1 != nullptr) {
                std::cout << "Element found\n";
                result = 1;
            }
            a = a+2;
            b++;
        }
    }
    return result;
}
