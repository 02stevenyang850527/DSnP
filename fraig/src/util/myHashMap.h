/****************************************************************************
  FileName     [ myHashMap.h ]
  PackageName  [ util ]
  Synopsis     [ Define HashMap and Cache ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2009-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_HASH_MAP_H
#define MY_HASH_MAP_H

#include <vector>

using namespace std;

// TODO: (Optionally) Implement your own HashMap and Cache classes.

//-----------------------
// Define HashMap classes
//-----------------------
// To use HashMap ADT, you should define your own HashKey class.
// It should at least overload the "()" and "==" operators.
//
 class HashKey
 {
 public:
    HashKey(size_t i0, size_t i1){
       if (i0 < i1)
          _key = (i0 << 32) + i1;
       else
          _key = (i1 << 32) + i0;
    }
 
    size_t operator() () const { return _key; }
    bool operator == (const HashKey& k) const { return (_key == k._key); }
 
 private:
    size_t _key;
 };

template <class HashKey, class HashData>
class HashMap
{
typedef pair<HashKey, HashData> HashNode;

public:
   HashMap(size_t b=0) : _numBuckets(0), _buckets(0) { if (b != 0) init(b); }
   ~HashMap() { reset(); }

   // [Optional] TODO: implement the HashMap<HashKey, HashData>::iterator
   // o An iterator should be able to go through all the valid HashNodes
   //   in the HashMap
   // o Functions to be implemented:
   //   - constructor(s), destructor
   //   - operator '*': return the HashNode
   //   - ++/--iterator, iterator++/--
   //   - operators '=', '==', !="
   //
   class iterator
   {
      friend class HashMap<HashKey, HashData>;

   public:
      iterator(vector<HashNode>* _b, size_t num, size_t _r, size_t _c = 0):
         _buckets(_b), _bucketNum(num), _row(_r), _col(_c) {}
      iterator( const iterator& i):
         _buckets(i._buckets), _bucketNum(i._bucketNum), _row(i._row), _col(i._col) {}
      ~iterator() {}
      const HashNode& operator* () const { return _buckets[_row][_col]; }
      HashNode& operator* () { return _buckets[_row][_col]; }

      iterator& operator ++ (){
         if (_row >= _bucketNum) return *this;
         if (_col + 1 < _buckets[_row].size())
            ++_col;
         else{
            while(++_row < _bucketNum)
               if (_buckets[_row].size() > 0)
                  break;
            _col = 0;
         }
         return *this;
      }

      iterator operator ++ (int) {
         iterator temp = *this;
         ++(*this);
         return temp;
      }

      iterator& operator = (const iterator& i){
         _buckets = i._buckets;
         _row = i._row;
         _col = i._col;
         _bucketNum = i._bucketNum;
         return *this;
      }

      bool operator == (const iterator& i){ return (i._row == _row && i._col == _col); }
      bool operator != (const iterator& i){ return !(*this == i); } 
      
   private:
      vector<HashNode>* _buckets;
      size_t _bucketNum;
      size_t _row, _col;
   };

   void init(size_t b) {
      reset(); _numBuckets = b; _buckets = new vector<HashNode>[b]; }
   void reset() {
      _numBuckets = 0;
      if (_buckets) { delete [] _buckets; _buckets = 0; }
   }
   void clear() {
      for (size_t i = 0; i < _numBuckets; ++i) _buckets[i].clear();
   }
   size_t numBuckets() const { return _numBuckets; }

   vector<HashNode>& operator [] (size_t i) { return _buckets[i]; }
   const vector<HashNode>& operator [](size_t i) const { return _buckets[i]; }

   // TODO: implement these functions
   //
   // Point to the first valid data
   iterator begin() const { 
      if (_buckets == 0) return end();
      for (unsigned i = 0; i < _numBuckets; ++i)
         if (_buckets[i].size() > 0)
            return iterator(_buckets, _numBuckets, i);

      return end();
   }
   // Pass the end
   iterator end() const { return iterator(_buckets, _numBuckets, _numBuckets); }
   // return true if no valid data
   bool empty() const {
      for (size_t i = 0; i < _numBuckets; ++i)
         if (_buckets[i].size() > 0)
            return false;
      return true;
   }
   // number of valid data
   size_t size() const {
      size_t s = 0;
      for (size_t i = 0; i < _numBuckets; ++i)
         s += _buckets[i].size();
      return s;
   }

   // check if k is in the hash...
   // if yes, return true;
   // else return false;
   bool check(const HashKey& k) const {
      size_t loc = bucketNum(k);
      for (size_t i = 0; i < _buckets[loc].size(); ++i)
         if (k == _buckets[loc][i].first)
            return true;

      return false;
   }

   bool checkINV(const HashKey& k) const {
      size_t loc = bucketNum(k);
      for (size_t i = 0; i < _buckets[loc].size(); ++i)
         if (k == _buckets[loc][i].first)
            return k.isInvert(_buckets[loc][i].first);

      return false;
   }
   // query if k is in the hash...
   // if yes, replace d with the data in the hash and return true;
   // else return false;
   bool query(const HashKey& k, HashData& d) const {
      size_t loc = bucketNum(k);
      for (size_t i = 0; i < _buckets[loc].size(); ++i)
         if (k == _buckets[loc][i].first){
            d = _buckets[loc][i].second;
            return true;
         }
      return false;
   }

   // update the entry in hash that is equal to k (i.e. == return true)
   // if found, update that entry with d and return true;
   // else insert d into hash as a new entry and return false;
   bool update(const HashKey& k, HashData& d) {
      size_t loc = bucketNum(k);
      for (size_t i = 0; i < _buckets[loc].size(); ++i)
         if (k == _buckets[loc][i].first){
            _buckets[loc][i].second = d;
            return true;
         }
      return false;
   }

   // return true if inserted d successfully (i.e. k is not in the hash)
   // return false is k is already in the hash ==> will not insert
   bool insert(const HashKey& k, const HashData& d) {
      size_t loc = bucketNum(k);
      for (size_t i = 0; i < _buckets[loc].size(); ++i)
         if (k == _buckets[loc][i].first)
            return false;
      _buckets[loc].push_back(HashNode(k,d));
      return true;
   }

   // return true if removed successfully (i.e. k is in the hash)
   // return fasle otherwise (i.e. nothing is removed)
   bool remove(const HashKey& k) { return false; }

private:
   // Do not add any extra data member
   size_t                   _numBuckets;
   vector<HashNode>*        _buckets;

   size_t bucketNum(const HashKey& k) const {
      return (k() % _numBuckets); }

};


//---------------------
// Define Cache classes
//---------------------
// To use Cache ADT, you should define your own HashKey class.
// It should at least overload the "()" and "==" operators.
//
// class CacheKey
// {
// public:
//    CacheKey() {}
//    
//    size_t operator() () const { return 0; }
//   
//    bool operator == (const CacheKey&) const { return true; }
//       
// private:
// }; 
// 
template <class CacheKey, class CacheData>
class Cache
{
typedef pair<CacheKey, CacheData> CacheNode;

public:
   Cache() : _size(0), _cache(0) {}
   Cache(size_t s) : _size(0), _cache(0) { init(s); }
   ~Cache() { reset(); }

   // NO NEED to implement Cache::iterator class

   // TODO: implement these functions
   //
   // Initialize _cache with size s
   void init(size_t s) { reset(); _size = s; _cache = new CacheNode[s]; }
   void reset() {  _size = 0; if (_cache) { delete [] _cache; _cache = 0; } }

   size_t size() const { return _size; }

   CacheNode& operator [] (size_t i) { return _cache[i]; }
   const CacheNode& operator [](size_t i) const { return _cache[i]; }

   // return false if cache miss
   bool read(const CacheKey& k, CacheData& d) const {
      size_t i = k() % _size;
      if (k == _cache[i].first) {
         d = _cache[i].second;
         return true;
      }
      return false;
   }
   // If k is already in the Cache, overwrite the CacheData
   void write(const CacheKey& k, const CacheData& d) {
      size_t i = k() % _size;
      _cache[i].first = k;
      _cache[i].second = d;
   }

private:
   // Do not add any extra data member
   size_t         _size;
   CacheNode*     _cache;
};


#endif // MY_HASH_H
