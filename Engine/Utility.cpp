#include <Windows.h>
#include "Utility.h"

using namespace std;

char WhiteSpace[] = " \t\n\r\v\f";

const string getWord(const string& input, int n, char* separator)
{
	size_t i1, i2;
	int j = 1;
	char* sep;
	if (!separator)
		sep = WhiteSpace;
	else
		sep = separator;
	i1 = input.find_first_not_of(sep, 0);
	while (j<n)
	{
		i1 = input.find_first_of(sep, i1);
		if (i1 == string::npos)
			return "";
		i1 = input.find_first_not_of(sep, i1);
		if (i1 == string::npos)
			return "";
		j++;
	};
	if (i1 == string::npos)
		return "";
	i2 = input.find_first_of(sep, i1);
	if (i2 == string::npos)
		i2 = input.length();
	return input.substr(i1, i2 - i1);
}

const string trim(const string& s)
{
	if (s.length() == 0)
		return s;
	size_t b = s.find_first_not_of(WhiteSpace);
	size_t e = s.find_last_not_of(WhiteSpace);
	if (b == string::npos) // No non-spaces
		return "";
	return string(s, b, e - b + 1);
}

char* trim(char* str)
{
	int p;
	size_t start, end;
	end = strlen(str);
	start = 0;
	while ((unsigned char)str[start] <= ' ')
	{
		start++;
		if (start >= end)
		{
			str[0] = '\0';
			return str;
		}
	}
	while ((unsigned char)str[end - 1] <= ' ')
	{
		end--;
		if (end <= start)
		{
			str[0] = '\0';
			return str;
		}
	}
	p = 0;
	while (start<end)
	{
		str[p] = str[start];
		p++;
		start++;
	}
	str[p] = '\0';
	return str;
}

bool booleanString(const std::string& s)
{
	string cs;
	if (s.length()<1)
		return false;
	unsigned int i;
	cs = "";
	for (i = 0; i<s.length(); i++)
		cs += toupper(s.at(i));
	if (cs.at(0) == 'T')
		return true;
	if (cs.at(0) == 'F')
		return false;
	if (cs == "ON")
		return true;
	if (cs == "OFF")
		return false;
	if (isdigit(cs.at(0)))
	{
		if (cs.at(0) == '0')
			return false;
		return true;
	}
	return false;
}

const std::string lowercase(const std::string& s)
{
	string line;
	size_t i, len;
	i = 0;
	len = s.length();
	while (i<len)
	{
		line += tolower(s.at(i));
		i++;
	}
	return line;
}

char* lowercase(char* s)
{
	size_t i, len;
	i = 0;
	len = strlen(s);
	while (i<len)
	{
		s[i] += tolower(s[i]);
		i++;
	}
	return s;
}

bool existIn(char c, const char* str)
{
	char* p = (char*)str;
	while (*p)
	{
		if (c == *p)
			return true;
		p++;
	}
	return false;
}

bool existIn(const char* c, const char* str)
{
	size_t i = 0, len = strlen(c);
	while (i<len)
		if (existIn(c[i++], str))
			return true;
	return false;
}

// Random number generator
// It is building upon Donald Knuth's description in 
// "The Art Of Computer Programming" Vol.2 third edition page 185-186
unsigned int _rand32()
{
	//  const unsigned int prime=2147483647;
	const unsigned int a = 48271;
	const unsigned int q = 44488;  // prime/a
	const unsigned int r = 3399;   // prime%a

								   //  static unsigned int seed=time(NULL);
	static unsigned int seed = 1;
	unsigned int x;
	x = a * (seed%q) - r * (seed / q);
	seed = x;
	return x;
}

unsigned int rand32()
{
	const unsigned int prime = 2147483399;
	const unsigned int a = 40692;
	const unsigned int q = 52774;
	const unsigned int r = 3791;

	//  static unsigned int seed=time(NULL);
	static unsigned int seed = 1;
	unsigned int y, x;
	y = a * (seed%q) - r * (seed / q);
	seed = y;
	x = _rand32();
	if (y >= x)
		return (x + prime - y - 1);
	return (x - y);
}

unsigned __int64 rand64()
{
	unsigned __int64 i64, low, high;
	low = rand32();
	high = rand32();
	i64 = (high << 32) | low;
	return i64;
}

const std::string getProgramPath()
{
	string s;
	char sz[MAX_PATH];
	if (!GetModuleFileName(NULL, sz, MAX_PATH))
		return string("");
	string::size_type size;
	s = sz;
	size = s.find_last_of('\\');
	if (size != string::npos)
		return s.substr(0, size + 1);
	return string("");
}