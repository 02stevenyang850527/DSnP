#include <iostream>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <vector>

using namespace std;

int main()
{
	string str;
	cout << "press something: " ;
//	cin.ignore(100,'\n');
	getline(cin, str);
	string test;
	int num;
	vector<string> store;
//	cout << str << endl;

	istringstream s(str);
	while(s >> str)
	{
		store.push_back(str);
	}

//	for (size_t i=0; i < store.size(); i++)
//		cout << store[i] << endl;
//	int t;
	cout << atoi(store[1].c_str())*3.3 ;
	return 0;
}
