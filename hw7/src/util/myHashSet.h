/****************************************************************************
  FileName     [ myHashSet.h ]
  PackageName  [ util ]
  Synopsis     [ Define HashSet ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_HASH_SET_H
#define MY_HASH_SET_H

#include <vector>

using namespace std;

//---------------------
// Define HashSet class
//---------------------
// To use HashSet ADT,
// the class "Data" should at least overload the "()" and "==" operators.
//
// "operator ()" is to generate the hash key (size_t)
// that will be % by _numBuckets to get the bucket number.
// ==> See "bucketNum()"
//
// "operator ()" is to check whether there has already been
// an equivalent "Data" object in the HashSet.
// Note that HashSet does not allow equivalent nodes to be inserted
//
template <class Data>
class HashSet
{
public:
   HashSet(size_t b = 0) : _numBuckets(0), _buckets(0) { if (b != 0) init(b); }
   ~HashSet() { reset(); }

   // TODO: implement the HashSet<Data>::iterator
   // o An iterator should be able to go through all the valid Data
   //   in the Hash
   // o Functions to be implemented:
   //   - constructor(s), destructor
   //   - operator '*': return the HashNode
   //   - ++/--iterator, iterator++/--
   //   - operators '=', '==', !="
   //
   class iterator
   {
      friend class HashSet<Data>;

   public:
		iterator(vector <Data>* b, size_t num, size_t m, size_t n = 0):
			_bucks(b), _numB(num), _row(m), _col(n){}
		iterator(const iterator& i):
			_bucks(i._bucks), _numB(i._numB), _row(i._row), _col(i._col){}
		~iterator() {}
		const Data& operator * () const { return _bucks[_row][_col]; }
		Data& operator * () { return _bucks[_row][_col];}
		iterator& operator ++ (){
			if (_row >= _numB)
				return *this;   // case: ++end()
			if (_col+1 < _bucks[_row].size())
				++_col;
			else{
				while (++_row < _numB)
					if (_bucks[_row].size() > 0)
						break;
				_col = 0;
			}
			return *this;
		}
		iterator operator ++ (int){
			iterator temp = iterator(*this);
			++(*this);
			return temp;
		}

		iterator& operator -- (){    // will not be called ...zz
			if (_row < 0)
				return *this;
			if (_col > 0)
				--_col;
			else{
				size_t i = _row;
				while (i > 0){
					--i;
					if (_bucks[i].size() > 0){
						_col = _bucks[i].size()-1;
						_row = i;
						break;
					}
				}
			}
			return *this;
		}
		iterator operator -- (int){
			iterator temp = iterator(*this);
			--(this);
			return temp;
		}

		iterator& operator = (const iterator& i){
			_numB = i._numB;
			_row = i._row;
			_col = i._col;
			_bucks = i._bucks;
			return *this;
		}
		bool operator == (const iterator& i){ return ((_row == i._row) && (_col == i._col)); }
		bool operator != (const iterator& i){ return !(*this == i); }

   private:
		vector <Data>* _bucks;
		size_t _numB, _row, _col;
   };

   void init(size_t b) { _numBuckets = b; _buckets = new vector<Data>[b]; }
   void reset() {
      _numBuckets = 0;
      if (_buckets) { delete [] _buckets; _buckets = 0; }
   }
   void clear() {
      for (size_t i = 0; i < _numBuckets; ++i) _buckets[i].clear();
   }
   size_t numBuckets() const { return _numBuckets; }

   vector<Data>& operator [] (size_t i) { return _buckets[i]; }
   const vector<Data>& operator [](size_t i) const { return _buckets[i]; }

   // TODO: implement these functions
   //
   // Point to the first valid data
   iterator begin() const {
		for (size_t i = 0; i < _numBuckets; ++i){
			if (_buckets[i].size() > 0)
				return iterator(_buckets, _numBuckets, i);
		}
		return end();
	}
   // Pass the end
   iterator end() const { return iterator(_buckets,_numBuckets, _numBuckets); } //TODO
   // return true if no valid data
   bool empty() const {
		for (size_t i = 0; i < _numBuckets; ++i){
			if (_buckets[i].size() > 0)
				return false;
		}
		return true;
	}
   // number of valid data
   size_t size() const {
		size_t s = 0;
		for (size_t i = 0; i < _numBuckets; ++i)
			s += _buckets[i].size();
		return s;
	}

   // check if d is in the hash...
   // if yes, return true;
   // else return false;
   bool check(const Data& d) const { 
		size_t loc = bucketNum(d);
		for (size_t i = 0; i < _buckets[loc].size(); ++i)
			if (_buckets[loc][i] == d)
				return true;

		return false;
	}

   // query if d is in the hash...
   // if yes, replace d with the data in the hash and return true;
   // else return false;
   bool query(Data& d) const { 
		size_t loc = bucketNum(d);
		for (size_t i = 0; i < _buckets[loc].size(); ++i)
			if (_buckets[loc][i] == d){
				d = _buckets[loc][i];
				return true;
			}
		return false;
	}

   // update the entry in hash that is equal to d (i.e. == return true)
   // if found, update that entry with d and return true;
   // else insert d into hash as a new entry and return false;
   bool update(const Data& d) {
		size_t loc = bucketNum(d);
		for (size_t i = 0; i < _buckets[loc].size(); ++i)
			if (_buckets[loc][i] == d)
				return true;

		_buckets[loc].push_back(d);
		return false;
	}

   // return true if inserted successfully (i.e. d is not in the hash)
   // return false is d is already in the hash ==> will not insert
   bool insert(const Data& d) {
		size_t loc = bucketNum(d);
		for (size_t i = 0; i < _buckets[loc].size(); ++i)
			if (_buckets[loc][i] == d)
				return false;

		_buckets[loc].push_back(d);
		return true;
	}

   // return true if removed successfully (i.e. d is in the hash)
   // return fasle otherwise (i.e. nothing is removed)
   bool remove(const Data& d) {
		size_t loc = bucketNum(d);
		for (size_t i = 0; i < _buckets[loc].size(); ++i)
			if (_buckets[loc][i] == d){
				_buckets[loc].erase(_buckets[loc].begin() + i);
				return true;
			}
		return false;
	}

private:
   // Do not add any extra data member
   size_t            _numBuckets;
   vector<Data>*     _buckets;

   size_t bucketNum(const Data& d) const {
      return (d() % _numBuckets); }
};

#endif // MY_HASH_SET_H
