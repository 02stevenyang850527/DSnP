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
		_data(d), _parent(p), _left(l), _right(r), _start(s) {}

	T					_data;
	BSTreeNode<T>*	_parent;
	BSTreeNode<T>*	_left;
	BSTreeNode<T>*	_right;
	BSTreeNode<T>* _start;    //only for _root
};


template <class T>
class BSTree
{
   // TODO: design your own class!!
public:

	BSTree(){
		_root = new BSTreeNode<T>(T());
		_root->_left = _root->_right = _root->_parent = _root->_start = 0; // _root is a dummy node
		_size = 0;
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

	iterator begin() const {
		if (empty()) return 0;

		BSTreeNode<T>* temp = _root->_start;
		while ( temp->_left != 0){
			temp = temp->_left;
		}
		return iterator(temp);
	}

	iterator end() const { 
		if (empty()) return 0;

		return iterator(_root);
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
		if (empty())
			return;
		erase(--end());
	}

	void insert(const T& x){
		if (empty()){
			BSTreeNode<T>* n = new BSTreeNode<T>(x, 0, 0, _root, 0);
			_root->_parent = n;
			_root->_start = n;   // special case, only for initialization
		}
		else{
			BSTreeNode<T>* temp = _root->_start;
			while (1){
				if (x > temp->_data){
					if ( temp->_right != 0){
						if ( temp->_right != _root)
							temp = temp->_right;
						else{
							BSTreeNode<T>* n = new BSTreeNode<T>(x, temp, 0, _root, 0);
							temp->_right = n;
							_root->_parent = n;
							break;
						}
					}
					else{
						BSTreeNode<T>* n = new BSTreeNode<T>(x, temp, 0, 0, 0);
						temp->_right = n;
						break;
					}
				}
				else{
					if ( temp->_left != 0)
						temp = temp->_left;
					else{
						BSTreeNode<T>* n = new BSTreeNode<T>(x, temp, 0, 0, 0);
						temp->_left = n;
						break;
					}
				}
			} //end the loop
		}
	//	cout << "insert done: " << x << endl;
		_size++;
	}
	
	bool erase(iterator pos) {
			if (empty()) return false;
		//	cout << "size is " << _size << endl;
			if (_size == 1){
				_root->_parent = 0;
				_root->_start = 0;
				
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
				temp._node->_parent->_left = 0;
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

	//	cout << "size is " << _size << endl;
		return true;
	} //TODO
	bool erase(const T& x) { return false; }   //TODO
	void clear() {
		while (_size != 0)
			erase(begin());
	}

	void print() {}    //TODO
	void sort() {}		 //no need to implement

private:
	BSTreeNode<T>* _root;
	size_t 			_size;

};

#endif // BST_H
