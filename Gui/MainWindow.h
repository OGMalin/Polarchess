#pragma once

#include <QMainWindow>
#include <QtWidgets>
#include <QString>

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
	void createMenu();
	void createStatusBar();
	void setLanguage();
	void loadLanguage(const QString& lang);
	void retranslateUi();
};
