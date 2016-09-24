#include <fstream>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include "p2Table.h"

using namespace std;

#define TRASH 99999

// Implement member functions of class Row and Table here

Table::Table(){}

Table::Table(int numRow, int numCol)
{
	numR = numRow;
	for (size_t i = 0; i < numR; i++)
		_rows.push_back( Row(numCol) );
}

Table::~Table()
{	_rows.clear();}

void Table::init(int numRow, int numCol)
{
	numR = numRow;
	for (size_t i = 0; i < numR; i++)
		_rows.push_back(Row(numCol));
}

void Table::print()
{
	for (size_t i=0; i < numR; i++)
	{	
		for (size_t j=0; j < _rows[0].get_numC(); j++)
			if (_rows[i][j] == TRASH)
				cout << right << setw(4) << " ";
			else cout << right << setw(4)<<  _rows[i][j];
		cout << endl;
	}
}

void Table::sum(size_t num)
{
	int total = 0;
	for (size_t i=0; i < numR; i++)
		if ( _rows[i][num] < 101)
			total += _rows[i][num];
	cout << "The summation of data in column #" << num << " is " << total << ".\n";
}

void Table::ave(size_t num)
{
	int total = 0;
	int count = 0;
	for (size_t i=0; i < numR; i++)
		if ( _rows[i][num] < 101 )
		{
			total += _rows[i][num];
			count ++;
		}
	cout << "The average of data in column #" << num << " is "  << fixed << setprecision(1) << double(total)/ double(count) << ".\n";
}

void Table::max(size_t num)
{
	int max = -100;
	for (size_t i=0; i < numR; i++)
		if ( _rows[i][num] > max && _rows[i][num] < 101 )
			max = _rows[i][num];
		cout << "The maximum of data in column #" << num << " is " << max << ".\n";
}

void Table::min(size_t num)
{
	int min = 101;
	for (size_t i=0; i < numR; i++)
		if ( _rows[i][num] < min && _rows[i][num] < 101 )
			min = _rows[i][num];
	cout << "The minimum of data in column #" << num << " is " << min << ".\n";
}

void Table::count(size_t num)
{
	bool count[200] = {0};
	int counter = 0;
	for (size_t i=0; i < numR; i++)
		if ( _rows[i][num] < 101 && count[ _rows[i][num] + 99] == 0 )
		{
			counter++;
			count[ _rows[i][num] + 99] = 1;
		}
	cout << "The distinct count of data in column #" << num << " is " << counter << ".\n";
}

void Table::add( vector<string> &cmd)
{
	size_t numC = _rows[0].get_numC();
	_rows.push_back( Row(numC) );
	for (size_t i=1; i < numC+1; i++)
	{
		if ( cmd[i] == "-" )
			_rows[numR][i-1] = TRASH;
		else
			_rows[numR][i-1] = atoi( cmd[i].c_str() );
	}
	numR++;
}

bool
Table::read(const string& csvFile)
{
	ifstream data;
	data.open( csvFile.c_str());

	if (!data)
		return false;
	else
		return true; // TODO
}
