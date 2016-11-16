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
		_data(d), _parent(p), _left(l), _right(r) {}
	
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
		_root->_left = _root->_right = _root->_parent = 0; // _root is a dummy node
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
				BSTreeNode<T>* temp = _node->_right;
				while (temp->_left != 0){
					temp = temp->_left;
				}
				_node = temp;
				return *this;
			}
			else{
				BSTreeNode<T>* temp = _node->_parent;
				if (temp->_left == _node){
					_node = temp;
					return *this;
				}
				else{
					_node = temp->_parent;
					return *this;
				}
			}
		}

		iterator operator ++ (int) {
			iterator temp = *this;
			++(*this);
			return temp;
		 }

		iterator& operator -- () {
			if (_node->_left != 0){
				BSTreeNode<T>* temp = _node->_left;
				while (temp->_right != 0){
					temp = temp->_right;
				}
				_node = temp;
				return *this;
			}
			else{
				BSTreeNode<T>* temp = _node->_parent;
				if (temp->_right == _node){
					_node = temp;
					return *this;
				}
				else{
					_node = temp->_parent;
					return *this;
				}
			}
		}

		iterator operator -- (int) {
			iterator temp = *this;
			--(*this);
			return temp;
		}

		iterator& operator = (const iterator& i) { _node = i._node; return *(this); }
		bool operator != (const iterator& i) const { return (_node != i._node); }
		bool operator == (const iterator& i) const { return (_node == i._node); }

	private:
		BSTreeNode<T>* _node;
	};

	iterator begin() const {
		if (empty()) return 0;

		BSTreeNode<T>* temp = _root;
		while ( temp->_left != 0){
			temp = temp->_left;
		}
		return iterator(temp);
	}

	iterator end() const { 
		if (empty()) return 0;

		BSTreeNode<T>* temp = _root;
		while ( temp->_right != 0){
			temp = temp->_right;
		}
		return ++iterator(temp);
	}

	bool empty() const {
		if ( (_root->_right == 0 && _root->_left == 0) && _root->_parent == 0)
			return true;
		else
			return false;
	}

	size_t size() const {
		size_t count = 0;
		if (empty())
			return count;
		for (iterator i = begin(); i != end(); ++i)
			count++;

		return count;
	}
	
	void pop_front() {
		if (empty())
			return;
		erase(begin());
	}

	void pop_back() {
		if (empty())
			return;
		erase(end());
	}

	void insert(const T& x){
		if (empty()){
			BSTreeNode<T>* n = new BSTreeNode<T>(x, _root, 0, 0);
			_root->_left = n;
			_root->_right = n;
		}
		else{
			BSTreeNode<T>* temp = _root->_left;
			while (1){
				if (x > temp->_data){
					if ( temp->_right != 0)
						temp = temp->_right;
					else{
						BSTreeNode<T>* n = new BSTreeNode<T>(x, temp, 0, 0);
						temp->_right = n;
						cout << size() << endl;
						break;
					}
				}
				else{
					if ( temp->_left != 0)
						temp = temp->_left;
					else{
						BSTreeNode<T>* n = new BSTreeNode<T>(x, temp, 0, 0);
						temp->_left = n;
						cout << size() << endl;
						break;
					}
				}
			} //end the loop
		}
	}      //TODO
	
	bool erase(iterator pos) { return false; } //TODO
	bool erase(const T& x) { return false; }   //TODO
	void clear() {}

	void print() {}    //no need to implement
	void sort() {}		 //no need to implement

private:
	BSTreeNode<T>* _root;


};

#endif // BST_H
