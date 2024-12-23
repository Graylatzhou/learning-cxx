#include "../exercise.h"
#include <memory>

// READ: `std::shared_ptr` <https://zh.cppreference.com/w/cpp/memory/shared_ptr>
// READ: `std::weak_ptr` <https://zh.cppreference.com/w/cpp/memory/weak_ptr>
//unique_ptr 独占指针(一个指针独占一个对象),shared_ptr(多个指针指向同一对象，当最后一个指针也被销毁的时候，对象被释放) 共享指针,weak_ptr 弱指针
// TODO: 将下列 `?` 替换为正确的值
int main(int argc, char **argv) {
    auto shared = std::make_shared<int>(10);//创建一个shared_ptr对象,指向一个int类型的对象，值为10
    std::shared_ptr<int> ptrs[]{shared, shared, shared};

    std::weak_ptr<int> observer = shared;//弱指针不参与引用计数
    ASSERT(observer.use_count() == 4, "");

    ptrs[0].reset();
    ASSERT(observer.use_count() == 3, "");

    ptrs[1] = nullptr;
    ASSERT(observer.use_count() == 2, "");

    ptrs[2] = std::make_shared<int>(*shared);//这里是指向一个新对象，只是值为*shared
    ASSERT(observer.use_count() == 1, "");

    ptrs[0] = shared;
    ptrs[1] = shared;
    //下面是用了std::move函数，将shared的值转移到ptrs[2]中，这样shared就不再指向任何对象了
    ptrs[2] = std::move(shared);
    ASSERT(observer.use_count() == 3, "");

    std::ignore = std::move(ptrs[0]);
    ptrs[1] = std::move(ptrs[1]);
    ptrs[1] = std::move(ptrs[2]);
    ASSERT(observer.use_count() == 2, "");

    shared = observer.lock();//将弱指针转换为shared_ptr
    ASSERT(observer.use_count() == 3, "");

    shared = nullptr;//->释放ptr,这个ptr来自弱指针
    //以下为释放ptrs中的指针
    for (auto &ptr : ptrs) ptr = nullptr;
    ASSERT(observer.use_count() == 0, "");

    shared = observer.lock();
    ASSERT(observer.use_count() == 0, "");

    return 0;
}
