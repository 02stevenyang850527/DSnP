/****************************************************************************
  FileName     [ cirGate.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define class CirAigGate member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdarg.h>
#include <cassert>
#include "cirGate.h"
#include "cirMgr.h"
#include "util.h"

using namespace std;

// TODO: Keep "CirGate::reportGate()", "CirGate::reportFanin()" and
//       "CirGate::reportFanout()" for cir cmds. Feel free to define
//       your own variables and functions.

extern CirMgr *cirMgr;

/**************************************/
/*   class CirGate member functions   */
/**************************************/
unsigned CirGate::_globalRef = 1;

void
CirGate::reportGate() const
{
   stringstream s;
   string str;
   cout << "==================================================\n";
   s << "= " << _type << "(" << _id << ")";
   if (_symbol.size() > 0)
      s << "\"" << _symbol << "\"";
   s << ", line " << _line;
   cout << setw(49) << left << s.str() << "=\n";
   cout << "==================================================\n";
}

void
CirGate::reportFanin(int level) const
{
   assert (level >= 0);
   setGlobalRef();
   dfsFanin(level, 0, false);
}

void
CirGate::reportFanout(int level) const
{
   assert (level >= 0);
   setGlobalRef();
   dfsFanout(level, 0, false);
}

void
CirGate::dfs4NetList(int& num) const
{
   if ( this-> _type == "UNDEF")
      return;
   for (unsigned k = 0; k < _fanin.size(); ++k){
      if (!_fanin[k]->isGlobalRef())
         _fanin[k]->dfs4NetList(num);
   }
   set2GlobalRef();
   cout << "[" << num << "] ";
   num++;
   cout << setw(4) << left << _type << _id;
   for (unsigned k = 0; k < _fanin.size(); ++k){
      cout << " ";
      if (_fanin[k]-> getTypeStr() == "UNDEF")
         cout << "*";
      if (isINV(1,k))
         cout << "!";
      cout << _fanin[k]->getIdNo();
   }
   if (_symbol.size() > 0)
      cout << " (" << _symbol << ")";
   cout << endl;
}

void
CirGate::dfsFanin(int level, int recur, bool inv) const
{
   if (level < 0)
      return;
   for (int k = 0; k < recur; ++k)
      cout << "  ";
   if (inv)
      cout << "!";
   cout << _type << " " << _id;

   unsigned size = _fanin.size();

   if (level == 0) 
      cout << endl;	
   else if (isGlobalRef()) 
      cout << " (*)" << endl;
   else{
      cout << endl;
      for (unsigned k = 0 ; k < size; ++k)
         _fanin[k]->dfsFanin(level-1, recur+1, isINV(1,k));
      if (size != 0)
         set2GlobalRef();
   }
}

void
CirGate::dfsFanout(int level, int recur, bool inv) const
{
   if (level < 0)
      return;
   for (int k = 0; k < recur; k++)
      cout << "  ";
   if (inv)
      cout << "!";
   cout << _type << " " << _id ;

   unsigned size = _fanout.size();

   if (level == 0)
      cout << endl;
   else if (isGlobalRef())
      cout << " (*)" << endl;
   else{
      cout << endl;
      for (unsigned k = 0; k < _fanout.size(); ++k)
         _fanout[k]->dfsFanout(level-1, recur+1, isINV(0,k));
      if (size != 0)
         set2GlobalRef();
   }
}

void
CirGate::dfs4Write(IdList& record) const
{
   if (this->_type == "UNDEF")
      return;
   for (unsigned k = 0; k < _fanin.size(); ++k){
      if (!_fanin[k]->isGlobalRef())
         _fanin[k]->dfs4Write(record);
   }
   for (unsigned k = 0; k < _fanin.size(); ++k){
      if (_fanin[k]->getTypeStr() == "AIG" && !_fanin[k]->isGlobalRef()){
         record.push_back(_fanin[k]->getIdNo());
      }
   }
   if (!isGlobalRef() && _type == "AIG")
      record.push_back(_id);
   set2GlobalRef();
}

void
CirGate::reconnect(unsigned id)
{
   for (unsigned k = 0; k < _fanin.size(); ++k){
      for (unsigned j = 0; j < _fanin[k]->_fanout.size(); ++j){
         if (_fanin[k]->_type == "UNDEF"){
            cout << "Sweeping: UNDEF" << "(" << _fanin[k]->getIdNo()
                 << ") removed..." << endl;
            delete _fanin[k];
            _fanin.erase(_fanin.begin() + k);
            --k;
            break;
         }
         if (_fanin[k]->_type == "PI"){
            if (_fanin[k]->_fanout[j]->getIdNo() == id){
               _fanin[k]->_fanout.erase(_fanin[k]->_fanout.begin() + j);
               --j;
               break;
            }
         }
      }
   }

   for (unsigned k = 0; k < _fanout.size(); ++k){
      for (unsigned j = 0; j < _fanout[k]->_fanin.size(); ++j){
         if (_fanout[k]->_fanin[j]->getIdNo() == id){
            _fanout[k]->_fanin.erase(_fanout[k]->_fanin.begin() + j);
            --j;
            break;
         }
      }
   }
   cout << "Sweeping: AIG" << "(" << id << ") removed..." << endl;
}
