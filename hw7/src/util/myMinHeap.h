/****************************************************************************
  FileName     [ myMinHeap.h ]
  PackageName  [ util ]
  Synopsis     [ Define MinHeap ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_MIN_HEAP_H
#define MY_MIN_HEAP_H

#include <algorithm>
#include <vector>

template <class Data>
class MinHeap
{
public:
   MinHeap(size_t s = 0) { if (s != 0) _data.reserve(s); }
   ~MinHeap() {}

   void clear() { _data.clear(); }

   // For the following member functions,
   // We don't respond for the case vector "_data" is empty!
   const Data& operator [] (size_t i) const { return _data[i]; }   
   Data& operator [] (size_t i) { return _data[i]; }

   size_t size() const { return _data.size(); }

   // TODO
   const Data& min() const { return _data[0]; }
   void insert(const Data& d) {
		int t = _data.size();
		_data.push_back(d);
		while (t > 0){
			int p = (t-1)/2 ;
			if (d >= _data[p])
				break;
			_data[t] = _data[p];
			t = p;
		}
		_data[t] = d;
	}

   void delMin() { delData(0); }
   void delData(size_t i) {
		int t = 2*i + 1;
		int n = _data.size();
		Data temp = _data[n-1];
		_data.pop_back();
		--n;
		if (i == n) return;

		while (t < n){
			if (t+1 < n)   // has right child
				if (_data[t] > _data[t+1])
					++t;  // to the smaller child
			if (temp < _data[t])
				break;
			_data[i] = _data[t];
			i = t;
			t = 2*i +1;
		}
	
	
	}

private:
   // DO NOT add or change data members
   vector<Data>   _data;
};

#endif // MY_MIN_HEAP_H
