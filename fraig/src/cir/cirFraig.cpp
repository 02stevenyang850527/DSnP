/****************************************************************************
  FileName     [ cirFraig.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir FRAIG functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2012-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "sat.h"
#include "myHashMap.h"
#include "util.h"

using namespace std;

// TODO: Please keep "CirMgr::strash()" and "CirMgr::fraig()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/
HashKey getKey(CirGate* temp)
{
   unsigned a = 2*(temp->get_fanin(0)->getIdNo()) + temp->input_isINV(0);
   unsigned b = 2*(temp->get_fanin(1)->getIdNo()) + temp->input_isINV(1);
   if (a < b) return HashKey(a,b);
   else       return HashKey(b,a);
}

/*******************************************/
/*   Public member functions about fraig   */
/*******************************************/
// _floatList may be changed.
// _unusedList and _undefList won't be changed
void
CirMgr::strash()
{
   IdList idtemp;
   CirGate::setGlobalRef();

   for (unsigned k = 0; k < o; ++k){
      CirGate* temp = getGate(m+k+1);
      temp->dfs4Write(idtemp);
   }

   HashMap<HashKey, CirGate*> hash(getHashSize(m));
   for (unsigned k = 0; k < idtemp.size(); ++k){
      CirGate* temp = getGate(idtemp[k]);
      HashKey ke = getKey(temp);
      if (!hash.insert(ke, temp)){
         CirGate* p;
         hash.query(ke, p);
         cout << "Strashing: " << p->getIdNo() << " merging "
              << temp->getIdNo() << "..." << endl;
         temp->recon4str((size_t)p);
         unsigned idNo = temp->getIdNo();
         for (unsigned k = 0; k < a; ++k){
            if (aig[k][0]/2 == idNo){
               aig.erase(aig.begin() + k);
               --a;
               break;
            }
         }
      }
   }
   update_fanout();
   update_aig();
}

void
CirMgr::update_fanout()
{
   for (unsigned k = 0; k < o; ++k){
      CirGate* temp = getGate(m+k+1);
      unsigned num= 2*(temp->get_fanin(0)->getIdNo());
      if (temp->input_isINV(0))
         ++num;
      output[k] = num;
   }
}

void
CirMgr::update_aig()
{
   for (unsigned k = 0; k < a; ++k){
      CirGate* temp = getGate(aig[k][0]/2);
      unsigned num1 = 2*(temp->get_fanin(0)->getIdNo());
      unsigned num2 = 2*(temp->get_fanin(1)->getIdNo());
      if (temp->input_isINV(0))  ++num1;
      if (temp->input_isINV(1))  ++num2;
      aig[k][1] = num1;
      aig[k][2] = num2;
   }
}
void
CirMgr::fraig()
{
}

/********************************************/
/*   Private member functions about fraig   */
/********************************************/
