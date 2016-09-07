#ifndef P2_TABLE_H
#define P2_TABLE_H

#include <vector>
#include <string>

using namespace std;

class Row
{
public:

	Row(int length)
	{
		numC = length;
		 _data = new int[numC]; 
	}

	size_t get_numC()
	{
		return numC;
	}

	const int operator[] (size_t i) const 
	{
		return _data[i];
	} // TODO

	int& operator[] (size_t i) 
	{
		return _data[i];
	 } // TODO

private:
	int  *_data;
	size_t numC;
};

class Table
{
public:
	Table();
	Table(int, int);
	~Table();
	void init(int, int);
	void print();
	void sum(size_t);
	void max(size_t);
	void min(size_t);
	void ave(size_t);
	void count(size_t);
	void add( vector<string> &);

	const Row& operator[] (size_t i) const
	{
		return _rows[i];
	}

	Row& operator[] (size_t i)
	{
		return _rows[i];
	}

	bool read(const string&);

private:
	vector<Row>  _rows;
	size_t numR;
};

#endif // P2_TABLE_H
