#ifndef __CLRS4_ORDER_RANK_TREE_H__
#define __CLRS4_ORDER_RANK_TREE_H__

#include <cassert>
#include <common.h>
#include <rbtree.h>

namespace frozenca {

using namespace std;

template <Containable T>
struct RBRankNode : public hard::detail::RBTreeNodeBase<T, RBRankNode<T>> {
  using Base = hard::detail::RBTreeNodeBase<T, RBRankNode<T>>;
  ptrdiff_t rank_ = 0;

  Base &base() { return static_cast<Base &>(*this); }

  const Base &base() const { return static_cast<const Base &>(*this); }

  RBRankNode *left() { return base().left_.get(); }

  const RBRankNode *left() const { return base().left_.get(); }

  RBRankNode *right() { return base().right_.get(); }

  const RBRankNode *right() const { return base().right_.get(); }
};

template <Containable K>
class RankTree
    : public hard::detail::RedBlackTreeBase<K, K, less<K>, false, RBRankNode<K>,
                                            RankTree<K>> {
public:
  using Base = hard::detail::RedBlackTreeBase<K, K, less<K>, false,
                                              RBRankNode<K>, RankTree<K>>;
  friend class Base;
  using Comp = less<K>;
  using Node = RBRankNode<K>;
  using key_type = Base::key_type;
  using reference_type = Base::reference_type;
  using const_reference_type = Base::const_reference_type;

  Base &base() { return static_cast<Base &>(*this); }

  const Base &base() const { return static_cast<const Base &>(*this); }

private:
  static int verify(const Node *node) {
    if (!node) {
      return 0;
    }

    auto left_black_height = verify(node->left_.get());
    auto right_black_height = verify(node->right_.get());

    // binary search tree properties
    assert(!node->left_ ||
           (node->left_->parent_ == node &&
            !Comp{}(this->proj(node->key_), this->proj(node->left_->key_))));
    assert(!node->right_ ||
           (node->right_->parent_ == node &&
            !Comp{}(this->proj(node->right_->key_), this->proj(node->key_))));

    // if a node is red, then both its children are black.
    assert(node->black_ || ((!node->left_ || node->left_->black_) &&
                            (!node->right_ || node->right_->black_)));

    // for each node, all simple paths from the node to
    // descendant leaves contain the same number of black nodes.
    assert(left_black_height == right_black_height);

    // to mute unused variable right_black_height compiler warning for release
    // builds..
    return ((left_black_height + right_black_height) / 2) +
           ((node->black_) ? 1 : 0);
  }

  void verify() const {
    // the root is black.
    assert(!this->get_root() || this->get_root()->black_);
    assert(base().bh_ == verify(this->get_root()));
  }

protected:
  const Node *os_select(const Node *node, ptrdiff_t i) const {
    assert(node);
    auto r = node->rank_;
    if (i == r) {
      return node;
    } else if (i < r) {
      return os_select(node->left_.get(), i);
    } else {
      return os_select(node->right_.get(), i - r - 1);
    }
  }

  const Node *os_select_iterative(ptrdiff_t i) const {
    auto node = this->get_root();
    while (true) {
      auto r = node->rank_;
      if (i == r) {
        return node;
      } else if (i < r) {
        node = node->left();
      } else {
        node = node->right();
        i -= (r + 1);
      }
    }
  }

  ptrdiff_t os_rank(const Node *node) const {
    assert(node);
    auto r = node->rank_;
    auto y = node;
    while (y != this->get_root()) {
      if (y == y->parent_->right()) {
        r += y->parent_->rank_ + 1;
      }
      y = y->parent_;
    }
    return r;
  }

private:
  void left_rotate_post(Node *y, Node *x) {
    assert(x && y);
    y->rank_ += (x->rank_ + 1);
  }

  void right_rotate_post(Node *y, Node *x) {
    assert(x && y);
    x->rank_ -= (y->rank_ + 1);
  }

  void insert_fixup_pre(Node *z) {
    auto curr = z;
    while (curr) {
      if (curr->parent_ && curr == curr->parent_->left_.get()) {
        curr->parent_->rank_++;
      }
      curr = curr->parent_;
    }
  }

  void erase_fixup_pre(Node *z, Node *zp) {
    auto curr = z;
    auto cp = zp;
    while (cp) {
      if (cp && curr == cp->left_.get()) {
        cp->rank_--;
      }
      curr = cp;
      cp = cp->parent_;
    }
  }

public:
  reference_type operator[](ptrdiff_t i) {
    return const_cast<Node *>(os_select(this->get_root(), i))->key_;
  }

  const_reference_type operator[](ptrdiff_t i) const {
    return os_select(this->get_root(), i)->key_;
  }

  reference_type at(ptrdiff_t i) {
    return const_cast<Node *>(os_select_iterative(i))->key_;
  }

  const_reference_type at(ptrdiff_t i) const {
    return os_select_iterative(i)->key_;
  }

  [[nodiscard]] ptrdiff_t index(const K &key) const {
    auto node = this->find_node(key);
    if (!node) {
      return -1;
    } else {
      return os_rank(node);
    }
  }

  const K &succ(const K &key, ptrdiff_t i) const {
    auto idx = index(key);
    if (idx == -1) {
      return key;
    } else {
      return at(idx + i);
    }
  }
};

} // namespace frozenca

#endif //__CLRS4_ORDER_RANK_TREE_H__
