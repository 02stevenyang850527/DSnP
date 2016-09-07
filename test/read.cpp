#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
 
using namespace std;

#define NAN 99999999;

int main()
{
    ifstream data, data1;
	string csvFile;
    string line;
    string chr;
	int numRow = 1;
	int numCol = 1;
	cout << "Enter the file name\n";
	cin >> csvFile;
	data.open( csvFile.c_str());
	data1.open( csvFile.c_str());
	while (getline(data, line))
	{
		for (int i=0; i < line.length(); i++)
		{
			if (int(line[i]) == 13)	numRow++;
			if (int(line[i]) == 44)	numCol++;
		}
	}

    while( getline(data1, chr, ','))
    {	
		istringstream s(chr);
		int num;
		while(s >> num)
			cout << num << " ";
	}

	cout << endl;

	data.close();
	data1.close();
	return 0;
}     
