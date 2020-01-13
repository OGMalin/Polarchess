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
	timeLimit = 59999;
	engineReady = false;
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
	freeze = new QPushButton("Freeze");
	analyze->setCheckable(true);
	freeze->setCheckable(true);
	analyze->setEnabled(false);
	freeze->setEnabled(false);
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

	//Enable an empty spot in the engine list to unload engines
	selengine->addItem(QString(""));
	engineName.clear();
	// Read engine list
	QFileInfoList qfil = QDir(iniPath, "*.eng").entryInfoList(QDir::NoFilter, QDir::Name | QDir::IgnoreCase);
	for (int i = 0; i < qfil.size(); i++)
		selengine->addItem(qfil[i].completeBaseName());

	setContextMenuPolicy(Qt::CustomContextMenu);

	connect(decline, SIGNAL(clicked(bool)), this, SLOT(slotDeclineClicked(bool)));
	connect(incline, SIGNAL(clicked(bool)), this, SLOT(slotInclineClicked(bool)));
	connect(freeze, SIGNAL(clicked(bool)), this, SLOT(slotFreezeClicked(bool)));
	connect(analyze, SIGNAL(clicked(bool)), this, SLOT(slotAnalyzeClicked(bool)));
	connect(selengine, SIGNAL(activated(const QString&)), this, SLOT(slotSelectEngine(const QString&)));
	connect(engine, SIGNAL(engineInfo(const EngineInfo&)), this, SLOT(slotEngineInfo(const EngineInfo&)));
	connect(engine, SIGNAL(engineStarted()), this, SLOT(slotEngineStarted()));
	connect(engine, SIGNAL(engineStoped()), this, SLOT(slotEngineStoped()));
	connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(slotShowContextMenu(const QPoint&)));

	font.setPointSize(12);
	this->setFont(font);
}

EngineWindow::~EngineWindow()
{
	delete engine;
}

void EngineWindow::setPosition(ChessBoard& cb, int mn)
{
	currentBoard = cb;
	movenr = mn;
	if (!engineReady)
		return;
	if (analyzing)
		if (!freezing)
		{
			if (engine->needRestart())
				return;
			engine->analyze(currentBoard);
		}
}

void EngineWindow::slotAnalyzeClicked(bool)
{
	if (!engineReady)
		return;
	if (analyze->isChecked())
	{
		if (analyzing)
			return;
		if (engineName.isEmpty())
		{
			analyze->setChecked(false);
			return;
		}
		analyzing = true;
		if (engine->needRestart())
			return;
		if (multipv > 1)
			engine->setMultiPV(multipv);
		engine->analyze(currentBoard);
	}
	else
	{
		if (analyzing)
		{
			engine->stop();
			analyzing = false;
		}
	}
}

void EngineWindow::slotFreezeClicked(bool)
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

void EngineWindow::slotInclineClicked(bool)
{
	char sz[16];
	if (multipv == 1)
		decline->setEnabled(true);
	++multipv;
	lines->setText(itoa(multipv, sz, 10));
	engine->setMultiPV(multipv);
	model->setRowCount(multipv);
}

void EngineWindow::slotDeclineClicked(bool)
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

void EngineWindow::slotEngineInfo(const EngineInfo& info)
{
	char sz[16];
	bool send = false;
	int line = info.multipv;
	if (line < 1)
		line = 1;
//	model->clear();
	if (info.time)
		time->setText(itoa(info.time / 1000, sz, 10) + QString(" s"));
	if (info.nodes)
	{
		if (info.nodes<10000)
			nodes->setText(itoa(info.nodes, sz, 10) + QString(" n"));
		else if (info.nodes<10000000)
			nodes->setText(itoa(info.nodes/1000, sz, 10) + QString(" kn"));
		else
			nodes->setText(itoa(info.nodes/1000000, sz, 10) + QString(" Mn"));
	}
	if (info.nps)
	{
		if (info.nps < 10000)
			nps->setText(itoa(info.nps / 1000, sz, 10) + QString(" nps"));
		else if (info.nps < 10000000)
			nps->setText(itoa(info.nps, sz, 10) + QString(" knps"));
		else
			nps->setText(itoa(info.nps, sz, 10) + QString(" Mnps"));
	}
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
			if ((info.time >= timeLimit) && (line==1) && !info.lowerbound && !info.upperbound)
			{
				ComputerDBEngine ce;
				ce.engine = engineName;
				ce.cp = info.cp;
				ce.depth = info.depth;
				ce.time = info.time;
				ce.pv = info.pv;
				emit enginePV(ce, freezing?freezeBoard:currentBoard);
			}
		}
	}
}

void EngineWindow::slotShowContextMenu(const QPoint& pos)
{
	QMenu* contextMenu = new QMenu(this);
	contextMenu->addAction(QString("Font"), this, SLOT(slotSelectFont()));
	contextMenu->exec(mapToGlobal(pos));
}

void EngineWindow::slotSelectFont()
{
	bool ok;
	QFont f = QFontDialog::getFont(&ok, font, this);
	if (ok)
	{
		font = f;
		this->setFont(font);
	}
}

void EngineWindow::slotSelectEngine(const QString& eng)
{
	if (engineName == eng)
		return;
	engineName = eng;
	if (engine->isLoaded())
		engine->unload();
	analyzing = false;
	engineReady = false;
	if (engineName.isEmpty())
		return;
	QString enginePath = iniPath + "/" + engineName + ".eng";
	engine->load(enginePath);
}

void EngineWindow::slotEngineStarted()
{
	engineReady = true;
	analyze->setEnabled(true);
	freeze->setEnabled(true);
}

void EngineWindow::slotEngineStoped()
{
	engineReady = false;
	freeze->setChecked(false);
	analyze->setEnabled(false);
	freeze->setEnabled(false);
	analyze->setChecked(false);
	analyzing = false;
}
