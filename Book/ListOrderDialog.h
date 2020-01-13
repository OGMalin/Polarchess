#pragma once

#include <QDialog>
#include <QWidget>
#include <QStringList>
#include <QListWidget>

class ListOrderDialog :public QDialog
{
public:
	ListOrderDialog(QString& label, QWidget* parent);
	void setList(const QStringList&);
	QStringList getList();
private:
	QListWidget* list;
};