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
   s << "= " << this->getTypeStr() << "(" << _id << ")";
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
   if ( this-> getType() == UNDEF_GATE)
      return;
   for (unsigned k = 0; k < _fanin.size(); ++k){
      if (!get_fanin(k)->isGlobalRef())
         get_fanin(k)->dfs4NetList(num);
   }
   set2GlobalRef();
   cout << "[" << num << "] ";
   num++;
   cout << setw(4) << left << getTypeStr() << _id;
   for (unsigned k = 0; k < _fanin.size(); ++k){
      cout << " ";
      if (get_fanin(k)-> getType() == UNDEF_GATE)
         cout << "*";
      if (input_isINV(k))
         cout << "!";
      cout << get_fanin(k)->getIdNo();
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
   cout << getTypeStr() << " " << _id;

   unsigned size = _fanin.size();

   if (level == 0) 
      cout << endl;	
   else if (isGlobalRef()) 
      cout << " (*)" << endl;
   else{
      cout << endl;
      for (unsigned k = 0 ; k < size; ++k)
         get_fanin(k)->dfsFanin(level-1, recur+1, input_isINV(k));
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
   cout << getTypeStr() << " " << _id ;

   unsigned size = _fanout.size();

   if (level == 0)
      cout << endl;
   else if (isGlobalRef())
      cout << " (*)" << endl;
   else{
      cout << endl;
      for (unsigned k = 0; k < _fanout.size(); ++k)
         get_fanout(k)->dfsFanout(level-1, recur+1, output_isINV(k));
      if (size != 0)
         set2GlobalRef();
   }
}

void
CirGate::dfs4Write(IdList& record) const
{
   if (this->getType() == UNDEF_GATE)
      return;
   for (unsigned k = 0; k < _fanin.size(); ++k){
      if (!get_fanin(k)->isGlobalRef())
         get_fanin(k)->dfs4Write(record);
   }
   for (unsigned k = 0; k < _fanin.size(); ++k){
      if (get_fanin(k)->getType() == AIG_GATE && !get_fanin(k)->isGlobalRef()){
         record.push_back(get_fanin(k)->getIdNo());
      }
   }
   if (!isGlobalRef() && getType() == AIG_GATE)
      record.push_back(_id);
   set2GlobalRef();
}

void
CirGate::dfs4list() const
{
   if (this->getType() == UNDEF_GATE)
      return;
   for (unsigned k = 0; k < _fanin.size(); ++k){
      if (!get_fanin(k)->isGlobalRef())
         get_fanin(k)->dfs4list();
   }
   set2GlobalRef();
}

void
CirGate::reconnect(unsigned id)
{
   for (unsigned k = 0; k < _fanin.size(); ++k){
      for (unsigned j = 0; j < get_fanin(k)->_fanout.size(); ++j){
         if (get_fanin(k)->getType() == UNDEF_GATE){
            cout << "Sweeping: UNDEF" << "(" << get_fanin(k)->getIdNo()
                 << ") removed..." << endl;
            delete get_fanin(k);
            _fanin.erase(_fanin.begin() + k);
            --k;
            break;
         }
         if (get_fanin(k)->getType() == PI_GATE){
            if (get_fanin(k)->get_fanout(j)->getIdNo() == id){
               get_fanin(k)->_fanout.erase(get_fanin(k)->_fanout.begin() + j);
               --j;
               break;
            }
         }
      }
   }

   for (unsigned k = 0; k < _fanout.size(); ++k){
      for (unsigned j = 0; j < get_fanout(k)->_fanin.size(); ++j){
         if (get_fanout(k)->get_fanin(j)->getIdNo() == id){
            get_fanout(k)->_fanin.erase(get_fanout(k)->_fanin.begin() + j);
            --j;
            break;
         }
      }
   }
   cout << "Sweeping: AIG" << "(" << id << ") removed..." << endl;
}

bool
CirGate::simplify(CirGate* constGate)
{
   if (get_fanin(0)->getIdNo() == get_fanin(1)->getIdNo()){ // for identical gate
      if (input_isINV(0) == input_isINV(1))
        this->recon4opt(get_faninWithInv(0));
      else  // for inverted gate
        this->recon4opt((size_t)constGate);
      return true;
   }
   else if (get_fanin(0)->getIdNo() == 0 || get_fanin(1)->getIdNo() == 0){
      if (get_fanin(0)->getIdNo() == 0){
         if (input_isINV(0) == false)
           this->recon4opt((size_t)constGate);
         else
           this->recon4opt(get_faninWithInv(1));
      }
      else{
         if (input_isINV(1) == false)
            this->recon4opt((size_t)constGate);
         else
            this->recon4opt(get_faninWithInv(0));
      }
      return true;
   }
   else
      return false;
}

void
CirGate::recon4opt(size_t input_with_INV)
{
   CirGate* temp = (CirGate*)(input_with_INV & ~size_t(NEG));
   for (unsigned k = 0; k < _fanin.size(); ++k)
   for (unsigned j = 0; j < get_fanin(k)->get_fanoutSize(); ++j){
      if (get_fanin(k)->get_fanout(j)->getIdNo() == _id){
         get_fanin(k)->erase_fanout(j);
         --j;
      }
   }

   for (unsigned k = 0; k < _fanout.size(); ++k){
      for (unsigned j = 0; j < get_fanout(k)->_fanin.size(); ++j){
         if (get_fanout(k)->get_fanin(j)->getIdNo() == _id){
            if (get_fanout(k)->input_isINV(j)){
               if (!(input_with_INV & NEG))
                  get_fanout(k)->reset_fanin(j, input_with_INV + 1);
               else
                  get_fanout(k)->reset_fanin(j, input_with_INV - 1);
            }
            else
               get_fanout(k)->reset_fanin(j, input_with_INV);
         }
      }
      temp->set_output_inv(output_isINV(k), get_fanout(k));
   }
   _fanout.clear();

   cout << "Simplifying: " << temp->getIdNo() << " merging ";
   if (input_with_INV & NEG)
      cout << "!";
   cout << _id << "..." << endl;
}


void
CirGate::recon4str(size_t input_with_INV)
{
   CirGate* temp = (CirGate*)(input_with_INV & ~size_t(NEG));
   for (unsigned k = 0; k < _fanin.size(); ++k)
   for (unsigned j = 0; j < get_fanin(k)->get_fanoutSize(); ++j){
      if (get_fanin(k)->get_fanout(j)->getIdNo() == _id){
         get_fanin(k)->erase_fanout(j);
         --j;
      }
   }

   for (unsigned k = 0; k < _fanout.size(); ++k){
      for (unsigned j = 0; j < get_fanout(k)->_fanin.size(); ++j){
         if (get_fanout(k)->get_fanin(j)->getIdNo() == _id){
            if (get_fanout(k)->input_isINV(j)){
               if (!(input_with_INV & NEG))
                  get_fanout(k)->reset_fanin(j, input_with_INV + 1);
               else
                  get_fanout(k)->reset_fanin(j, input_with_INV - 1);
            }
            else
               get_fanout(k)->reset_fanin(j, input_with_INV);
         }
      }
      temp->set_output_inv(output_isINV(k), get_fanout(k));
   }
   _fanout.clear();
}

bool
POGate::simulate()
{
   isSim = get_fanin(0)->simulate();
   if (isSim)
      _value = get_fanin(0)->getValue() ^ (0 - input_isINV(0));
   return isSim;
}

bool
AIGGate::simulate()
{
   if (isGlobalRef())
      return isSim;
   set2GlobalRef();
   isSim = get_fanin(0)->simulate() + get_fanin(1)->simulate();
   if (isSim){
      unsigned val;
      val = (get_fanin(0)->getValue() ^ (0 - input_isINV(0))) & (get_fanin(0)->getValue() ^ (0 - input_isINV(0)));
      if (_value == val) isSim = false;
      _value = val;
   }
   return isSim;
}

