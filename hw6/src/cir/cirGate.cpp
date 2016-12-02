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
	cout << setw(46) << left << s.str() << "=\n";
	cout << "===============================================\n";	
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
		if (isINV(1,k))
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
	cout << _type << " " << _id << endl;
	unsigned n = 0;
	for ( ; n < _fanin.size(); ++n){
		if (!_fanin[n]->isGlobalRef())
			_fanin[n]->dfsFanin(level-1, recur+1,isINV(1,n));
		else{
			for (int cnt = 0; cnt < recur; ++cnt)
				cout << "  ";
			if (isINV(1,n))
					cout << "!";
			cout << _fanin[n]-> getTypeStr() << " " << _fanin[n]->getIdNo();
			if (_fanin[n]->_fanin.size() > 0 && level > 0)
			if (_fanin[n]->_fanin[0]->isGlobalRef())
				cout << " (*)";
			cout << endl;
		}
	}
	set2GlobalRef();
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
	cout << _type << " " << _id << endl;
	set2GlobalRef();
	for (unsigned k = 0; k < _fanout.size(); ++k){
		if (!_fanout[k]->isGlobalRef())
			_fanout[k]->dfsFanout(level-1, recur+1, isINV(0,k));
		else{
			for (int cnt = 0; cnt < recur; ++cnt)
				cout << "  ";
			if (isINV(0,k))
				cout << "!";
			cout << _type << " " << _id << " (*)" << endl;
		}
	}
}
