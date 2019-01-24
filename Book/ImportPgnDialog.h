#pragma once

#include <QDialog>

class QLineEdit;
class QRadioButton;
//class Database;

class ImportPgnDialog : public QDialog
{
	Q_OBJECT

public:
	ImportPgnDialog(QWidget *parent);
	void setItems(bool theory, bool rep, bool white, int moves);
	void getItems(QString& path, bool& theory, bool& rep, bool& white, int& moves);


public slots:
	void openFile();

private:
	QLineEdit* filename;
	QRadioButton* theoryfile;
	QRadioButton* repfile;
	QRadioButton* whiterep;
	QRadioButton* blackrep;
	QLineEdit* numberofmoves;
};

