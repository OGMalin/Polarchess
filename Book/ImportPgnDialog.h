#pragma once

#include <QDialog>
#include <QString>

class QLineEdit;
class QRadioButton;
class Database;
class QCheckBox;

class ImportPgnDialog : public QDialog
{
	Q_OBJECT

public:
	ImportPgnDialog(QWidget *parent);
	void setItems(int moves, bool com, bool var);
	void getItems(QString& path, int& moves, bool& com, bool& var);
	void importPgnFile(QWidget* parent, Database* db, QString& pgnfile, int moves, bool comment, bool variation);

public slots:
	void openFile();

private:
	QLineEdit* filename;
	QCheckBox* comment;
	QCheckBox* variation;
	QLineEdit* numberofmoves;
};

