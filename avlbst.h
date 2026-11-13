#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
    void rotateLeft(AVLNode<Key, Value>* node);
    void rotateRight(AVLNode<Key, Value>* node);
    void fixBalance(AVLNode<Key, Value>* node, int diff, bool inserting);

};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    // TODO
    if (this->root_ == nullptr) {
      this->root_ = new AVLNode<Key, Value>(new_item.first, new_item.second, nullptr);
      return;
    }
    AVLNode<Key, Value>* current = static_cast<AVLNode<Key, Value>*>(this->root_);
    while (true) {
      if (new_item.first < current->getKey()) {
        if (current->getLeft() == nullptr) {
          current->setLeft(new AVLNode<Key, Value>(new_item.first, new_item.second, current));
          fixBalance(current, -1, true);
          return;
        }
        current = current->getLeft();
      }
      else if (current->getKey() < new_item.first) {
        if (current->getRight() == nullptr) {
          current->setRight(new AVLNode<Key, Value>(new_item.first, new_item.second, current));
          fixBalance(current, 1, true);
          return;
        }
        current = current->getRight();
      }
      else {
        current->setValue(new_item.second);
        return;
      }
    }
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    // TODO
    AVLNode<Key, Value>* target = static_cast<AVLNode<Key, Value>*>(this->internalFind(key));
    if (target == nullptr) {
      return;
    }
    if (target->getLeft() != nullptr && target->getRight() != nullptr) {
      AVLNode<Key, Value>* pred = static_cast<AVLNode<Key, Value>*>(this->predecessor(target));
      nodeSwap(target, pred);
    }
    AVLNode<Key, Value>* child = target->getLeft();
    if (child == nullptr) {
      child = target->getRight();
    }
    AVLNode<Key, Value>* parent = target->getParent();
    int diff = 0;
    if (parent == nullptr) {
      this->root_ = child;
    }
    else if (parent->getLeft() == target) {
      parent->setLeft(child);
      diff = 1;
    }
    else {
      parent->setRight(child);
      diff = -1;
    }
    if (child != nullptr) {
      child->setParent(parent);
    }
    delete target;
    fixBalance(parent, diff, false);
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft( AVLNode<Key,Value>* node)
{
    AVLNode<Key, Value>* right = node->getRight();
    AVLNode<Key, Value>* parent = node->getParent();
    node->setRight(right->getLeft());
    if (node->getRight() != nullptr) {
      node->getRight()->setParent(node);
    }
    right->setParent(parent);
    if (parent == nullptr) {
      this->root_ = right;
    }
    else if (parent->getLeft() == node) {
      parent->setLeft(right);
    }
    else {
      parent->setRight(right);
    }
    right->setLeft(node);
    node->setParent(right);
    int rightBalance = right->getBalance();
    int nodeBalance = node->getBalance() - 1 - std::max(rightBalance, 0);
    node ->setBalance(nodeBalance);
    right->setBalance(rightBalance - 1 + std::min(nodeBalance, 0));
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight( AVLNode<Key,Value>* node)
{
    AVLNode<Key, Value>* left = node->getLeft();
    AVLNode<Key, Value>* parent = node->getParent();
    node->setLeft(left->getRight());
    if (node->getLeft() != nullptr) {
      node->getLeft()->setParent(node);
    }
    left->setParent(parent);
    if (parent == nullptr) {
      this->root_ = left;
    }
    else if (parent->getLeft() == node) {
      parent->setLeft(left);
    }
    else {
      parent->setRight(left);
    }
    left->setRight(node);
    node->setParent(left);
    int leftBalance = left->getBalance();
    int nodeBalance = node->getBalance() + 1 - std::min(leftBalance, 0);
    node ->setBalance(nodeBalance);
    left->setBalance(leftBalance + 1 + std::max(nodeBalance, 0));
}

template<class Key, class Value>
void AVLTree<Key, Value>::fixBalance( AVLNode<Key,Value>* node, int diff, bool inserting)
{
    while (node != nullptr) {
      node->updateBalance(diff);
      int balance = node->getBalance();
      if (inserting && balance == 0) {
        return;
      }
      if (!inserting && (balance == -1 || balance == 1)) {
        return;
      }
      if (balance == -2 || balance == 2) {
        if (balance == -2) {
          if (node->getLeft()->getBalance() > 0) {
            rotateLeft(node->getLeft());
          }
          rotateRight(node);
        }
        else {
          if (node->getRight()->getBalance() < 0) {
            rotateRight(node->getRight());
          }
          rotateLeft(node);
        }
        node = node->getParent();
        if (inserting || node->getBalance() != 0) {
          return;
        }
      }
      AVLNode<Key, Value>* parent = node->getParent();
      if (parent == nullptr) {
        return;
      }
      if (node == parent->getLeft()) {
        diff = -1;
      }
      else {
        diff = 1;
      }
      if (!inserting) {
        diff = -diff;
      }
      node = parent;
    }
}

#endif
