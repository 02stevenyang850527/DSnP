/****************************************************************************
  FileName     [ dlist.h ]
  PackageName  [ util ]
  Synopsis     [ Define doubly linked list package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef DLIST_H
#define DLIST_H

#include <cassert>

template <class T> class DList;

// DListNode is supposed to be a private class. User don't need to see it.
// Only DList and DList::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class DListNode
{
   friend class DList<T>;
   friend class DList<T>::iterator;

   DListNode(const T& d, DListNode<T>* p = 0, DListNode<T>* n = 0):
      _data(d), _prev(p), _next(n) {}

   T              _data;
   DListNode<T>*  _prev;
   DListNode<T>*  _next;
};


template <class T>
class DList
{
public:
   // TODO: decide the initial value for _isSorted
   DList() {
      _head = new DListNode<T>(T());
      _head->_prev = _head->_next = _head; // _head is a dummy node
   }
   ~DList() { clear(); delete _head; }

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class DList;

   public:
      iterator(DListNode<T>* n= 0): _node(n) {}
      iterator(const iterator& i) : _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const { return _node->_data; }
      T& operator * () { return _node->_data; }
      iterator& operator ++ () { _node = _node->_next;	return *(this); }

      iterator operator ++ (int) {
			iterator temp(_node);
			_node = _node->_next;
			return temp;
	  }
      iterator& operator -- () { _node = _node->_prev;	return *(this); }

      iterator operator -- (int){
			iterator temp(_node);
			_node = _node->_prev;
		 	return temp; 
	  }

      iterator& operator = (const iterator& i) { _node = i._node;	return *(this); }

      bool operator != (const iterator& i) const {
			if (_node == i._node)
				return false; 
			else return true;
	  }
      bool operator == (const iterator& i) const {
			if (_node == i._node)
				return true;
			else return false;
	  }

   private:
      DListNode<T>* _node;
   };

   // TODO: implement these functions
   iterator begin() const { return iterator(_head->_next); }
   iterator end() const { return iterator(_head); }
   bool empty() const {
	if (_head == _head->_next)
		return true;
	else return false;
   }
   size_t size() const {
	 size_t count = 0;
	 for (iterator i = begin(); i != end(); ++i)
		 count++;
	 return count;
   }

   void push_back(const T& x) {
	DListNode<T>* p = _head;
	DListNode<T>* n = new DListNode<T>(x, p->_prev, p);
	n->_prev->_next = n;
	n->_next->_prev = n;
   }
   void pop_front() { erase(begin());}
   void pop_back() { erase(--end());}

   // return false if nothing to erase
   bool erase(iterator pos) {
		if (pos == end())
			return false;
		else{
			DListNode<T>* temp = pos._node;
			temp->_prev->_next = temp->_next;
			temp->_next->_prev = temp->_prev;
			delete pos._node;
			return true;
		}
   }

   bool erase(const T& x) {
		for (iterator i = begin(); i != end() ; ++i){
			if (*i == x )
				{	 erase(i); return true; }
		}	
		return false; 
   }

   void clear() {   // delete all nodes except for the dummy node
		if (!empty()){
			DListNode<T>* temp = _head->_next->_next;
			while(1){
				delete temp->_prev;
				if (temp == _head) break;
				temp = temp->_next;
			}
			_head->_next = _head->_prev = _head;
		}
   }

   void sort() const { insertion_sort();}

private:
   DListNode<T>*  _head;     // = dummy node if list is empty
  // mutable bool   _isSorted; // (optionally) to indicate the array is sorted

   void insertion_sort() const{
		for (iterator i = ++begin(); i != end(); ++i){
			T temp = i._node->_data;
			iterator j = i;
			for ( ; j != begin() && temp < j._node->_prev->_data; j--)
				j._node->_data = j._node->_prev->_data;
			j._node->_data = temp;
  		}
	}

/*   void insert(const iterator a, const iterator b) const{ //insert b before a
		if (a == b) return;
		DListNode<T>* i = (a._node)->_prev;
		DListNode<T>* j = (b._node)->_next;
		i->_next = b._node;
		b._node->_next = a._node;
		a._node->_next = j;
		j->_prev = a._node;
		a._node->_prev = b._node;
		b._node->_prev = i;
   }
	
	void swap(const iterator i, const iterator j) const{
		DListNode<T>* a = i._node;
		DListNode<T>* b = j._node;
		T* temp = a->_data;
		b->_data = a->_data;
		a->_data = temp;
	}
  */ 
// [OPTIONAL TODO] helper functions; called by public member functions
};

#endif // DLIST_H
