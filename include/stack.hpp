#pragma once

#include <array>
#include <cstdint>
#include <iostream>

namespace c8 {

template <typename T, auto N> class Stack {
public:
  Stack() = default;
  Stack(Stack &&) = default;
  Stack(const Stack &) = default;
  Stack &operator=(Stack &&) = default;
  Stack &operator=(const Stack &) = default;
  ~Stack() = default;

  Stack(uint8_t &SP_reg) : SP{&SP_reg}{};
  void push(const T ins) {
    if (*(this->SP) == N) {
      std::cerr << "ERROR: Cannot push element to full stack!\n";
      return;
    }
    stack.at(*(this->SP)) = ins;
    ++(*(this->SP));
  }

  void pop() {
    if (*(this->SP) == 0) {
      std::cerr << "ERROR: Cannot pop element from empty stack!\n";
      return;
    }
    --(*(this->SP));
  }

  inline T top(){
    return stack.at(*(this->SP) - 1);
  }

private:
  uint8_t *const SP{};
  std::array<T, N> stack{{}};
};
} // namespace c8
