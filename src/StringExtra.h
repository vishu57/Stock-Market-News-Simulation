//File "StringExtra.h"  C++ declarations of some useful Java capabilities.

#ifndef STRINGEXTRA__H
#define STRINGEXTRA__H

#include <string>
#include <sstream>  // for new itoa,ftoa,atof,atoi

using namespace std;

string ftoa(const double& x);
string itoa(const int& x);
string ltoa(const long& x);
double atof(const string& s);
int atoi(const string& s);
long atol(const string& s);

bool isWhitespace(char c);

string trim(string str);

bool startsWith(string str, string prefix);
bool endsWith(string str, string suffix);

string toLowerCase(string str);

bool equalsIgnoreCase(string str1, string str2);

#endif
