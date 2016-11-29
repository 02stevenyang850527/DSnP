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
	bool isINV() const {return ;}

   // Printing functions
   virtual void printGate() const = 0;
   void reportGate() const;
   void reportFanin(int level) const;
   void reportFanout(int level) const;

	// setting function
	void set_inv(unsigned n, int t)
	{
		if (t == 1)
	}

private:
	string _type, _symbol;
	unsigned _line, _id, _ref;
	static unsigned _globalRef;
	GateList _fanin, _fanout;
	vector<bool> in_inv, out_inv;
	
};

class ConstGate: public CirGate()
{
public:
	ConstGate(){
		_id = 0;
		_line = 0;
		_type = "CONST";
	}
	~ConstGate() {}
};

class UndefGate: public CirGate()
{
public:
	UndefGate(unsigned i){
		_id = i;
		_line = 0;
		_type = "UNDEF";
	}
	~UndefGate() {}
	Idlist getOut();
};

class POGate: public CirGate()
{
public:
	POGate(unsigned i, unsigned li){
		_id = i;
		_line =li;
		_type = "PO";
	}
	~POGate() {}
}

class PIGate: public CirGate()
{
public:
	PIGate(unsigned i, unsigned li){
		_id = i;
		_line = li;
		_type = "PI";
	}
	~PIGate() {}
}

class AIGGate: public CirGate()
{
public:
	AIGGate(unsigned i, unsigned li){
		_id = i;
		_line = li;
		_type = "AIG";
	}
	~AigGate() {}
};


#endif // CIR_GATE_H_
