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
	
	BSTreeNode(const T& d,BSTreeNode<T>* p = 0 , BSTreeNode<T>* l = 0, BSTreeNode<T>* r = 0, BSTreeNode<T>* s = 0):
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
		dummy = new BSTreeNode<T>(T());
		dummy->_left = dummy->_right = dummy->_parent = 0; // _root is a dummy node
		_size = 0;
	}
	~BSTree() { clear(); delete dummy; }

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
					while (temp->_parent->_left != temp){
				//		if (temp->_parent->_left == 0)
				//			break;					// for the case: ++end()
						temp = temp->_parent;
					}
					temp = temp->_parent;
					_node = temp;
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
					while (temp->_parent->_right != temp){
						temp = temp->_parent;
					}
					_node = temp;
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
	
	iterator find_root() const {
		if (empty()) return 0;
		BSTreeNode<T>* _root = dummy;
		while (_root->_parent != 0)
			_root = _root->_parent;
		
		return iterator(_root);
	}

	iterator begin() const {
		if (empty()) return 0;

		BSTreeNode<T>* temp = find_root()._node;

		while (temp->_left != 0){
			temp = temp->_left;
		}
		return iterator(temp);
	}

	iterator end() const { 
		if (empty()) return 0;

		return iterator(dummy);
	}

	bool empty() const {
		return (_size == 0);
	}

	size_t size() const {
		return _size;
	}
	
	void pop_front() {
		if (empty())
			return;
		erase(begin());
	}

	void pop_back() {
/*		iterator temp = end();
		--temp;
		cout << "begin(): " << *begin() << endl;
		cout << "end(): " << *temp << endl;
		cout << "size is " << _size << endl;*/
		if (empty())
			return;
		erase(--end());
	}

	void insert(const T& x){
		if (empty()){
			BSTreeNode<T>* n = new BSTreeNode<T>(x, 0, 0, dummy);
			dummy->_parent = n;
		}
		else{
			BSTreeNode<T>* temp = find_root()._node;
			while (1){
				if (x > temp->_data){
					if ( temp->_right != 0){
						if ( temp->_right != dummy)
							temp = temp->_right;
						else{
							BSTreeNode<T>* n = new BSTreeNode<T>(x, temp, 0, dummy);
							temp->_right = n;
							dummy->_parent = n;
							break;
						}
					}
					else{
						BSTreeNode<T>* n = new BSTreeNode<T>(x, temp, 0, 0);
						temp->_right = n;
						break;
					}
				}
				else{
					if ( temp->_left != 0)
						temp = temp->_left;
					else{
						BSTreeNode<T>* n = new BSTreeNode<T>(x, temp, 0, 0);
						temp->_left = n;
						break;
					}
				}
			} //end the loop
		}
		_size++;
	}
	
	bool erase(iterator pos) {
			if (empty()) return false;
			if (_size == 1){
				dummy->_parent = 0;
				--_size;
				delete pos._node;
				return true;
			}
			
			iterator temp = pos;
			++temp;  // minimum iterator larger than pos
			if (temp._node->_left == pos._node){
				temp._node->_left = pos._node->_left;
				if (pos._node->_left != 0)
					pos._node->_left->_parent = temp._node;
			}
			else if (temp._node->_parent == pos._node){
				temp._node->_parent = pos._node->_parent;
				if (pos._node->_parent != 0){
					if (pos._node->_parent->_left == pos._node)
						pos._node->_parent->_left = temp._node;
					else
						pos._node->_parent->_right = temp._node;
				}
				temp._node->_left = pos._node->_left;
				if (pos._node->_left != 0)
					pos._node->_left->_parent = temp._node;
			}
			else if (pos._node->_right == 0){
				pos._node->_parent->_right = pos._node->_left;
				if (pos._node->_left != 0)
					pos._node->_left->_parent = pos._node->_parent;
			}
			else{
				temp._node->_parent->_left = temp._node->_right;
				if (temp._node->_right != 0)
					temp._node->_right->_parent = temp._node->_parent;

				temp._node->_parent = pos._node->_parent;
				if (pos._node->_parent->_left == pos._node)
					pos._node->_parent->_left = temp._node;
				else
					pos._node->_parent->_right = temp._node;

				temp._node->_left = pos._node->_left;
				temp._node->_right = pos._node->_right;
				if (pos._node->_left != 0)
					pos._node->_left->_parent = temp._node;
				if (pos._node->_right != 0)
					pos._node->_right->_parent = temp._node;	
			}
			
		delete pos._node;
		--_size;

		return true;
	}

	bool erase(const T& x) {
		iterator i = begin();
		for ( ; i != end(); ++i){
			if (*i == x){
				erase(i);
				return true; 
				break;
			}
		}
//		if (*i == x){   //for the case i = begin()
//			erase(i);
//			return true;
//		}

		return false;
	}

	void clear() {
		while (_size != 0)
			erase(--end());
	}

	void print() {}    //TODO
	void sort() {}		 //no need to implement

private:
	BSTreeNode<T>* dummy;
	size_t 			_size;

};

#endif // BST_H
