#include <functional>
#include <iostream>
#include <ostream>

int sum(int x, int y, int z) {
    return x + y + z;
}

class BindClass
{
public:
    int cls_sum(int x, int y, int z) {
        return x + y + z;
    }

    int cls_sum(int x, int y) {
        return x + y;
    }
};

int main() {
    std::cout << "bind normal func\n";
    auto bind_sum = std::bind(sum, 1, std::placeholders::_1, std::placeholders::_2);
    auto bind_front_sum = std::bind_front(sum, 1);
    auto bind_end_sum = std::__bind_back(sum, 3);  // fuck clang
    std::cout << bind_sum(2, 3) << bind_front_sum(2, 3) << bind_end_sum(1, 2) << std::endl;

    std::cout << "bind class func\n";
    auto bind_cls = BindClass();
    // auto bind_cls_sum_3 = std::bind(&BindClass::cls_sum, &bind_cls, 1, std::placeholders::_1, std::placeholders::_2); //Error, when have repeat name func
    // auto bind_cls_sum_2 = std::bind(&BindClass::cls_sum, &bind_cls, 1, std::placeholders::_1);  //Error, when have repeat name func
    // std::cout << bind_cls_sum_3(2, 3) << bind_cls_sum_2(2) << std::endl;

    // use std::bind_front
    // auto bind_cls_front_3 = std::bind_front(&BindClass::cls_sum, &bind_cls); // Error either!
}