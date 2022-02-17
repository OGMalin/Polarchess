#pragma once

#include <string>

// Returns a word from a string where words are separated with whitespace.
// First word is n = 1
extern const std::string getWord(const std::string& s, int n, char* separator = NULL);

// Returns a line from a string where line are separated with \r or \n.
// First line is n = 1
extern const std::string getLine(const std::string& s, int n);

//Return characters upto a newline ('\n' not included).
extern const std::string getLine(const char* sz);

// Removes whitespace in front and after a string.
extern const std::string trim(const std::string& s);
extern char* trim(char* s);

// Force all char to lowercase.
extern const std::string lowercase(const std::string& s);
extern char* lowercase(char* s);

// Removes whitespace in front and after a string and set
// all whitespace inbetween to a single space
extern const std::string trunc(const std::string& s);

extern bool booleanString(const std::string& s);
extern bool existIn(char c, const char* str);
extern bool existIn(const char* c, const char* str);

extern unsigned int rand32();
extern unsigned __int64 rand64();

// Checking to see if a string is a number.
// type = 0 (decimal plus '.+- ', 8 (octal), 10 (decimal) or 16 (hexadecimal)
extern bool isNumber(const std::string& s, int type = 0);

extern void breakLines(std::string& s, int linelength);

extern const std::string getQuotedString(std::string& s);

// Get the directory to a program.
// Ex. c:\Program Files\Engines\WCrafty.exe would return c:\Program Files\Engines\
// and ..\Engines\ could also return c:\Program Files\Engines\
// If this routine fails to find the directory it return null.
extern char* findPath(const char* fullpath, char* buf, int len);

// Get the filename without path and extention.
extern const std::string getFilename(std::string fullpath);

extern const std::string getProgramPath();