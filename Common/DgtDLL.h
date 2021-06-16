#pragma once
#include <QObject>
#include <QLibrary>
#include <Windows.h>

typedef int __stdcall F();
typedef int __stdcall FI(int);
typedef int __stdcall FB(bool);
typedef int __stdcall FC(const char*);
typedef int __stdcall FCC(const char*, const char*);
typedef int __stdcall FFC(FC*);

class DgtDLL :public QObject
{
	Q_OBJECT
private:
	bool visible;
	QLibrary* dgtebdll;
	F* Init;
	F* Exit;
	FI* ShowDialog;
	FI* HideDialog;
//signals:
//	int status(const char*);
public:
	DgtDLL(QObject* parent = NULL);
	~DgtDLL();
	bool isVisible() { return visible; };
	void show();
	void hide();
};