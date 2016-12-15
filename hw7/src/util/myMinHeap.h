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
			if (d < _data[p]){
				_data[t] = _data[p];
				t = p;
			}
			else
				break;
		}
		_data[t] = d;
	}

   void delMin() { delData(0); }
   void delData(size_t i) {
		if (_data.empty())
			return;
		int a = i+1; 
		int p = 2*a;
		while ( (unsigned)p <= size()){
			if ( (unsigned)p < size())
				if (_data[p] < _data[p-1]) ++p;
			if (_data[size()-1] < _data[p-1] || _data[size()-1].getLoad() == _data[p-1].getLoad())
				break;
			_data[a-1] = _data[p-1];
			a = p;
			p = 2*a;
		}
		_data[a-1] = _data[size()-1];

		while (a > 1){
			if (_data[a-1] < _data[a/2-1]){
				swap(_data[a-1], _data[a/2-1]);
			}
			else
				break;
			a /= 2;
		}
		_data.resize(size()-1);
	}

private:
   // DO NOT add or change data members
   vector<Data>   _data;
};

#endif // MY_MIN_HEAP_H
