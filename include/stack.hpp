#pragma once

#include <array>
#include <cstdint>
#include <iostream>

namespace c8 {

template <typename T, auto N> class Stack {
    public:
    Stack() = delete;
    explicit Stack(uint8_t &SP_reg) : SP{&SP_reg} {};

    void push(const T ins) {
        if (*(SP) == N) {
            std::cerr << "ERROR: Cannot push element to full stack!\n";
            return;
        }
        stack.at(*SP) = ins;
        ++(*SP);
    }

    void pop() {
        if (*SP == 0) {
            std::cerr << "ERROR: Cannot pop element from empty stack!\n";
            return;
        }
        --(*SP);
    }

    inline T top() { return stack.at(*SP - 1); }

    private:
    uint8_t * const SP{};
    std::array<T, N> stack{{}};
};
} // namespace c8
