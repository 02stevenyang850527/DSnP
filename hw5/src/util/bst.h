/****************************************************************************
  FileName     [ bst.h ]
  PackageName  [ util ]
  Synopsis     [ Define binary search tree package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef BST_H
#define BST_H

#include <cassert>

using namespace std;

template <class T> class BSTree;

// BSTreeNode is supposed to be a private class. User don't need to see it.
// Only BSTree and BSTree::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class BSTreeNode
{
   // TODO: design your own class!!
	friend class BSTree<T>;
	friend class BSTree<T>::iterator;
	
	BSTreeNode(const T& d,BSTreeNode<T>* p = 0 , BSTreeNode<T>* l = 0, BSTreeNode<T>* r = 0):
		_data(d), _left(l), _right(r) {}
	
	T					_data;
	BSTreeNode<T>*	_parent;
	BSTreeNode<T>*	_left;
	BSTreeNode<T>*	_right;
};


template <class T>
class BSTree
{
   // TODO: design your own class!!
public:

	BSTree(){
		_root = new BSTreeNode<T>(T());
		_root->_left = _root->_right = _root; // _root is a dummy node
	}
	~BSTree() { clear(); delete _root; }

   class iterator 
	{
		friend class BSTree;

	public:
		iterator(BSTreeNode<T>* n= 0): _node(n) {}
		iterator(const iterator& i): _node(i._node) {}
		~iterator() {}

		const T& operator * () const { return _node->_data; }
		T& operator * () { return _node->_data; }

		iterator& operator ++ () { 
			if (_node->_right != 0){
				_node = _node->_right;
				return *(this);
			}
			else{
				BSTreeNode<T>* temp = _node->_parent;
				if (temp->_left == _node){
					_node = _node->_parent;
					return *(this);
				}
				else{
					++temp;
					_node = temp;
					return *(this);
				}
			}
		}

		iterator operator ++ (int) {
			iterator tmp = *this;

			if (_node->_right != 0)
				_node = _node->_right;
			else{
				BSTreeNode<T>* temp = _node->_parent;
				if (temp->_left == _node)
					_node = _node->_parent;
				else{
					++temp;
					_node = temp;
				}
			}
			return tmp;
		 }

		iterator& operator -- () {
			if (_node->_left != 0){
				_node = _node->_left;
				return *(this);
			}
			else{
				BSTreeNode<T>* temp = _node->_parent;
				if (_node->_right == _node){
					_node = _node->_parent;
					return *(this);
				}
				else{
					--temp;
					_node = temp;
					return *(this);
				}
			}
		}

		iterator operator -- (int) {
			iterator tmp = *this;
			if (_node->_left != 0)
				_node = _node->_left;
			else{
				BSTreeNode<T>* temp = _node->_parent;
				if (_node->_right == _node)
					_node = _node->_parent;
				else{
					--temp;
					_node = temp;
				}
			}
			return tmp;
		}

		iterator& operator = (const iterator& i) { _node = i._node; return *(this); }
		bool operator != (const iterator& i) const { return (_node != i._node); }
		bool operator == (const iterator& i) const { return (_node == i._node); }

	private:
		BSTreeNode<T>* _node;
	};

	iterator begin() const {
		BSTreeNode<T>* temp = _root->_left;
		while ( temp->_left == 0){
			temp = temp->_left;
		}
		return iterator(temp);
	} //TODO
	iterator end() const { return 0; }   //TODO
	bool empty() const { return false; }
	size_t size() const { return 0; }
	
	void push_back(const T&x) {}
	void pop_front() {}
	void pop_back() {}
	void insert(const T& x){}
	
	bool erase(iterator pos) { return false; }
	bool erase(const T& x) { return false; }
	void clear() {}

	void print() {}
	void sort() {}

private:
	BSTreeNode<T>* _root;

};

#endif // BST_H
