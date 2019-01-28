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
	void setItems(bool theory, bool rep, bool white, int moves);
	void getItems(QString& path, bool& theory, bool& rep, bool& white, int& moves, bool& com, bool& var);
	void importPgnFile(QWidget* parent, Database* db, QString& pgnfile, bool whiterep, bool blackrep, int moves, bool comment, bool variation);

public slots:
	void openFile();
	void typeChanged();

private:
	QLineEdit* filename;
	QRadioButton* theoryfile;
	QRadioButton* repfile;
	QRadioButton* whiterep;
	QRadioButton* blackrep; 
	QCheckBox* comment;
	QCheckBox* variation;
	QLineEdit* numberofmoves;
};

