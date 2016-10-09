/****************************************************************************
  FileName     [ dbTable.cpp ]
  PackageName  [ db ]
  Synopsis     [ Define database Table member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2015-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iomanip>
#include <string>
#include <cctype>
#include <cassert>
#include <sstream>
#include <set>
#include <algorithm>
#include "dbTable.h"

using namespace std;

/*****************************************/
/*          Global Functions             */
/*****************************************/
ostream& operator << (ostream& os, const DBRow& r)
{
   // TODO: to print out a row.
   // - Data are seperated by a space. No trailing space at the end.
   // - Null cells are printed as '.'
	for (size_t i=0; i < r.size()-1; i++){
		if (r[i] == INT_MAX)
			os << '.' << ' ';
		else
			os << r[i] << ' ';
	}

	if (r[r.size()-1] == INT_MAX)
		os << '.' << ' ';
	else
		os << r[r.size()-1] << ' ';

   return os;
}

ostream& operator << (ostream& os, const DBTable& t)
{
   // TODO: to print out a table
   // - Data are seperated by setw(6) and aligned right.
   // - Null cells are printed as '.'
	for ( size_t i=0; i < t.nRows(); i++){
		for ( size_t j=0; j < t[i].size(); j++)
			if (t[i][j] == INT_MAX)
				os << right << setw(6) << '.' << ' ';
			else
				os << right << setw(6) <<  t[i][j] << ' ';
		os << endl;
	}
/*	for (int j=0; j < t[t.nRows()-1].size(); j++)
		if (t[t.nRows()-1][j] == INT_MAX)
			os << right << setw(6) << '.' << ' ';
		else
			os << right << setw(6) <<  t[t.nRows()-1][j] << ' ';*/
   return os;
}

ifstream& operator >> (ifstream& ifs, DBTable& t)
{
   // TODO: to read in data from csv file and store them in a table
   // - You can assume all the data of the table are in a single line.
	string line;
	int count = 0;
	DBRow temp;
	vector<string> result;
	getline(ifs, line);
	size_t size  = line.length();
	for (size_t i = 0; i < size; i++)
	{
		size_t pos = line.find(char(13),i);
		if (pos < size)
		{
			string s = line.substr(i,pos-i);
			result.push_back(s);
			i = pos;
		}
	}

	result.pop_back();

	for (size_t i=0; i < result.size(); i++){
		istringstream s(result[i]);
		int num;
		bool first = true;
		bool last = false;
		while (s >> num || !s.eof()){
			if (s.fail()){
				s.clear();
				char dummy;
				s >> dummy;
				last = true;
				if (int(dummy) == 44 && first == true)
					temp.addData(INT_MAX);
				if (int(dummy) == 44)
					count++;
				if (count >= 2)
					temp.addData(INT_MAX);
				continue;
			}
			temp.addData(num);
			first = false; last =false;
			count = 0;
		}
		if (last == true)
			temp.addData(INT_MAX);
		t.addRow(temp); temp.reset();
	}
	return ifs;
}

/*****************************************/
/*   Member Functions for class DBRow    */
/*****************************************/
void
DBRow::removeCell(size_t c)
{
   // TODO
	_data.erase(_data.begin() + c);
}

/*****************************************/
/*   Member Functions for struct DBSort  */
/*****************************************/
bool
DBSort::operator() (const DBRow& r1, const DBRow& r2) const
{
   // TODO: called as a functional object that compares the data in r1 and r2
   //       based on the order defined in _sortOrder
	
   return false;
}

/*****************************************/
/*   Member Functions for class DBTable  */
/*****************************************/
void
DBTable::reset()
{
   // TODO
	_table.clear();
}

void
DBTable::addCol(const vector<int>& d)
{
   // TODO: add a column to the right of the table. Data are in 'd'.
	for (size_t i=0; i < _table.size(); i++)
		_table[i].addData(d[i]);
}

void
DBTable::delRow(int c)
{
   // TODO: delete row #c. Note #0 is the first row.
	_table.erase(_table.begin() + c);
}

void
DBTable::delCol(int c)
{
   // delete col #c. Note #0 is the first row.
   for (size_t i = 0, n = _table.size(); i < n; ++i)
      _table[i].removeCell(c);
}

// For the following getXXX() functions...  (except for getCount())
// - Ignore null cells
// - If all the cells in column #c are null, return NAN
// - Return "float" because NAN is a float.
float
DBTable::getMax(size_t c) const
{
   // TODO: get the max data in column #c
	float max = INT_MIN ;
	bool empty = true;
	for (size_t i=0; i < _table.size(); i++)
		if (_table[i][c] > max && _table[i][c] != INT_MAX)
		{	max = _table[i][c]; empty = false;}

	if (empty == false)
		return max;
	else 
		return NAN;
}

float
DBTable::getMin(size_t c) const
{
   // TODO: get the min data in column #c
	float min = INT_MAX - 1;
	bool empty = true;
	for (size_t i=0; i < _table.size(); i++)
		if (_table[i][c] < min && _table[i][c] != INT_MAX)
		{	min = _table[i][c]; empty = false;}

	if (empty == false)
		return min;
	else 
		return NAN;
}

float 
DBTable::getSum(size_t c) const
{
   // TODO: compute the sum of data in column #c
	float sum = 0;
	bool empty = true;
	for (size_t i=0; i < _table.size(); i++)
		if (_table[i][c] != INT_MAX )
		{	sum += _table[i][c]; empty = false;}

	if (empty == false)
		return sum;
	else 
		return NAN;
}

int
DBTable::getCount(size_t c) const
{
   // TODO: compute the number of distinct data in column #c
   // - Ignore null cells
	int count = 0;
	bool repeat = false;
	for (size_t i=0; i < _table.size(); i++){
		if (_table[i][c] != INT_MAX){
			int temp = _table[i][c];
			repeat = false;
			for (size_t j=0; j < i; j++)
				if (_table[j][c] == temp)
					repeat = true;
			if (repeat == false)
				count++;
		}
	}
	return count;
}

float
DBTable::getAve(size_t c) const
{
   // TODO: compute the average of data in column #c
	float sum = 0;
	float count = 0 ;
	bool empty = true;
	for (size_t i=0; i < _table.size(); i++)
		if (_table[i][c] != INT_MAX )
		{	sum += _table[i][c]; empty = false; count++;}

	if (empty == false)
		return sum/count;
	else 
		return NAN;
}

void
DBTable::sort(const struct DBSort& s)
{
   // TODO: sort the data according to the order of columns in 's'
}

void
DBTable::printCol(size_t c) const
{
   // TODO: to print out a column.
   // - Data are seperated by a space. No trailing space at the end.
   // - Null cells are printed as '.'
	for (size_t i=0; i < _table.size()-1; i++)
		if (_table[i][c] == INT_MAX)
			cout << '.' << ' ';
		else
			cout << _table[i][c] << ' ';

	if (_table[_table.size()-1][c] == INT_MAX)
		cout << '.' << ' ';
	else
		cout << _table[_table.size()-1][c] << ' ';
}

void
DBTable::printSummary() const
{
   size_t nr = nRows(), nc = nCols(), nv = 0;
   for (size_t i = 0; i < nr; ++i)
      for (size_t j = 0; j < nc; ++j)
         if (_table[i][j] != INT_MAX) ++nv;
   cout << "(#rows, #cols, #data) = (" << nr << ", " << nc << ", "
        << nv << ")" << endl;
}

