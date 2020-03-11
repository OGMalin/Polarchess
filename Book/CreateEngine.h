#pragma once
#include <QDialog>
#include <QLineEdit>
#include <QComboBox>

class CreateEngine : public QDialog
{
public:
	CreateEngine(QWidget* parent);

private:
	QLineEdit* enginePath;
	QComboBox* engineType;
};
