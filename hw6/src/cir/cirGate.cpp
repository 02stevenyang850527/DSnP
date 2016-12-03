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
#include <algorithm>
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
	cout << _type << " " << _id;

	unsigned size = _fanin.size();

	if (level == 0) 
		cout << endl;	
	else if ( isGlobalRef()) 
		cout << " (*)" << endl;
	else{
		cout << endl;
		for (unsigned k = 0 ; k < size; ++k)
			_fanin[k]->dfsFanin(level-1, recur+1, isINV(1,k));
		if (size != 0)
			set2GlobalRef();
	}
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
	cout << _type << " " << _id ;
	
	unsigned size = _fanout.size();
	
	if (level == 0)
		cout << endl;
	else if ( isGlobalRef())
		cout << " (*)" << endl;
	else{
		cout << endl;
		for (unsigned k = 0; k < _fanout.size(); ++k)
				_fanout[k]->dfsFanout(level-1, recur+1, isINV(0,k));
		if (size != 0)
			set2GlobalRef();
	}
}

void
CirGate::dfs4Write(IdList& record) const
{
	if (this->_type == "UNDEF")
		return;
	for (unsigned k = 0; k < _fanin.size(); ++k){
		if (!_fanin[k]->isGlobalRef())
			_fanin[k]->dfs4Write(record);
	}
	for (unsigned k = 0; k < _fanin.size(); ++k){
		if (_fanin[k]->getTypeStr() == "AIG" && !_fanin[k]->isGlobalRef()){
			record.push_back(_fanin[k]->getIdNo());
		}
	}
	if (!isGlobalRef() && _type == "AIG")
		record.push_back(_id);
	set2GlobalRef();
}

/*
void
CirGate::reOrder_output()
{
	IdList temp;
	vector< vector<int> > recordInv;
	for (unsigned k = 0; k < _fanout.size(); ++k){
		unsigned XDD = _fanout[k]->getIdNo();
		temp.push_back(XDD);
		vector <int> tmp;
		
	}
	sort(temp.begin(), temp.end());
	_fanout.clear();
	for (unsigned k = 0; k < _temp.size(); ++k)
		_fanout.push_back(getGate(temp[k]));
	
}*/
