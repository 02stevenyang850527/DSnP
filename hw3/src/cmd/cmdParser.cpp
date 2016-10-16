/****************************************************************************
  FileName     [ cmdParser.cpp ]
  PackageName  [ cmd ]
  Synopsis     [ Define command parsing member functions for class CmdParser ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <cassert>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include "util.h"
#include "cmdParser.h"

using namespace std;

//----------------------------------------------------------------------
//    External funcitons
//----------------------------------------------------------------------
void mybeep();


//----------------------------------------------------------------------
//    Member Function for class cmdParser
//----------------------------------------------------------------------
// return false if file cannot be opened
// Please refer to the comments in "DofileCmd::exec", cmdCommon.cpp
bool
CmdParser::openDofile(const string& dof)
{
   // TODO...
	_dofile = new ifstream(dof.c_str());
	_dofileStack.push(_dofile);
	if (!_dofile->is_open())
	{	closeDofile(); return false; }
	else  return true; 
}

// Must make sure _dofile != 0
void
CmdParser::closeDofile()
{
	assert(_dofile != 0);
   // TODO...
	_dofile->close();
	delete _dofile;
	_dofileStack.pop();
	if (_dofileStack.empty())
		_dofile = 0;
	else
		_dofile = _dofileStack.top();
}

// Return false if registration fails
bool
CmdParser::regCmd(const string& cmd, unsigned nCmp, CmdExec* e)
{
   // Make sure cmd hasn't been registered and won't cause ambiguity
   string str = cmd;
   unsigned s = str.size();
   if (s < nCmp) return false;
   while (true) {
      if (getCmd(str)) return false;
      if (s == nCmp) break;
      str.resize(--s);
   }

   // Change the first nCmp characters to upper case to facilitate
   //    case-insensitive comparison later.
   // The strings stored in _cmdMap are all upper case
   //
   assert(str.size() == nCmp);  // str is now mandCmd
   string& mandCmd = str;
   for (unsigned i = 0; i < nCmp; ++i)
      mandCmd[i] = toupper(mandCmd[i]);
   string optCmd = cmd.substr(nCmp);
   assert(e != 0);
   e->setOptCmd(optCmd);

   // insert (mandCmd, e) to _cmdMap; return false if insertion fails.
   return (_cmdMap.insert(CmdRegPair(mandCmd, e))).second;
}

// Return false on "quit" or if excetion happens
CmdExecStatus
CmdParser::execOneCmd()
{
   bool newCmd = false;
   if (_dofile != 0)
      newCmd = readCmd(*_dofile);
   else
      newCmd = readCmd(cin);

   // execute the command
   if (newCmd) {
      string option;
      CmdExec* e = parseCmd(option);
      if (e != 0)
         return e->exec(option);
   }
   return CMD_EXEC_NOP;
}

// For each CmdExec* in _cmdMap, call its "help()" to print out the help msg.
// Print an endl at the end.
void
CmdParser::printHelps() const
{
   // TODO...
	for (CmdMap::const_iterator i = _cmdMap.begin(); i != _cmdMap.end(); ++i)
		i->second->help();
	cout << endl;
}

void
CmdParser::printHistory(int nPrint) const
{
   assert(_tempCmdStored == false);
   if (_history.empty()) {
      cout << "Empty command history!!" << endl;
      return;
   }
   int s = _history.size();
   if ((nPrint < 0) || (nPrint > s))
      nPrint = s;
   for (int i = s - nPrint; i < s; ++i)
      cout << "   " << i << ": " << _history[i] << endl;
}


//
// Parse the command from _history.back();
// Let string str = _history.back();
//
// 1. Read the command string (may contain multiple words) from the leading
//    part of str (i.e. the first word) and retrive the corresponding
//    CmdExec* from _cmdMap
//    ==> If command not found, print to cerr the following message:
//        Illegal command!! "(string cmdName)"
//    ==> return it at the end.
// 2. Call getCmd(cmd) to retrieve command from _cmdMap.
//    "cmd" is the first word of "str".
// 3. Get the command options from the trailing part of str (i.e. second
//    words and beyond) and store them in "option"
//
CmdExec*
CmdParser::parseCmd(string& option)
{
	assert(_tempCmdStored == false);
	assert(!_history.empty());
	string str = _history.back();

   // TODO...
	string token;
	vector<string> tokens;
	size_t n =myStrGetTok(str, token);
	while (token.size()){
		tokens.push_back(token);
		n = myStrGetTok(str, token, n);
	}
	option.clear();
	CmdExec* e = getCmd(tokens[0]);
	if (e != 0){
		for (size_t i=1; i < tokens.size(); i++)
		{	option += tokens[i]; option += " ";}
		option = option.substr(0, option.length()-1);
		return e;
	}

	cerr << "Illegal Command!!" <<" (" << token[0] << ")" << endl;
	assert(str[0] != 0 && str[0] != ' ');
	return NULL;
}

// This function is called by pressing 'Tab'.
// It is to list the partially matched commands.
// "str" is the partial string before current cursor position. It can be 
// a null string, or begin with ' '. The beginning ' ' will be ignored.
//
// Several possibilities after pressing 'Tab'
// (Let $ be the cursor position)
// 1. [Before] Null cmd
//    cmd> $
//    -----------
//    [Before] Cmd with ' ' only
//    cmd>     $
//    [After Tab]
//    ==> List all the commands, each command is printed out by:
//           cout << setw(12) << left << cmd;
//    ==> Print a new line for every 5 commands
//    ==> After printing, re-print the prompt and place the cursor back to
//        original location (including ' ')
//
// 2. [Before] partially matched (multiple matches)
//    cmd> h$                   // partially matched
//    [After Tab]
//    HELp        HIStory       // List all the parially matched commands
//    cmd> h$                   // and then re-print the partial command
//    -----------
//    [Before] partially matched (multiple matches)
//    cmd> h$aaa                // partially matched with trailing characters
//    [After Tab]
//    HELp        HIStory       // List all the parially matched commands
//    cmd> h$aaa                // and then re-print the partial command
//
// 3. [Before] partially matched (single match)
//    cmd> he$
//    [After Tab]
//    cmd> heLp $
//    -----------
//    [Before] partially matched with trailing characters (single match)
//    cmd> he$hahah
//    [After Tab]
//    cmd> heLp $hahaha
//    ==> Automatically complete on the same line
//    ==> The auto-expanded part follow the strings stored in cmd map and
//        cmd->_optCmd. Insert a space after "heLp"
//
// 4. [Before] No match
//    cmd> hek$
//    [After Tab]
//    ==> Beep and stay in the same location
//
// 5. [Before] Already matched
//    cmd> help asd$fg
//    [After] Print out the usage for the already matched command
//    Usage: HELp [(string cmd)]
//    cmd> help asd$fg
//
// 6. [Before] Cursor NOT on the first word and NOT matched command
//    cmd> he haha$kk
//    [After Tab]
//    ==> Beep and stay in the same location
//
void
CmdParser::listCmd(const string& str)
{
   // TODO...
	vector<string> list;
	vector<string> tokens;
	string token;
	size_t n = myStrGetTok(str, token);
	while (token.size()){
		tokens.push_back(token);
		n = myStrGetTok(str, token, n);
	}

	if (tokens.size() == 0){
		for (CmdMap::const_iterator i = _cmdMap.begin(); i != _cmdMap.end(); ++i)
			list.push_back(i->first + i->second->getOptCmd());
		cout << endl;
		for (int i=0; i < int(list.size()); i++){
			cout << setw(12) << left << list[i];
			if ((i+1) % 5 == 0) cout << endl;
		}
		reprintCmd();
	}	// #1 done

	if (tokens.size() == 1){
		for (CmdMap::const_iterator i = _cmdMap.begin(); i != _cmdMap.end(); ++i){
			string temp = i->first + i->second->getOptCmd();
			if (temp.size() >= tokens[0].size())
			if (myStrNCmp(temp, tokens[0], tokens[0].length()) == 0) 
				list.push_back(temp);
		}

		if (list.size() == 0)
			mybeep();    // #4 done
		else if (int(list.size()) == 1){
			if (str.length() == tokens[0].length()){
				for (size_t i = tokens[0].length(); i < list[0].length(); i++)
					insertChar(list[0][i]);
				insertChar(' ');
			}
		}
		else
		{	cout << endl;
			for (int i=0; i < int(list.size()); i++){
				cout << setw(12) << left << list[i];
			if ((i+1) % 5 == 0 && i != int(list.size()) - 1) cout << endl;
			}
			reprintCmd();
		}
	}  // #2, #3 done

	if (tokens.size() >= 1 && (str.length() > tokens[0].size()) ){
		list.clear();
		for (CmdMap::const_iterator i = _cmdMap.begin(); i != _cmdMap.end(); ++i){
			string temp = i->first + i->second->getOptCmd();
			if (temp.size() >= tokens[0].size())
			if (myStrNCmp(temp, tokens[0], tokens[0].length()) == 0)
				list.push_back(temp);
		}

		if (list.size() == 0)
			mybeep();         // #6 done
		if (list.size() == 1){
			for (CmdMap::const_iterator i = _cmdMap.begin(); i != _cmdMap.end(); ++i)
				if (myStrNCmp(list[0], i->first, i->first.length()) == 0)
				{	if (tokens[0].length() >= i->first.length())
					{	cout << endl; i->second->usage(cout); reprintCmd();}
					else 
						mybeep();
					 return; 
				}
		}
	/*	if (list.size() > 1)
		{	cerr << "\nsomething shit happens!\n";    // for self-check 
			for (size_t i =0; i < list.size(); i++)
				cout << list[i] << endl;
		}*/
	}
}

// cmd is a copy of the original input
//
// return the corresponding CmdExec* if "cmd" matches any command in _cmdMap
// return 0 if not found.
//
// Please note:
// ------------
// 1. The mandatory part of the command string (stored in _cmdMap) must match
// 2. The optional part can be partially omitted.
// 3. All string comparison are "case-insensitive".
//
CmdExec*
CmdParser::getCmd(string cmd)
{
	CmdExec* e = 0;
   // TODO...
	for (CmdMap::const_iterator i = _cmdMap.begin(); i != _cmdMap.end(); ++i){
		string str = i->first + i->second->getOptCmd();
		if (myStrNCmp(str, cmd, i->first.length()) == 0)
		{	e = i->second;
			return e;
		}
	}
	return 0;
}


//----------------------------------------------------------------------
//    Member Function for class CmdExec
//----------------------------------------------------------------------
// Return false if error options found
// "optional" = true if the option is optional XD
// "optional": default = true
//
bool
CmdExec::lexSingleOption
(const string& option, string& token, bool optional) const
{
   size_t n = myStrGetTok(option, token);
   if (!optional) {
      if (token.size() == 0) {
         errorOption(CMD_OPT_MISSING, "");
         return false;
      }
   }
   if (n != string::npos) {
      errorOption(CMD_OPT_EXTRA, option.substr(n));
      return false;
   }
   return true;
}

// if nOpts is specified (!= 0), the number of tokens must be exactly = nOpts
// Otherwise, return false.
//
bool
CmdExec::lexOptions
(const string& option, vector<string>& tokens, size_t nOpts) const
{
   string token;
   size_t n = myStrGetTok(option, token);
   while (token.size()) {
      tokens.push_back(token);
      n = myStrGetTok(option, token, n);
   }
   if (nOpts != 0) {
      if (tokens.size() < nOpts) {
         errorOption(CMD_OPT_MISSING, "");
         return false;
      }
      if (tokens.size() > nOpts) {
         errorOption(CMD_OPT_EXTRA, tokens[nOpts]);
         return false;
      }
   }
   return true;
}

CmdExecStatus
CmdExec::errorOption(CmdOptionError err, const string& opt) const
{
   switch (err) {
      case CMD_OPT_MISSING:
         cerr << "Error: Missing option";
         if (opt.size()) cerr << " after (" << opt << ")";
         cerr << "!!" << endl;
      break;
      case CMD_OPT_EXTRA:
         cerr << "Error: Extra option!! (" << opt << ")" << endl;
      break;
      case CMD_OPT_ILLEGAL:
         cerr << "Error: Illegal option!! (" << opt << ")" << endl;
      break;
      case CMD_OPT_FOPEN_FAIL:
         cerr << "Error: cannot open file \"" << opt << "\"!!" << endl;
      break;
      default:
         cerr << "Error: Unknown option error type!! (" << err << ")" << endl;
      exit(-1);
   }
   return CMD_EXEC_ERROR;
}

