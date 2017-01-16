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

#define NEG 0x1

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
   virtual GateType getType() const = 0;
   virtual string getTypeStr() const = 0;
   string getSymStr() const { return _symbol; }
   unsigned getLineNo() const { return _line; }
   unsigned getIdNo() const { return _id; }
   virtual bool isAig() const { return (getType() == AIG_GATE); }
   bool input_isINV(size_t n) const
   { return (((size_t)_fanin[n]) & NEG); }
	bool output_isINV(size_t n) const
   { return (((size_t)_fanout[n]) & NEG); }

   size_t get_faninSize() { return _fanin.size(); }
   size_t get_fanoutSize() { return _fanout.size(); }
   CirGate* get_fanin(size_t i) const { return (CirGate*)(((size_t)_fanin[i]) & ~size_t(NEG)); }
   CirGate* get_fanout(size_t i) const { return (CirGate*)(((size_t)_fanout[i]) & ~size_t(NEG)); }
   size_t get_faninWithInv(size_t i) const { return (size_t)_fanin[i]; }

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
   void dfs4list() const;

   // Setting function
   void set_input_inv( bool isINV, CirGate* p)
   {	
      if (isINV)
         p = (CirGate*)((size_t)p + 1);
      _fanin.push_back(p);
   }
   void reset_fanin
   (size_t i, size_t input){ _fanin[i] = (CirGate*)input; }

   void erase_fanout(size_t k)
   {
     _fanout.erase(_fanout.begin() + k);
   }

   void set_output_inv( bool isINV, CirGate* p)
   {	
      if (isINV)
         p = (CirGate*)((size_t)p + 1);
      _fanout.push_back(p);
   }
   void setSymbol(string& s) { _symbol = s; }
   void reconnect(unsigned); // for sweeping
   bool simplify(CirGate*); // for optimize
   void recon4opt(size_t);
   void recon4str(size_t);
   static void setGlobalRef() { _globalRef++; }
   bool isGlobalRef() const { return (_ref == _globalRef); }

private:

protected:
   string _symbol;
   unsigned _line, _id;
   static unsigned _globalRef;
   mutable unsigned _ref;
   GateList _fanin, _fanout;
   void set2GlobalRef() const { _ref = _globalRef; }
};

class ConstGate: public CirGate
{
public:
   ConstGate(){
      _id = 0;
      _line = 0;
   }
   virtual GateType getType() const { return CONST_GATE; }
   virtual string getTypeStr() const {return "CONST"; }
	~ConstGate() {}
};

class UndefGate: public CirGate
{
public:
   UndefGate(unsigned i){
      _id = i;
      _line = 0;
   }
   virtual GateType getType() const { return UNDEF_GATE; }
   virtual string getTypeStr() const {return "UNDEF"; }
	~UndefGate() {}
};

class POGate: public CirGate
{
public:
   POGate(unsigned i, unsigned li){
      _id = i;
      _line =li;
   }
   virtual GateType getType() const { return PO_GATE; }
   virtual string getTypeStr() const {return "PO"; }
	~POGate() {}
};

class PIGate: public CirGate
{
public:
   PIGate(unsigned i, unsigned li){
      _id = i;
      _line = li;
   }
   virtual GateType getType() const { return PI_GATE; }
   virtual string getTypeStr() const {return "PI"; }
	~PIGate() {}
};

class AIGGate: public CirGate
{
public:
   AIGGate(unsigned i, unsigned li){
      _id = i;
      _line = li;
   }
   virtual GateType getType() const { return AIG_GATE; }
   virtual string getTypeStr() const {return "AIG"; }
   ~AIGGate() {}
};

#endif // CIR_GATE_H
