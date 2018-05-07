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
