#include "../exercise.h"

// READ: 有 cv 限定符的成员函数 <https://zh.cppreference.com/w/cpp/language/member_functions>
/*
 Fibonacci 对象被声明为 constexpr，
 这意味着它是一个常量对象。而在常量对象上调用成员函数时，
 该成员函数必须是 const 成员函数，否则编译器会报错
*/
struct Fibonacci {
    int numbers[11];
    // TODO: 修改方法签名和实现，使测试通过
   int get(int i) const{
        if (i <= 10) {
            return numbers[i];
        }
        return -1;
    }
};

int main(int argc, char **argv) {
    Fibonacci constexpr FIB{{0, 1, 1, 2, 3, 5, 8, 13, 21, 34, 55}};
    ASSERT(FIB.get(10) == 55, "fibonacci(10) should be 55");
    std::cout << "fibonacci(10) = " << FIB.get(10) << std::endl;
    return 0;
}
