/****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <cstdio>
#include <ctype.h>
#include <cassert>
#include <cstring>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Implement memeber functions for class CirMgr

/*******************************/
/*   Global variable and enum  */
/*******************************/
CirMgr* cirMgr = 0;

enum CirParseError {
   EXTRA_SPACE,
   MISSING_SPACE,
   ILLEGAL_WSPACE,
   ILLEGAL_NUM,
   ILLEGAL_IDENTIFIER,
   ILLEGAL_SYMBOL_TYPE,
   ILLEGAL_SYMBOL_NAME,
   MISSING_NUM,
   MISSING_IDENTIFIER,
   MISSING_NEWLINE,
   MISSING_DEF,
   CANNOT_INVERTED,
   MAX_LIT_ID,
   REDEF_GATE,
   REDEF_SYMBOLIC_NAME,
   REDEF_CONST,
   NUM_TOO_SMALL,
   NUM_TOO_BIG,

   DUMMY_END
};

/**************************************/
/*   Static varaibles and functions   */
/**************************************/
static unsigned lineNo = 0;  // in printint, lineNo needs to ++
static unsigned colNo  = 0;  // in printing, colNo needs to ++
static char buf[1024];
static string errMsg;
static int errInt;
static CirGate *errGate;

static bool
parseError(CirParseError err)
{
   switch (err) {
      case EXTRA_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Extra space character is detected!!" << endl;
         break;
      case MISSING_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing space character!!" << endl;
         break;
      case ILLEGAL_WSPACE: // for non-space white space character
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal white space char(" << errInt
              << ") is detected!!" << endl;
         break;
      case ILLEGAL_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal "
              << errMsg << "!!" << endl;
         break;
      case ILLEGAL_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal identifier \""
              << errMsg << "\"!!" << endl;
         break;
      case ILLEGAL_SYMBOL_TYPE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal symbol type (" << errMsg << ")!!" << endl;
         break;
      case ILLEGAL_SYMBOL_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Symbolic name contains un-printable char(" << errInt
              << ")!!" << endl;
         break;
      case MISSING_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing " << errMsg << "!!" << endl;
         break;
      case MISSING_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing \""
              << errMsg << "\"!!" << endl;
         break;
      case MISSING_NEWLINE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": A new line is expected here!!" << endl;
         break;
      case MISSING_DEF:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing " << errMsg
              << " definition!!" << endl;
         break;
      case CANNOT_INVERTED:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": " << errMsg << " " << errInt << "(" << errInt/2
              << ") cannot be inverted!!" << endl;
         break;
      case MAX_LIT_ID:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Literal \"" << errInt << "\" exceeds maximum valid ID!!"
              << endl;
         break;
      case REDEF_GATE:
         cerr << "[ERROR] Line " << lineNo+1 << ": Literal \"" << errInt
              << "\" is redefined, previously defined as "
              << errGate->getTypeStr() << " in line " << errGate->getLineNo()
              << "!!" << endl;
         break;
      case REDEF_SYMBOLIC_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ": Symbolic name for \""
              << errMsg << errInt << "\" is redefined!!" << endl;
         break;
      case REDEF_CONST:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Cannot redefine const (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_SMALL:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too small (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_BIG:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too big (" << errInt << ")!!" << endl;
         break;
      default: break;
   }
   return false;
}

/**************************************************************/
/*   class CirMgr member functions for circuit construction   */
/**************************************************************/

CirMgr::CirMgr() { _idList.push_back(new ConstGate()); }
CirMgr::~CirMgr() {
   for (unsigned i = 0; i < _idList.size(); ++i)
      delete _idList[i];
   output.clear();
   input.clear();
   aig.clear();
}

bool
CirMgr::readCircuit(const string& fileName)
{
   ifstream data(fileName.c_str(), ios::in);
   if (!data.is_open()){
      cerr << "Cannot open design \"" << fileName << "\"!!" << endl;
      return false;
   }

   string line, temp;
   getline(data, line);

   istringstream s(line);
   while (s >> temp){
      if (temp == "aag")
         s >> m >> i >> l >> o >> a;
      else
         return false;
   }
   ++lineNo;
   _idList.resize(m + o + 1, 0); //_idlist[i] get the Cirgate of _id == i

    for (unsigned k = 0; k < i; ++k){
      getline(data, line);
      istringstream ss(line);
      unsigned xdd;
      ss >> xdd;
      ++lineNo;
      input.push_back(xdd);
      _idList[xdd/2] = new PIGate(xdd/2, lineNo);
      }

   for (unsigned k = 0; k < l; ++k){    // should not enter the loop in hw6
      cout << "something go wrong!/n";
      ++lineNo;
   }

   for (unsigned k = 0; k < o; ++k){
      getline(data, line);
      istringstream ss(line);
      unsigned xdd;
      ss >> xdd;
      ++lineNo;
      output.push_back(xdd);
      _idList[m+1+k] = new POGate(m+1+k, lineNo);
   }

   _idList.resize(m + o, 0);
   for (unsigned k = 0; k < a; ++k){
      getline(data, line);
      istringstream ss(line);
      unsigned x,y,z;
      ss >> x >> y >> z;
      vector <unsigned> temp;
      temp.push_back(x); temp.push_back(y); temp.push_back(z);
      aig.push_back(temp);
      ++lineNo;
      _idList[x/2] = new AIGGate(x/2, lineNo);
   }

   for (unsigned k = 0; k < a; ++k)
      linkAIG(aig[k]);
   for (unsigned k = 0; k < o; ++k)
      linkPO(output[k],k);

   while (getline(data, line)){
      if (line.empty())
         break;
      if (line[0] == 'c')
         break;
      string tok, sub;
      size_t s = myStrGetTok(line, tok);
      if (s == string::npos)
         return false;
      else
         sub = line.substr(s+1);

      if (tok[0] == 'i'){
         tok.erase(tok.begin());
         int num;
         myStr2Int(tok, num);
         getGate(input[num]/2)->setSymbol(sub);
      }

      if (tok[0] == 'o'){
         tok.erase(tok.begin());
         int num;
         myStr2Int(tok, num);
         getGate(num+m+1)->setSymbol(sub);
      }
   }
   data.close();
   return true;
}
void
CirMgr::linkAIG(vector <unsigned>& aigInfo)
{
   CirGate* temp = getGate(aigInfo[0]/2);
   CirGate* p1 = getGate(aigInfo[1]/2);
   CirGate* p2 = getGate(aigInfo[2]/2);
   unsigned id1 = aigInfo[1]/2;
   unsigned id2 = aigInfo[2]/2;
   if (p1 == 0){
      p1 = new UndefGate(id1);
      _idList[id1] = p1;
	}
   if (p2 == 0){
      p2 = new UndefGate(id2);
      _idList[id2] = p2;
	}
   temp->set_input_inv(aigInfo[1]%2, p1);
   temp->set_input_inv(aigInfo[2]%2, p2);
   p1->set_output_inv(aigInfo[1]%2, temp);
   p2->set_output_inv(aigInfo[2]%2, temp);
}

void
CirMgr::linkPO(unsigned id, unsigned k)
{
	CirGate* temp = getGate(m+k+1);
	CirGate* p = getGate(id/2);
	unsigned id1 = id/2;
	if (p == 0){
		p = new UndefGate(id1);
		_idList[id1] = p;
	}
	temp->set_input_inv(id%2, p);
	p->set_output_inv(id%2, temp);
}

/**********************************************************/
/*   class CirMgr member functions for circuit printing   */
/**********************************************************/
/*********************
Circuit Statistics
==================
  PI          20
  PO          12
  AIG        130
------------------
  Total      162
*********************/
void
CirMgr::printSummary() const
{
   cout << endl;
   cout << "Circuit Statistics" << endl;
   cout << "==================" << endl;
   cout << "  " << setw(7) << left << "PI" << setw(7) << right << i << endl;
   cout << "  " << setw(7) << left << "PO" << setw(7) << right << o << endl;
   cout << "  " << setw(7) << left << "AIG" << setw(7) << right << a << endl;
   cout << "------------------" << endl;
   cout << "  " << setw(7) << left << "Total" << setw(7) << right << i+o+a << endl;
}

void
CirMgr::printNetlist() const
{
   int num = 0;
   CirGate::setGlobalRef();
   cout << endl;
   for (unsigned k = 0; k < o; k++){
      CirGate* temp = getGate(k+m+1);
      temp->dfs4NetList(num);
   }
}

void
CirMgr::printPIs() const
{
   cout << "PIs of the circuit:";
   for (unsigned k = 0; k < i; k++)
      cout << " " << input[k]/2;
   cout << endl;
}

void
CirMgr::printPOs() const
{
   cout << "POs of the circuit:";
   for (unsigned k = 0; k < o; k++)
      cout << " " << k+m+1;
   cout << endl;
}

void
CirMgr::printFloatGates() const
{
   IdList undef, unused;
   for (unsigned k = 0; k < i+o+a; k++){
      if (k < i){
         unsigned id1 = input[k]/2;
         CirGate* temp = getGate(id1);
         if (temp->get_fanoutSize() == 0)
            unused.push_back(id1);
      }
      else if (k >= i && k < i+o){
         unsigned id1 = k-i+m+1;
         CirGate* temp = getGate(id1);
         for (unsigned q = 0; q < temp->get_faninSize(); q++){
            if (temp->get_fanin(q)->getTypeStr() == "UNDEF"){
               undef.push_back(id1);
               break;
            }
         }
      }
      else{
         unsigned id1 = aig[k-i-o][0]/2;
         CirGate* temp = getGate(id1);
         if (temp->get_fanoutSize() == 0)
            unused.push_back(id1);
         for (unsigned q = 0; q < temp->get_faninSize(); q++){
            if (temp->get_fanin(q)->getTypeStr() == "UNDEF"){
               undef.push_back(id1);
               break;
            }
         }
      }
   }
   sort(undef.begin(), undef.end());
   sort(unused.begin(), unused.end());

   if (undef.size() > 0){
      cout << "Gates with floating fanin(s):";
      for (unsigned k = 0; k < undef.size(); k++)
         cout << " " << undef[k];
      cout << endl;
   }
   if (unused.size() > 0){
      cout << "Gates defined but not used  :";
      for (unsigned k = 0; k < unused.size(); k++)
         cout << " " << unused[k];
      cout << endl;
}
}

void
CirMgr::printFECPairs() const
{
}

void
CirMgr::writeAag(ostream& outfile) const
{
   IdList idtemp;
   CirGate::setGlobalRef();

   for (unsigned k = 0; k < o; ++k){
      CirGate* temp = getGate(m+k+1);
      temp->dfs4Write(idtemp);
   }
	
   outfile << "aag " << m << " " << i << " " << l << " "
           << o << " " << idtemp.size() << endl;
   for (unsigned k = 0; k < i; ++k)
      outfile << input[k] << endl;
   for (unsigned k = 0; k < o; ++k)
      outfile << output[k] << endl;

   for (unsigned k = 0; k < idtemp.size(); k++){
      CirGate* temp = getGate(idtemp[k]);
      outfile << 2*idtemp[k];
    //  GateList idd = temp->get_fanin_or_fanout(1);
      for (unsigned n = 0; n < 2; n++){
         unsigned id1 = temp->get_fanin(n)->getIdNo();
         if (temp->input_isINV(n))
            outfile << " " << (2*id1+1);
         else
            outfile << " " << (2*id1);
      }
      outfile << endl;
   }

   for (unsigned k = 0; k < i; ++k){
      CirGate* temp = getGate(input[k]/2);
      string str = temp->getSymStr();
      if (str.size() > 0)
         outfile << "i" << k << " " << str << endl;
   }
   for (unsigned k = 0; k < o; ++k){
      CirGate* temp = getGate(k+m+1);
      string str = temp->getSymStr();
      if (str.size() > 0)
         outfile << "o" << k << " " << str << endl;
   }

   outfile << "c\n";
   outfile << "AAG output by Steven Yang\n";
}

void
CirMgr::writeGate(ostream& outfile, CirGate *g) const
{
}

