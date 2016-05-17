#pragma once

namespace bst {
template <class T>
struct Tree<T>::Impl {
  class Node;
  using NodePtr = std::shared_ptr<Node>;
  using NodePtrW = std::weak_ptr<Node>;

  NodePtr root;
  size_t size;

  Impl();
  ~Impl() = default;

  auto equals(const NodePtr& lhs, const NodePtr& rhs) const -> bool;
  auto find(const T& val, const NodePtr& node) const -> NodePtr;
  auto findMax(const NodePtr& node) const -> NodePtr;
  auto getChild(const NodePtr& node, const T& val) const -> NodePtr&;
  auto getInsertPos(const T& val, const NodePtr& node) const -> NodePtr;
  template <class U>
  auto insert(U&& val, NodePtr& node, const NodePtr& parent = nullptr) -> void;
  auto replace(NodePtr& node) const -> void;
  auto shiftUp(const NodePtr& node) const -> void;
  auto print(const NodePtr& node, int depth, std::ostream& os) const -> void;
};

template <class T>
struct Tree<T>::Impl::Node {
  T val;
  NodePtr left;
  NodePtr right;
  NodePtrW parent;

  template <class U>
  explicit Node(U&& val);
  template <class U, class NodePtrT>
  Node(U&& val, const NodePtrT& parent);
  ~Node() = default;

  template <class NodePtrT = NodePtr>
  auto clone() const -> NodePtrT;
};

template <class T>
inline Tree<T>::Tree() : impl(new Impl()) {}

template <class T>
Tree<T>::Tree(std::initializer_list<T> list) : impl(new Impl()) {
  insert(list);
}

template <class T>
template <template <class...> class Container>
inline Tree<T>::Tree(const Container<T>& vals) : impl(new Impl()) {
  insert(vals);
}

template <class T>
inline Tree<T>::Tree(const Tree& other) : impl(new Impl()) {
  *this = other;
}

template <class T>
auto Tree<T>::operator=(const Tree& other) -> Tree& {
  if(other.impl->root) impl->root = other.impl->root->clone();
  impl->size = other.impl->size;
  return *this;
}

template <class T>
auto Tree<T>::operator=(std::initializer_list<T> list) -> Tree& {
  clear();
  insert(list);
  return *this;
}

template <class T>
inline auto Tree<T>::operator==(const Tree& other) const -> bool {
  if(size() != other.size()) return false;
  return impl->equals(impl->root, other.impl->root);
}

template <class T>
inline auto Tree<T>::operator!=(const Tree& other) const -> bool {
  return !(*this == other);
}

template <class T>
inline auto Tree<T>::size() const -> size_t {
  return impl->size;
}

template <class T>
inline auto Tree<T>::empty() const -> bool {
  return !impl->size;
}

template <class T>
inline auto Tree<T>::exists(const T& val) const -> bool {
  return static_cast<bool>(impl->find(val, impl->root));
}

template <class T>
template <class U>
auto Tree<T>::insert(U&& val) -> void {
  if(!impl->root) impl->insert(std::forward<U>(val), impl->root);
  else if(auto node = impl->getInsertPos(val, impl->root)) {
    impl->insert(std::forward<U>(val), impl->getChild(node, val), node);
  }
}

template <class T>
auto Tree<T>::insert(std::initializer_list<T> list) -> void {
  for(const auto& val : list) {
    insert(val);
  }
}

template <class T>
template <template <class...> class Container>
auto Tree<T>::insert(const Container<T>& vals) -> void {
  for(const auto& val : vals) {
    insert(val);
  }
}

template <class T>
auto Tree<T>::remove(const T& val) -> void {
  auto node = impl->find(val, impl->root);
  if(!node) return;
  if(node->left && node->right) impl->replace(node);
  impl->shiftUp(node);
  impl->size--;
}

template <class T>
inline auto Tree<T>::clear() -> void {
  impl.reset(new Impl());
}

template <class T>
inline auto Tree<T>::print(std::ostream& os) const -> void {
  if(impl->root) impl->print(impl->root, 0, os);
}

template <class T>
inline Tree<T>::Impl::Impl() : root(), size() {}

template <class T>
auto Tree<T>::Impl::equals(const NodePtr& lhs,
                           const NodePtr& rhs) const -> bool {
  if(lhs->val != rhs->val) return false;
  if(static_cast<bool>(lhs->left) != static_cast<bool>(rhs->left)) return false;
  if(lhs->left && !equals(lhs->left, rhs->left)) return false;
  if(lhs->right && !equals(lhs->right, rhs->right)) return false;
  return true;
}

template <class T>
auto Tree<T>::Impl::find(const T& val, const NodePtr& node) const -> NodePtr {
  if(!node || node->val == val) return node;
  return find(val, node->val > val ? node->left : node->right);
}

template <class T>
inline auto Tree<T>::Impl::findMax(const NodePtr& node) const -> NodePtr {
  return node && node->right ? findMax(node->right) : node;
}

template <class T>
inline auto Tree<T>::Impl::getChild(const NodePtr& node,
                                    const T& val) const -> NodePtr& {
  return node->val > val ? node->left : node->right;
}

template <class T>
auto Tree<T>::Impl::getInsertPos(const T& val,
                                 const NodePtr& node) const -> NodePtr {
  if(node->val == val) return NodePtr();
  auto& next = getChild(node, val);
  return next ? getInsertPos(val, next) : node;
}

template <class T>
template <class U>
auto Tree<T>::Impl::insert(U&& val, NodePtr& node,
                           const NodePtr& parent) -> void {
  node.reset(new Node(std::forward<U>(val), parent));
  size++;
}

template <class T>
auto Tree<T>::Impl::replace(NodePtr& node) const -> void {
  auto max_node = findMax(node->left);
  node->val = max_node->val;
  node = max_node;
}

template <class T>
auto Tree<T>::Impl::shiftUp(const NodePtr& node) const -> void {
  auto child = node->left ? node->left : node->right;
  if(child) child->parent = node->parent;
  if(auto parent = node->parent.lock()) {
    parent->val > node->val ? parent->left : parent->right = child;
  }
}

template <class T>
auto Tree<T>::Impl::print(const NodePtr& node, int depth,
                          std::ostream& os) const -> void {
  if(node->right) print(node->right, depth + 1, os);
  os << std::string(3 * depth, ' ') << node->val << std::endl;
  if(node->left) print(node->left, depth + 1, os);
}

template <class T>
template <class U>
Tree<T>::Impl::Node::Node(U&& val)
  : val(std::forward<U>(val)), left(), right(), parent() {}

template <class T>
template <class U, class NodePtrT>
Tree<T>::Impl::Node::Node(U&& val, const NodePtrT& parent)
  : val(std::forward<U>(val)), left(), right(), parent(parent) {}

template <class T>
template <class NodePtrT>
auto Tree<T>::Impl::Node::clone() const -> NodePtrT {
  NodePtrT node(new Node(val));
  if(left) {
    node->left = left->clone();
    node->left->parent = node;
  }
  if(right) {
    node->right = right->clone();
    node->right->parent = node;
  }
  return std::move(node);
}
}
