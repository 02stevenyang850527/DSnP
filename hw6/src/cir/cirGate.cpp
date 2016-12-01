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

extern CirMgr *cirMgr;

// TODO: Implement memeber functions for class(es) in cirGate.h

/**************************************/
/*   class CirGate member functions   */
/**************************************/
unsigned CirGate::_globalRef = 1;

void
CirGate::reportGate() const
{
	stringstream s;
	string str;
	cout << "===============================================\n";
	s << "= " << _type << "(" << _id << ")";
	if (_symbol.size() > 0)
		s << "\"" << _symbol << "\"";
	s << ", line " << _line;
	cout << setw(49) << left << s.str() << "=\n";
	cout << "===============================================\n";	
}

void
CirGate::reportFanin(int level) const
{
   assert (level >= 0);
}

void
CirGate::reportFanout(int level) const
{
   assert (level >= 0);
}

void
CirGate::dfs4NetList(int& num)
{
	if ( this-> _type == "UNDEF")
		return;
	for (unsigned k = 0; k < _fanin.size(); k++){
		if (!_fanin[k]->isGlobalRef())
			_fanin[k]->dfs4NetList(num);
	}
	set2GlobalRef();
	cout << "[" << num << "] ";
	num++;
	cout << setw(4) << left << _type << _id;
	for (unsigned k = 0; k < _fanin.size(); k++){
		cout << " ";
		if (_fanin[k]->_type == "UNDEF")
			cout << "*";
		if (isINV(1,k))
			cout << "!";
		cout << _fanin[k]->getIdNo();
	}
	if (_symbol.size() > 0)
		cout << " (" << _symbol << ")";
	cout << endl;
}

