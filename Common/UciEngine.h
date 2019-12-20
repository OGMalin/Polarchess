#pragma once

#include <QString>
#include <string>

class UciEngine
{
public:
	UciEngine();
	void clear();
	int readLine(std::string& line, QString& responsestring);
};