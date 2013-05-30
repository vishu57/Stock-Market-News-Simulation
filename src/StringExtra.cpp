//File "StringExtra.cc"  C++ definitions of some useful Java capabilities.

#include <string>
#include <iostream>
#include <sstream> ///<stringstream> uses ANSI string class instead of char*,
/// and would avoid use of ".c_str()" below, but Sun CC doesn't have it yet.
using namespace std;

#include "StringExtra.h"
#include <stdio.h>  // for sprintf---NO LONGER NEEDED

string ftoa(const double& x) {  //Converts float or double to an ANSI string.
   ostringstream OUT;
   OUT << x;
   //return OUT.str();
   return string(OUT.str());  ///better than "return (string)(OUT.str);" ? 
   //char b[100]; sprintf(b, "%f", x); string s = string(b); return s;
}  //This code and itoa based on Cay Horstmann, /Computing Concepts with C++/

string itoa(const int& x) {  //Converts [short] int to an ANSI string.
   ostringstream OUT;
   OUT << x;
   return string(OUT.str());
   //char b[100]; sprintf(b, "%d", x); string s = string(b); return s;
}

string ltoa(const long& x) { //Converts long into to an ANSI string.
   ostringstream OUT;
   OUT << x;
   return string(OUT.str());
   //char b[100]; sprintf(b, "%d", x); string s = string(b); return s;
}

double atof(const string& s) {
   istringstream IN(s.c_str());
   double temp;
   IN >> temp;
   return temp;
}
   
int atoi(const string& s) {
   istringstream IN(s.c_str());
   int temp;
   IN >> temp; 
   return temp;
}

long atol(const string& s) {
   istringstream IN(s.c_str());
   long temp;
   IN >> temp;
   return temp;
}

bool isWhitespace(char c){return c==' ' || c=='\n' || c=='\t' || c=='\0';}

string trim(string str) {
   int i = 0; int j = (int) str.length() - 1;  ///in C++, length is a method too.
   while (i < j && isWhitespace(str.at(i))) i++;
   if (i==j+1) return (string)""; //string is all whitespace
   else {               //in this case str.at(i) != whitespace and acts as a
      while (isWhitespace(str.at(j))) j--; //sentinel, so need not say j >= i
      return str.substr(i,j-i+1);          //take j-i+1 chars from position i
   }
}

bool startsWith(string str, string prefix) {
   return str.find(prefix) == 0;
}

bool endsWith(string str, string suffix) {
   return str.rfind(suffix) == str.length()-1;
}

string toLowerCase(string str) {
   char c;       
   for (int i = 0; i < (int)str.length(); i++) {
   /// ^ Putting "unsigned" before "int" avoids a g++ -Wall warning, but is
   /// nerdy and nebbishy and wrong---better advice is "Avoid Using Unsigned"!
   /// Note that Java eliminated all "unsigned" types.  Unsigned->systems use.
      c = str.at(i);
      if ('A' <= c && c <= 'Z') {
         c += 32;   //converts to ASCII lower case.
         str.at(i) = c;
      }
   }
   return str;
}

bool equalsIgnoreCase(string str1, string str2) {
   return (toLowerCase(str1) == toLowerCase(str2));
}

