/****************************************************************************
  FileName     [ cirGate.h ]
  PackageName  [ cir ]
  Synopsis     [ Define basic gate data structures ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_GATE_H
#define CIR_GATE_H

#include <string>
#include <vector>
#include <iostream>
#include "cirDef.h"

using namespace std;

class CirGate;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
// TODO: Define your own data members and member functions, or classes
class CirGate
{
public:
   CirGate() { _ref = 0; }
   virtual ~CirGate() {}

   // Basic access methods
   string getTypeStr() const { return _type; }
   string getSymStr() const { return _symbol; }
   unsigned getLineNo() const { return _line; }
   unsigned getIdNo() const { return _id; }
	bool isINV(int i, unsigned n) const 		// i = 1 for input; i = 0 for output
	{
		if (i == 1)
			return in_inv[n];
		else
			return out_inv[n];
	}

   // Printing functions
   //virtual void printGate() const = 0;
   void reportGate() const;
   void reportFanin(int level) const;
   void reportFanout(int level) const;

	// setting function
	void set_inv(int n, int t, CirGate* p)  // n = 1 for in_inv; n = 0 for out_inv
	{											 		 // t = 0 for false; t = 1 for true
		if (n == 1){
			if (t == 1)
				in_inv.push_back(true);
			else
				in_inv.push_back(false);
			_fanin.push_back(p);
		}
		else{
			if (t == 1)
				out_inv.push_back(true);
			else
				out_inv.push_back(false);
			_fanout.push_back(p);
		}
	}

protected:
	string _type, _symbol;
	unsigned _line, _id, _ref;
	static unsigned _globalRef;
	GateList _fanin, _fanout;
	vector<bool> in_inv, out_inv;
	
};

class ConstGate: public CirGate
{
public:
	ConstGate(){
		_id = 0;
		_line = 0;
		_type = "CONST";
	}
	~ConstGate() {}
};

class UndefGate: public CirGate
{
public:
	UndefGate(unsigned i){
		_id = i;
		_line = 0;
		_type = "UNDEF";
	}
	~UndefGate() {}
};

class POGate: public CirGate
{
public:
	POGate(unsigned i, unsigned li){
		_id = i;
		_line =li;
		_type = "PO";
	}
	~POGate() {}
};

class PIGate: public CirGate
{
public:
	PIGate(unsigned i, unsigned li){
		_id = i;
		_line = li;
		_type = "PI";
	}
	~PIGate() {}
};

class AIGGate: public CirGate
{
public:
	AIGGate(unsigned i, unsigned li){
		_id = i;
		_line = li;
		_type = "AIG";
	}
	~AIGGate() {}
};


#endif // CIR_GATE_H_
