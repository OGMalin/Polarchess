#include "EngineWindow.h"
#include <QCoreApplication>
#include <QStandardPaths>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTableView>
#include <QMenu>
#include <QFontDialog>
#include <QDir>

EngineWindow::EngineWindow(QWidget *parent)
	: QWidget(parent)
{
	char sz[16];
	multipv = 1;
	movenr = 1;
	freezemovenr = 1;
	analyzing = false;
	freezing = false;
	engine = new Engine;
	currentBoard.setStartposition();
	engineName.clear(); 
	iniPath = QStandardPaths::locate(QStandardPaths::DocumentsLocation, QCoreApplication::organizationName(), QStandardPaths::LocateDirectory);
	iniPath += "/" + QCoreApplication::applicationName() + "/Engines";

	QGridLayout* grid = new QGridLayout;

	QHBoxLayout* hbox = new QHBoxLayout;
	lines = new QLabel;
	lines->setAlignment(Qt::AlignCenter);
	lines->setText(itoa(multipv, sz, 10));
	decline = new QPushButton("-");
	incline = new QPushButton("+");
	analyze = new QPushButton("Analyze");
	analyze->setCheckable(true);
	freeze = new QPushButton("Freeze");
	freeze->setCheckable(true);
	nodes = new QLabel;
	nodes->setAlignment(Qt::AlignCenter);
	nps = new QLabel;
	nps->setAlignment(Qt::AlignCenter);
	time = new QLabel;
	time->setAlignment(Qt::AlignCenter);
	selengine = new QComboBox;
	hbox->addWidget(decline);
	hbox->addWidget(lines);
	hbox->addWidget(incline);
	hbox->addWidget(analyze);
	hbox->addWidget(freeze);
	hbox->addWidget(nodes);
	hbox->addWidget(nps);
	hbox->addWidget(time);
	hbox->addWidget(selengine);
	grid->addLayout(hbox, 0, 0);

	model = new QStandardItemModel(multipv, 3);
	QStringList header;
	header << "Score" << "Depth" << "PV";
	model->setHorizontalHeaderLabels(header);
	QTableView* output = new QTableView;
	output->setModel(model);

	grid->addWidget(output, 1, 0);// , Qt::AlignLeft);

	setLayout(grid);

	if (multipv == 1)
		decline->setEnabled(false);

	// Read engine list
	QFileInfoList qfil = QDir(iniPath, "*.eng").entryInfoList(QDir::NoFilter, QDir::Name | QDir::IgnoreCase);
	for (int i = 0; i < qfil.size(); i++)
	{
		if (i == 0)
			engineName = qfil[i].completeBaseName(); // Defaults to first engine.
		selengine->addItem(qfil[i].completeBaseName());
	}

	setContextMenuPolicy(Qt::CustomContextMenu);

	connect(decline, SIGNAL(clicked(bool)), this, SLOT(declineClicked(bool)));
	connect(incline, SIGNAL(clicked(bool)), this, SLOT(inclineClicked(bool)));
	connect(freeze, SIGNAL(clicked(bool)), this, SLOT(freezeClicked(bool)));
	connect(analyze, SIGNAL(clicked(bool)), this, SLOT(analyzeClicked(bool)));
	connect(selengine, SIGNAL(activated(const QString&)), this, SLOT(selectEngine(const QString&)));
	connect(engine, SIGNAL(engineReady()), this, SLOT(engineReady()));
	connect(engine, SIGNAL(engineMove(const QString&, const QString&)), this, SLOT(engineStoped(const QString&, const QString&)));
	connect(engine, SIGNAL(engineStoped()), this, SLOT(engineStoped()));
	connect(engine, SIGNAL(engineInfo(const EngineInfo&)), this, SLOT(engineInfo(const EngineInfo&)));
	connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showContextMenu(const QPoint&)));

	font.setPointSize(12);
	this->setFont(font);
}

EngineWindow::~EngineWindow()
{
	delete engine;
}

void EngineWindow::setPosition(const ChessBoard& cb, int mn)
{
	currentBoard = cb;
	movenr = mn;
	if (analyzing)
		if (!freezing)
			engine->stop();
}

void EngineWindow::analyzeClicked(bool)
{
	if (analyze->isChecked())
	{
		if (analyzing)
			return;
		if (engineName.isEmpty())
		{

			analyze->setChecked(false);
			return;
		}
		QString enginePath = iniPath + "/" + engineName + ".eng";
		if (engine->load(enginePath))
		{
			analyzing = true;
		}
	}
	else
	{
		if (analyzing)
		{
			engine->unload();
			analyzing = false;
		}
	}
}

void EngineWindow::freezeClicked(bool)
{
	if (freeze->isChecked())
	{
		freezing = true;
		freezeBoard = currentBoard;
		freezemovenr = movenr;
	}
	else
	{
		freezing = false;
		if (analyzing)
			engine->stop();
	}
}

void EngineWindow::inclineClicked(bool)
{
	char sz[16];
	if (multipv == 1)
		decline->setEnabled(true);
	++multipv;
	lines->setText(itoa(multipv, sz, 10));
	engine->setMultiPV(multipv);
	model->setRowCount(multipv);
}

void EngineWindow::declineClicked(bool)
{
	char sz[16];
	if (multipv > 1)
	{
		--multipv;
		lines->setText(itoa(multipv, sz, 10));
		if (multipv == 1)
			decline->setEnabled(false);
		engine->setMultiPV(multipv);
		model->setRowCount(multipv);
	}
}

void EngineWindow::engineReady()
{
	engine->setMultiPV(multipv);
	if (analyzing)
		engine->analyze(currentBoard, movelist);
}

void EngineWindow::engineStoped()
{
	if (analyzing)
		engine->analyze(currentBoard, movelist);
}

void EngineWindow::engineInfo(const EngineInfo& info)
{
	char sz[16];
	int line = info.multipv;
	if (line < 1)
		line = 1;
//	model->clear();
	if (info.time)
		time->setText(itoa(info.time / 1000, sz, 10) + QString("s"));
	if (info.nodes)
		nodes->setText(itoa(info.nodes, sz, 10) + QString(" nodes"));
	if (info.nps)
		nps->setText(itoa(info.nps, sz, 10) + QString(" nps"));

	if (info.pv.size)
	{
		QStandardItem* item;
		QString s;
		ChessBoard b;
		ChessMove m;
		int mn;
		if (freezing)
		{
			b = freezeBoard;
			mn = freezemovenr;
		}
		else
		{
			b = currentBoard;
			mn = movenr;
		}
		if (b.isLegal(ChessMove(info.pv.list[0])))
		{
			if (info.lowerbound)
				item = new QStandardItem("--");
			else if (info.upperbound)
				item = new QStandardItem("++");
			else if (info.mate)
				item = new QStandardItem(QString("M") + itoa(info.mate, sz, 10));
			else
				item = new QStandardItem(itoa(info.cp, sz, 10));
			item->setEditable(false);
			item->setTextAlignment(Qt::AlignCenter);
			model->setItem(line - 1, 0, item);

			item = new QStandardItem(itoa(info.depth, sz, 10));
			item->setEditable(false);
			item->setTextAlignment(Qt::AlignCenter);
			model->setItem(line - 1, 1, item);

			for (int i = 0; i < info.pv.size; i++)
			{
				m = info.pv.list[i];
				if (b.toMove == WHITE)
				{
					s += itoa(mn + i / 2, sz, 10) + QString(".");
				}
				else if (i == 0)
				{
					s += itoa(mn, sz, 10) + QString("...");
					++mn;
				}
				s += QString(b.makeMoveText(m, FIDE).c_str());
				if (i < (info.pv.size - 1))
					s += " ";
				b.doMove(m, false);
			}
			item = new QStandardItem(s);
			item->setEditable(false);
			item->setTextAlignment(Qt::AlignLeft);
			model->setItem(line - 1, 2, item);
		}
	}
}

void EngineWindow::showContextMenu(const QPoint& pos)
{
	QMenu* contextMenu = new QMenu(this);
	contextMenu->addAction(QString("Font"), this, SLOT(selectFont()));
	contextMenu->exec(mapToGlobal(pos));
}

void EngineWindow::selectFont()
{
	bool ok;
	QFont f = QFontDialog::getFont(&ok, font, this);
	if (ok)
	{
		font = f;
		this->setFont(font);
	}
}

void EngineWindow::selectEngine(const QString& eng)
{
	if (analyzing)
		analyze->animateClick();
	engineName = eng;
}