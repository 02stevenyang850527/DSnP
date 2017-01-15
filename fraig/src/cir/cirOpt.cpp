/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir optimization functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include <vector>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Please keep "CirMgr::sweep()" and "CirMgr::optimize()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/**************************************************/
/*   Public member functions about optimization   */
/**************************************************/
// Remove unused gates
// DFS list should NOT be changed
// UNDEF, float and unused list may be changed
void
CirMgr::sweep()
{
   CirGate::setGlobalRef();
   for (unsigned k = 0; k < o; ++k){
      CirGate* temp = getGate(m+k+1);
      temp->dfs4list();
   }
   CirGate* temp;
   for (unsigned k = 0; k < a; ++k){
      temp = getGate(aig[k][0]/2);
      if (!(temp->isGlobalRef())){
         temp->reconnect(aig[k][0]/2);
         delete temp;
         aig.erase(aig.begin() + k);
         --a;
         --k;
      }
   }
}

// Recursively simplifying from POs;
// _dfsList needs to be reconstructed afterwards
// UNDEF gates may be delete if its fanout becomes empty...
void
CirMgr::optimize()
{
   CirGate* constGate = getGate(0);
   CirGate::setGlobalRef();
   IdList idtemp;
   for (unsigned k = 0; k < o; ++k){
      CirGate* temp = getGate(m+k+1);
      temp->dfs4Write(idtemp);
   }
   CirGate* temp;
   for (unsigned k = 0; k < idtemp.size(); ++k){
      temp = getGate(idtemp[k]);
      if (temp->simplify(constGate)){
         for (unsigned j = 0; j < a; ++j){
            if (aig[j][0]/2 == idtemp[k])
               aig.erase(aig.begin() + j);
               --a; --j;
               delete temp;
               break;
         }
      }
   }
}

/***************************************************/
/*   Private member functions about optimization   */
/***************************************************/
