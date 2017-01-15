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
      if (input_isINV(k))
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
         _fanin[k]->dfsFanin(level-1, recur+1, input_isINV(k));
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
         _fanout[k]->dfsFanout(level-1, recur+1, output_isINV(k));
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
CirGate::dfs4list() const
{
   if (this->_type == "UNDEF")
      return;
   for (unsigned k = 0; k < _fanin.size(); ++k){
      if (!_fanin[k]->isGlobalRef())
         _fanin[k]->dfs4list();
   }
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
            in_inv.erase(in_inv.begin() + k);
            --k;
            break;
         }
         if (_fanin[k]->_type == "PI"){
            if (_fanin[k]->_fanout[j]->getIdNo() == id){
               _fanin[k]->_fanout.erase(_fanin[k]->_fanout.begin() + j);
               _fanin[k]->out_inv.erase(_fanin[k]->out_inv.begin() + j);
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
            _fanout[k]->in_inv.erase(_fanout[k]->in_inv.begin() + j);
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
   if (_fanin[0]->getIdNo() == _fanin[1]->getIdNo()){ // for identical gate
      if (in_inv[0] == in_inv[1])
        this->recon4opt(_fanin[0], in_inv[0]);
      else  // for inverted gate
        this->recon4opt(constGate, false);
      return true;
   }
   else if (_fanin[0]->getIdNo() == 0 || _fanin[1]->getIdNo() == 0){
      if (_fanin[0]->getIdNo() == 0){
         if (in_inv[0] == false){
           this->recon4opt(constGate, false); cout << "replace 0\n";}
         else
           this->recon4opt(_fanin[1], in_inv[1]);
      }
      else{
         if (in_inv[1] == false)
            this->recon4opt(constGate, false);
         else
            this->recon4opt(_fanin[0], in_inv[0]);
      }
      return true;
   }
   else
      return false;
}

void
CirGate::recon4opt(CirGate* temp, bool inv)
{
	bool inv_origin = false;
   for (unsigned k = 0; k < _fanin.size(); ++k)
   for (unsigned j = 0; j < _fanin[k]->get_fanoutSize(); ++j){
      if (_fanin[k]->get_fanout(j)->getIdNo() == _id){
			inv_origin = _fanin[k]->output_isINV(j);
         _fanin[k]->erase_fanout(j);
         --j;
         break;
      }
   }

   for (unsigned k = 0; k < _fanout.size(); ++k){
      for (unsigned j = 0; j < _fanout[k]->_fanin.size(); ++j){
         if (_fanout[k]->_fanin[j]->getIdNo() == _id){
            _fanout[k]->_fanin[j] = temp;
            _fanout[k]->in_inv[j] = inv_origin;
            break;
         }
      }
      temp->set_output_inv(out_inv[k], _fanout[k]);
   }
   _fanout.clear();
   out_inv.clear();

   cout << "Simplifying: " << temp->getIdNo() << " merging ";
   if (inv)
      cout << "!";
   cout << _id << "..." << endl;
}


