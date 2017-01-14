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
#include "sat.h"

using namespace std;

// TODO: Feel free to define your own classes, variables, or functions.

class CirGate;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
class CirGate
{
public:
   CirGate(){ _ref = 0;}
   virtual ~CirGate() {}

   // Basic access methods
   string getTypeStr() const { return _type; }
   string getSymStr() const { return _symbol; }
   unsigned getLineNo() const { return _line; }
   unsigned getIdNo() const { return _id; }
   virtual bool isAig() const { return (_type == "AIG"); }
   bool isINV(int i, unsigned n) const 		// i = 1 for input; i = 0 for output
   {
      if (i == 1)
         return in_inv[n];
      else
         return out_inv[n];
	}

   size_t get_faninSize() { return _fanin.size(); }
   size_t get_fanoutSize() { return _fanout.size(); }
   CirGate* get_fanin(unsigned i) { return _fanin[i]; }
   CirGate* get_fanout(unsigned i) { return _fanout[i]; }

   // Printing functions
  // virtual void printGate() const {}
   void reportGate() const;
   void reportFanin(int level) const;
   void reportFanout(int level) const;

	// Depth First Search function
   void dfs4NetList(int&) const;
   void dfsFanin(int, int, bool) const;
   void dfsFanout(int, int, bool) const;
   void dfs4Write(IdList& ) const;

   // Setting function
   void set_input_inv( bool isINV, CirGate* p)
   {	
      if (isINV)
         in_inv.push_back(true);
      else
         in_inv.push_back(false);
      _fanin.push_back(p);
   
   }

   void set_output_inv( bool isINV, CirGate* p)
   {	
      if (isINV)
         out_inv.push_back(true);
      else
         out_inv.push_back(false);
      _fanout.push_back(p);
   }
   void setSymbol(string& s) { _symbol = s; }
   void reconnect(unsigned id); // for sweeping
   static void setGlobalRef() { _globalRef++; }

private:

protected:
   string _type, _symbol;
   unsigned _line, _id;
   static unsigned _globalRef;
   mutable unsigned _ref;
   GateList _fanin, _fanout;
   vector<bool> in_inv, out_inv;
   void set2GlobalRef() const { _ref = _globalRef; }
   bool isGlobalRef() const { return (_ref == _globalRef); }
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

#endif // CIR_GATE_H
