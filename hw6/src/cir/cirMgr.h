/****************************************************************************
  FileName     [ cirMgr.h ]
  PackageName  [ cir ]
  Synopsis     [ Define circuit manager ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_MGR_H
#define CIR_MGR_H

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

#include "cirDef.h"

extern CirMgr *cirMgr;

// TODO: Define your own data members and member functions
class CirMgr
{
public:
   CirMgr() 
	{
		CirGate* temp = new ConstGate();
		_idList.push_back(temp);
	}
   ~CirMgr() { _idList.clear(); }

   // Access functions
   // return '0' if "gid" corresponds to an undefined gate.
   CirGate* getGate(unsigned gid) const
	{ 
		if (gid > m + o)
			return 0; 
		else
			return _idList[gid];
	}

   // Member functions about circuit construction
   bool readCircuit(const string&);

   // Member functions about circuit reporting
   void printSummary() const;
   void printNetlist() const;
   void printPIs() const;
   void printPOs() const;
   void printFloatGates() const;
   void writeAag(ostream&) const;

private:
	GateList _gList, _idList;
	unsigned m, i, l, o, a;
	vector <unsigned> input;
	vector< vector<unsigned> > output;
//	vector<unsigned> latch;  // not used in hw6
	vector< vector<unsigned> > aig;
	void linkAIG( vector<unsigned> );
	void linkPo( vector<unsigned> );
};

#endif // CIR_MGR_H
