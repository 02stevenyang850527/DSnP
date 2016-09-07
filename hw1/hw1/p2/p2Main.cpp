#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <math.h>
#include "p2Table.h"

using namespace std;

#define TRASH 99999

int main()
{
   Table table;

   // TODO: read in the csv file
	ifstream data;
	string csvFile;
	string line, line1;
	string chr;
	size_t numCol = 1;
	size_t numRow = 0;
	vector<int> temp;
	vector<int> temp1;
	
	cout << "Please enter the file name: ";
	cin >> csvFile;

	if (table.read(csvFile))
		cout << "File \"" << csvFile << "\" was read in successfully." << endl;
	else exit(-1); // csvFile does not exist.

	data.open( csvFile.c_str());
	while( getline(data,line))
	{
		for (size_t i=0; i < line.length()-1; i++)
		{	
			if ( int(line[i]) == 13)	
				numRow++;
			if ( int(line[i]) == 44 && numRow == 1)	numCol++;
		}
		line1.assign(line);
	}

	table.init(numRow, numCol);
	data.clear();
	data.seekg(0, ios::beg);

	while( getline(data, chr, ','))
	{
		istringstream s(chr);
		int num;
		while (s >> num)
			temp.push_back(num);	
	}

	size_t countL = 0;
	size_t countT = 0;
	temp1.clear();
	while ( countL < line1.length()-2 )
	{
		if (line1[countL] == 44 && countL == 0)
				{ temp1.push_back(TRASH); countL++; }
		else if ( int(line1[countL]) == 44 && int(line1[countL+1]) == 44)
				{ temp1.push_back(TRASH); countL++; }
		else if ( int(line1[countL]) == 44 && int(line1[countL+1]) == 13)
				{ temp1.push_back(TRASH); countL++; }
		else if ( int(line1[countL]) == 13 && int(line1[countL+1]) == 44)
				{ temp1.push_back(TRASH); countL++; }
		else if ( int(line1[countL]) == 13 && int(line1[countL+1]) == 13)
				{	for (size_t i=0; i < numCol; i++)
						temp1.push_back(TRASH);
					countL++;
				}
		else if ( int(line1[countL]) > 47 && int(line1[countL+1]) < 48 )
				{	if (countT < temp.size())
					{	temp1.push_back(temp[countT]);
						countT++;
					}
					countL++;
				}
		else
			 	countL++;
	}

	for (size_t i=0; i < numRow; i++)
	for (size_t j=0; j < numCol; j++)
	{	table[i][j] = temp1[ i*numCol + j];}

	string Input, str;
	vector <string> command;
	cin.ignore(1000, '\n');
   // TODO read and execute commands
	while (true) {
		cin.clear();
		str.clear();
		Input.clear();
		getline(cin, Input);
		istringstream s(Input);
		command.clear();
		while(s >> str)
		{	command.push_back(str); }

		if (command[0] == "PRINT")
			table.print();
		else if (command[0] == "SUM")
			table.sum( atoi(command[1].c_str()) );
		else if (command[0] == "AVE")
			table.ave( atoi(command[1].c_str()) );
		else if (command[0] == "MAX")
			table.max( atoi(command[1].c_str()) );
		else if (command[0] == "MIN")
			table.min( atoi(command[1].c_str()) );
		else if (command[0] == "COUNT")
			table.count( atoi(command[1].c_str()));
		else if (command[0] == "ADD")
			table.add( command );
		else if (command[0] == "EXIT")
			 break;
		else 
			cout << "Error Command ! Try Again\n";   
	}

	data.close();
	return 0;
}
