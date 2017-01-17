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
   CirGate(): _value(0), _ref(0), _fec(0), isSim(false) {}
   virtual ~CirGate() {}

   // Basic access methods
   virtual GateType getType() const = 0;
   virtual string getTypeStr() const = 0;
   string getSymStr() const { return _symbol; }
   unsigned getLineNo() const { return _line; }
   unsigned getIdNo() const { return _id; }
   unsigned getValue() const { return _value; }
   virtual bool isAig() const { return false; }
   bool input_isINV(size_t n) const
   { return (((size_t)_fanin[n]) & NEG); }
	bool output_isINV(size_t n) const
   { return (((size_t)_fanout[n]) & NEG); }

   size_t get_faninSize() { return _fanin.size(); }
   size_t get_fanoutSize() { return _fanout.size(); }
   CirGate* get_fanin(size_t i) const { return (CirGate*)(((size_t)_fanin[i]) & ~size_t(NEG)); }
   CirGate* get_fanout(size_t i) const { return (CirGate*)(((size_t)_fanout[i]) & ~size_t(NEG)); }
   size_t get_faninWithInv(size_t i) const { return (size_t)_fanin[i]; }
   virtual bool simulate() { return isSim; }

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
   void dfs4WriteGate(IdList& ) const;
   void dfs4list() const;

   // Setting function
   void set_input_inv( bool isINV, CirGate* p)
   {	
      if (isINV)
         p = (CirGate*)((size_t)p + 1);
      _fanin.push_back(p);
   }
   void reset_fanin(size_t i, size_t input) { _fanin[i] = (CirGate*)input; }
   void erase_fanout(size_t k) { _fanout.erase(_fanout.begin() + k); }
   void set_output_inv( bool isINV, CirGate* p)
   {	
      if (isINV)
         p = (CirGate*)((size_t)p + 1);
      _fanout.push_back(p);
   }
   void setSymbol(string& s) { _symbol = s; }
   virtual void setValue(unsigned n) {} // only for PIGate
   virtual void initValue() {} // only for AIGGate
   void setFec(IdList* i, unsigned id) {_fec = i; _fecId = id; }

   void reconnect(unsigned); // for sweeping
   bool simplify(CirGate*); // for optimize
   void recon4opt(size_t);
   void recon4str(size_t);
   static void setGlobalRef() { _globalRef++; }
   bool isGlobalRef() const { return (_ref == _globalRef); }

private:

protected:
   string _symbol;
   unsigned _line, _id, _fecId, _value;
   static unsigned _globalRef;
   mutable unsigned _ref;
   IdList* _fec;
   mutable bool isSim;
   GateList _fanin, _fanout;
   void set2GlobalRef() const { _ref = _globalRef; }
};

class ConstGate: public CirGate
{
public:
   ConstGate(){
      _id = 0;
      _line = 0;
      _value = 0;
   }
	~ConstGate() {}
   virtual GateType getType() const { return CONST_GATE; }
   virtual string getTypeStr() const {return "CONST"; }
};

class UndefGate: public CirGate
{
public:
   UndefGate(unsigned i){
      _id = i;
      _line = 0;
   }
	~UndefGate() {}
   virtual GateType getType() const { return UNDEF_GATE; }
   virtual string getTypeStr() const {return "UNDEF"; }
};

class POGate: public CirGate
{
public:
   POGate(unsigned i, unsigned li){
      _id = i;
      _line =li;
   }
	~POGate() {}
   virtual GateType getType() const { return PO_GATE; }
   virtual string getTypeStr() const {return "PO"; }
   virtual bool simulate();
};

class PIGate: public CirGate
{
public:
   PIGate(unsigned i, unsigned li){
      _id = i;
      _line = li;
   }
	~PIGate() {}
   virtual GateType getType() const { return PI_GATE; }
   virtual string getTypeStr() const {return "PI"; }
   virtual void setValue(unsigned n) { _value = n; isSim = true; set2GlobalRef(); }
};

class AIGGate: public CirGate
{
public:
   AIGGate(unsigned i, unsigned li){
      _id = i;
      _line = li;
   }
   ~AIGGate() {}
   virtual GateType getType() const { return AIG_GATE; }
   virtual string getTypeStr() const {return "AIG"; }
   virtual bool isAIG() const { return true; }
   virtual bool simulate();
   virtual void initValue()
   { _value = (get_fanin(0)->getValue() ^ (0 - input_isINV(0))) &( get_fanin(1)->getValue() ^ (0 - input_isINV(1))); }
};

#endif // CIR_GATE_H
