#pragma once

#include <memory>
#include <iostream>

namespace bst {
template <class T>
class Tree {
 public:
  Tree();
  Tree(std::initializer_list<T> list);
  template <template <class...> class Container>
  explicit Tree(const Container<T>& vals);
  Tree(const Tree& other);
  Tree(Tree&&) noexcept = default;
  ~Tree() = default;

  auto operator=(const Tree& other) -> Tree&;
  auto operator=(Tree&&) noexcept -> Tree& = default;
  auto operator=(std::initializer_list<T> list) -> Tree&;
  auto operator==(const Tree& other) const -> bool;
  auto operator!=(const Tree& other) const -> bool;

  auto size() const -> size_t;
  auto empty() const -> bool;
  auto exists(const T& val) const -> bool;
  template <class U>
  auto insert(U&& val) -> void;
  auto insert(std::initializer_list<T> list) -> void;
  template <template <class...> class Container>
  auto insert(const Container<T>& vals) -> void;
  auto remove(const T& val) -> void;
  auto clear() -> void;
  auto print(std::ostream& os = std::cout) const -> void;

 private:
  class Impl;
  std::unique_ptr<Impl> impl;
};
}

#include "bst_impl.hpp"
