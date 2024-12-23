﻿#include "../exercise.h"

// READ: 类模板 <https://zh.cppreference.com/w/cpp/language/class_template>

template<class T>
struct Tensor4D {
    unsigned int shape[4];
    T *data;

    Tensor4D(unsigned int const shape_[4], T const *data_) {
        unsigned int size = 1;
        // TODO: 填入正确的 shape 并计算 size
        for (unsigned int i = 0; i < 4; ++i) {
            shape[i] = shape_[i];
            size *= shape[i];
        }
        data = new T[size];
        std::memcpy(data, data_, size * sizeof(T));
    }
    ~Tensor4D() {
        delete[] data;
    }

    // 为了保持简单，禁止复制和移动
    Tensor4D(Tensor4D const &) = delete;
    Tensor4D(Tensor4D &&) noexcept = delete;

    // 这个加法需要支持“单向广播”。
    // 具体来说，`others` 可以具有与 `this` 不同的形状，形状不同的维度长度必须为 1。
    // `others` 长度为 1 但 `this` 长度不为 1 的维度将发生广播计算。
    // 例如，`this` 形状为 `[1, 2, 3, 4]`，`others` 形状为 `[1, 2, 1, 4]`，
    // 则 `this` 与 `others` 相加时，3 个形状为 `[1, 2, 1, 4]` 的子张量各自与 `others` 对应项相加。

    /*
    pointer -> move
    1 2 3 4 5 6 7 8 9 10
    如果需要广播则重置指针
    */
    Tensor4D &operator+=(Tensor4D const &others) {
        // 检查形状是否满足广播条件
        for (auto i = 0u; i < 4; ++i) {
            if (shape[i] != others.shape[i] && others.shape[i] != 1) {
                throw std::invalid_argument("Shapes are not compatible for broadcasting");
            }
        }

        // 预先存储每个维度是否需要广播
        bool broadcast[4];
        for (auto i = 0u; i < 4; ++i) {
            broadcast[i] = (others.shape[i] == 1);
        }

        // 计算总元素数
        unsigned int size = 1;
        for (auto i = 0u; i < 4; ++i) {
            size *= shape[i];
        }

        // 遍历所有元素并进行相加
        for (unsigned int idx = 0; idx < size; ++idx) {
            // 计算在 each dimension 的索引
            unsigned int idx_temp = idx;
            unsigned int idx_this[4];
            unsigned int idx_others[4];
            for (int i = 3; i >= 0; --i) {
                idx_this[i] = idx_temp % shape[i];
                idx_temp /= shape[i];
                idx_others[i] = broadcast[i] ? 0 : idx_this[i];
            }

            // 计算偏移量
            unsigned int offset_this =
                idx_this[0] * shape[1] * shape[2] * shape[3] +
                idx_this[1] * shape[2] * shape[3] +
                idx_this[2] * shape[3] +
                idx_this[3];

            unsigned int offset_others =
                idx_others[0] * others.shape[1] * others.shape[2] * others.shape[3] +
                idx_others[1] * others.shape[2] * others.shape[3] +
                idx_others[2] * others.shape[3] +
                idx_others[3];

            // 执行相加操作
            this->data[offset_this] += others.data[offset_others];
        }

        return *this;
    }
};

// ---- 不要修改以下代码 ----
int main(int argc, char **argv) {
    {
        unsigned int shape[]{1, 2, 3, 4};
        // clang-format off
        int data[]{
             1,  2,  3,  4,
             5,  6,  7,  8,
             9, 10, 11, 12,

            13, 14, 15, 16,
            17, 18, 19, 20,
            21, 22, 23, 24};
        // clang-format on
        auto t0 = Tensor4D(shape, data);
        auto t1 = Tensor4D(shape, data);
        t0 += t1;
        for (auto i = 0u; i < sizeof(data) / sizeof(*data); ++i) {
            ASSERT(t0.data[i] == data[i] * 2, "Tensor doubled by plus its self.");
        }
    }
    {
        unsigned int s0[]{1, 2, 3, 4};
        // clang-format off
        float d0[]{
            1, 1, 1, 1,
            2, 2, 2, 2,
            3, 3, 3, 3,

            4, 4, 4, 4,
            5, 5, 5, 5,
            6, 6, 6, 6};
        // clang-format on
        unsigned int s1[]{1, 2, 3, 1};
        // clang-format off
        float d1[]{
            6,
            5,
            4,

            3,
            2,
            1};
        // clang-format on

        auto t0 = Tensor4D(s0, d0);
        auto t1 = Tensor4D(s1, d1);
        t0 += t1;
        for (auto i = 0u; i < sizeof(d0) / sizeof(*d0); ++i) {
            ASSERT(t0.data[i] == 7.f, "Every element of t0 should be 7 after adding t1 to it.");
        }
    }
    {
        unsigned int s0[]{1, 2, 3, 4};
        // clang-format off
        double d0[]{
             1,  2,  3,  4,
             5,  6,  7,  8,
             9, 10, 11, 12,

            13, 14, 15, 16,
            17, 18, 19, 20,
            21, 22, 23, 24};
        // clang-format on
        unsigned int s1[]{1, 1, 1, 1};
        double d1[]{1};

        auto t0 = Tensor4D(s0, d0);
        auto t1 = Tensor4D(s1, d1);
        t0 += t1;
        for (auto i = 0u; i < sizeof(d0) / sizeof(*d0); ++i) {
            ASSERT(t0.data[i] == d0[i] + 1, "Every element of t0 should be incremented by 1 after adding t1 to it.");
        }
    }
}