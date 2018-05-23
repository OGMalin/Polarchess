#pragma once

#include <QMainWindow>
#include <QtWidgets>
#include <QString>
#include "BoardWindow.h"
#include "Scoresheet.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

protected:
	void changeEvent(QEvent*);

protected slots:
	void slotLanguageChanged(QAction* action);
public:
	MainWindow(QWidget *parent = Q_NULLPTR);
	virtual ~MainWindow();
private:
	QMenu * fileMenu;
	QMenu* settingsMenu;
	QMenu* helpMenu;
	QMenu* langMenu;
	QAction* exitAct;
	QAction* engAct;
	QAction* norAct;
	QAction* aboutAct;
	QTranslator translator;
	QActionGroup * langGroup;
	QString locale;
	BoardWindow* boardwindow;
	Scoresheet* scoresheet;
	QSplitter* hSplitter;
	void createMenu();
	void createStatusBar();
	void setLanguage();
	void loadLanguage(const QString& lang);
	void retranslateUi();
};
