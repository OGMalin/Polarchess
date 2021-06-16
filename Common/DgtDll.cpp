#include "../Common/DgtDLL.h"


DgtDLL::DgtDLL(QObject* parent)
	:QObject(parent)
{
	visible = false;
	dgtebdll = new QLibrary("dgtebdll64", this);
	Init = (F*)dgtebdll->resolve("_DGTDLL_Init");
	if (Init != nullptr)
		Init();
	ShowDialog = (FI*)dgtebdll->resolve("_DGTDLL_ShowDialog");
	HideDialog = (FI*)dgtebdll->resolve("_DGTDLL_HideDialog");

}

DgtDLL::~DgtDLL()
{
	dgtebdll->unload();
}

void DgtDLL::show()
{
	if (ShowDialog(0))
		visible = false;
	else
		visible = true;
}

void DgtDLL::hide()
{
	HideDialog(0);
	visible = false;
}
