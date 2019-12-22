#pragma once

#include <QString>
#include <string>
#include "../Common/BaseEngine.h"

class UciEngine:public BaseEngine
{
protected:
	QMap<QString, QString> initOptions;
	virtual void fromEngine(std::string& input);
public slots:
	virtual void started();
public:
	UciEngine();
};