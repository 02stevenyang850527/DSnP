/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir simulation functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cassert>
#include <cmath>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Keep "CirMgr::randimSim()" and "CirMgr::fileSim()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

class FKey
{
public:
   FKey(CirGate* p): value(p->getValue()) {}
   size_t operator() () const {
      if (~value < value) 
         return ~value;
      else
         return value;
   }
   bool operator == (const FKey& k) const { return (*this)() == k(); }
   bool isInvert (const FKey& k) const { return (value == ~k.value); }
private:
   unsigned value;
};

class SKey
{
public:
   SKey(CirGate* p, bool inv){
      if (inv)
         value = ~(p->getValue());
      else
         value = p->getValue();
   }
   size_t operator() () const { return value; }
   bool operator == (const SKey& k) const { return (value == k.value); }
private:
   unsigned value;
};

FKey getFKey(CirGate* p) { return FKey(p); }
SKey getSKey(CirGate* p, bool inv) { return SKey(p,inv); }

/************************************************/
/*   Public member functions about Simulation   */
/************************************************/
void
CirMgr::randomSim()
{
/*   IdList idtemp;
   CirGate::setGlobalRef();
   for (unsigned k = 0; k < o; ++k){
      CirGate* temp = getGate(m+k+1);
      temp->dfs4Write(idtemp);
   }*/
//   unsigned max = log(idtemp.size() + o + i) / log(2.5) + 0.5;
   unsigned max = unsigned(3.5*sqrt(double(i)));
   unsigned fail = 0, pre = 0, num = 0;
   vector<unsigned> pattern(i);
   cout << "MAX_FAILS = " << max << endl;
   while (fail < max){
      for (unsigned k = 0; k < i; ++k)
         pattern[k] = rand();
      num += 32;
      sim(pattern);
      if (_fecList.size() == pre) ++fail;
      pre = _fecList.size();
   }
   cout << num << " pattern simulated. \n";
}

void
CirMgr::fileSim(ifstream& patternFile)
{
   unsigned num = 0;
   vector<unsigned> pattern(i, 0);
   string line;
   while (!patternFile.eof()){
      patternFile >> line;
      while (patternFile.peek() == '\n' || patternFile.peek() == ' ')
         patternFile.get();
      ++num;
      if (line.size() != i){
         cerr << "\nError: Pattern(" << line << ") length(" << line.size()
              << ") does not match the number of inputs(" << i << ") in a circuit!!" << endl;
      }
      for (unsigned k = 0; k < i; ++k){
         if (isdigit(line[k] - '0'))
         cerr << "\nError: Pattern(" << line << ") contains a non-0/1 character(\'" << line[k] << "\')" << endl;
         pattern[k] = (pattern[k] << 1) + line[k] - '0';
      }
      if ((num % 32) == 0){
         sim(pattern);
         makeLog();
         vector<unsigned> temp(i, 0);
         pattern.swap(temp);
      }
   }
   if (num%32) {
      sim(pattern);
      makeLog();
   }
   cout << num << " patterns simulated.\n";
}

void
CirMgr::sim(vector<unsigned>& pattern)
{
   for (unsigned k = 0; k < i; ++k)
      getGate(input[k]/2)->setValue(pattern[k]);

   if (_fecList.empty()){
      IdList idtemp;
      CirGate::setGlobalRef();
      for (unsigned k = 0; k < o; ++k){
         CirGate* temp = getGate(m+k+1);
         temp->dfs4Write(idtemp);
      }
      idtemp.push_back(0);
      
      for (unsigned k = 0; k < idtemp.size(); ++k)
         getGate(idtemp[k])->initValue();

      sort(idtemp.begin(),idtemp.end());
      HashMap<FKey, IdList*> fecHash1(getHashSize(idtemp.size()));
      for (unsigned k = 0; k < idtemp.size(); ++k){
         FKey ke = getFKey(getGate(idtemp[k]));
         vector<unsigned>* z;
         if (!fecHash1.query(ke,z)){
            z = new IdList();
            _fecList.push_back(z);
            getGate(idtemp[k])->setFec(z,0);
            z->push_back(idtemp[k]*2);
            fecHash1.insert(ke,z);
         }
         else{
            getGate(idtemp[k])->setFec(z,z->size());
            z->push_back(2*idtemp[k] + fecHash1.checkINV(ke));
         }
      }
   }
   else{
      IdList idtemp;
      CirGate::setGlobalRef();
      for (unsigned k = 0; k < o; ++k){
         CirGate* temp = getGate(m+k+1);
         temp->dfs4Write(idtemp);
      }

      CirGate::setGlobalRef();
      for (unsigned k = 0; k < idtemp.size(); ++k)
         getGate(idtemp[k])->simulate();
      for (unsigned k = 0; k < o; ++k)
         getGate(m+k+1)->simulate();

      vector<IdList*> newfec;
      for (unsigned k = 0; k < _fecList.size(); ++k){
         HashMap<SKey,IdList*> fecHash(getHashSize(_fecList[k]->size()));
         for (unsigned j = 0; j < _fecList[k]->size(); ++j){
            unsigned temp = (*_fecList[k])[j];
            SKey ke = getSKey(getGate(temp/2), temp % 2);
            vector<unsigned>* z;
            if (!fecHash.query(ke,z)){
               z = new IdList();
               newfec.push_back(z);
               z->push_back(temp);
               fecHash.insert(ke,z);
               getGate(temp/2)->setFec(z, 0);
            }
            else{
               getGate(temp/2)->setFec(z,z->size());
               z->push_back(temp);
            }
         }
      }
      for (unsigned k = 0; k < _fecList.size(); ++k)
         delete _fecList[k];
      _fecList.swap(newfec);
   }
   for (unsigned k = 0; k < _fecList.size(); ++k){
      if (_fecList[k]->size() == 1){
         getGate((*_fecList[k])[0]/2)->setFec(0, 0);
         delete _fecList[k];
         _fecList.erase(_fecList.begin() + k);
         --k;
      }
   }
   cout << "Total #FEC Group = " << _fecList.size() << char(13) << flush;
}

void
CirMgr::makeLog(){}
/*************************************************/
/*   Private member functions about Simulation   */
/*************************************************/
