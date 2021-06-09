#include "ImportLC.h"
#include "../Common/ChessMove.h"
#include "../Common/MoveList.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QVector>
#include <QApplication>

ImportLC::ImportLC(QWidget* parent)
	: QDialog(parent) 
{
	QFont f=font();
	f.setPointSize(10);
	setFont(f);
	QVBoxLayout* vbox;
	QHBoxLayout* hbox;
	QPushButton* button;
	QButtonGroup* group;
	QLabel* label;
	vbox = new QVBoxLayout;
	setWindowTitle("Import from OpeningExplorer");

	hbox = new QHBoxLayout;
	label = new QLabel("Link:");
	link = new QLineEdit;
	hbox->addWidget(label);
	hbox->addWidget(link);
	vbox->addLayout(hbox);

	hbox = new QHBoxLayout;
	label = new QLabel("Rating:");
	rating1600 = new QCheckBox("1600");
	rating1800 = new QCheckBox("1800");
	rating2000 = new QCheckBox("2000");
	rating2200 = new QCheckBox("2200");
	rating2500 = new QCheckBox("2500");
	hbox->addWidget(label);
	hbox->addWidget(rating1600);
	hbox->addWidget(rating1800);
	hbox->addWidget(rating2000);
	hbox->addWidget(rating2200);
	hbox->addWidget(rating2500);
	vbox->addLayout(hbox);

	hbox = new QHBoxLayout;
	label = new QLabel("Speed:");
	bullet = new QCheckBox("Bullet");
	blitz = new QCheckBox("Blitz");
	rapid = new QCheckBox("Rapid");
	classical = new QCheckBox("Classical");
	hbox->addWidget(label);
	hbox->addWidget(bullet);
	hbox->addWidget(blitz);
	hbox->addWidget(rapid);
	hbox->addWidget(classical);
	vbox->addLayout(hbox);

	hbox = new QHBoxLayout;
	label = new QLabel("Number og games:");
	played = new QLineEdit;
	hbox->addWidget(label);
	hbox->addWidget(played);
	label = new QLabel("in %:");
	playedp = new QLineEdit;
	hbox->addWidget(label);
	hbox->addWidget(playedp);
	label = new QLabel("first move (%):");
	playedpfirst = new QLineEdit;
	hbox->addWidget(label);
	hbox->addWidget(playedpfirst);
	vbox->addLayout(hbox);

	hbox = new QHBoxLayout;
	overwright = new QCheckBox("Overwrigth");
	hbox->addWidget(overwright);
	vbox->addLayout(hbox);

	hbox = new QHBoxLayout;
	button = new QPushButton("Cancel");
	hbox->addWidget(button);
	connect(button, SIGNAL(clicked()), this, SLOT(reject()));
	button = new QPushButton("Ok");
	hbox->addWidget(button);
	connect(button, SIGNAL(clicked()), this, SLOT(accept()));
	vbox->addLayout(hbox);

	setLayout(vbox);

	// Set some defaults
	link->setText("https://explorer.lichess.ovh/lichess?variant=standard");
	rating1600->setChecked(true);
	rapid->setChecked(true);
	classical->setChecked(true);
	played->setText("100000");
	playedp->setText("20.0");
	playedpfirst->setText("3.0");
	overwright->setChecked(true);
	overwright->setDisabled(true);
}

ImportLC::~ImportLC()
{

}

void ImportLC::Import(QWidget* parent, PolyglotBook* bk)
{
	progress = new ImportLCProgress(parent);
	progress->show();
	book = bk;
//	QString fullLink= "https://explorer.lichess.ovh/lichess?variant=standard&speeds[]=blitz&speeds[]=rapid&speeds[]=classical&ratings[]=2200&ratings[]=2500&fen=rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR%20w%20KQkq%20-%200%201";
	QString fullLink=link->text();
	if (bullet->isChecked())
		fullLink += "&speeds[]=bullet";
	if (blitz->isChecked())
		fullLink += "&speeds[]=blitz";
	if (rapid->isChecked())
		fullLink += "&speeds[]=rapid";
	if (classical->isChecked())
		fullLink += "&speeds[]=classical";
	if (rating1600->isChecked())
		fullLink += "&ratings[]=1600";
	if (rating1800->isChecked())
		fullLink += "&ratings[]=1800";
	if (rating2000->isChecked())
		fullLink += "&ratings[]=2000";
	if (rating2200->isChecked())
		fullLink += "&ratings[]=2200";
	if (rating2500->isChecked())
		fullLink += "&ratings[]=2500";
	fullLink += "&moves=50";
	fullLink += "&topGames=0";
	fullLink += "&recentGames=0";
	fullLink += "&fen=";
	ChessBoard cb;
	cb.setStartposition();
	QEventLoop eventLoop;
	QNetworkAccessManager manager(this);
	connect(&manager, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));
	count = 0;
	importTravel(cb, eventLoop, manager, fullLink, true);
	delete progress;
}

void ImportLC::importTravel(ChessBoard& cb, QEventLoop& eventLoop, QNetworkAccessManager& manager, QString& fullLink, bool first)
{
	int i,j,w,d,b;
	char sz[16];
	ChessMove m;
	QNetworkReply* reply;

	emit newPosition(cb);
	QApplication::processEvents();

	// Been here before
	if (book->exist(cb))
		return;

	MoveList* ml = new MoveList;

	int games = played->text().toInt();
	if (games < 1)
		games = 10000;
	QString fen = cb.getFen().c_str();
	fen.replace(" ", "%20");
	QUrl url(fullLink+fen);
	_sleep(1000);
	QNetworkRequest req(url);
	reply = manager.get(req);
	eventLoop.exec();

	QApplication::processEvents();
	if (progress->abort)
	{
		delete ml;
		return;
	}

	if (reply->error())
	{
		w = 0;
		for (i = 1; i < 10; i++)
		{
			if (i == 9)
			{
				progress->status->setText("Giving up");
				QApplication::processEvents();
				progress->abort = true;
				delete ml;
				return;
			}
			w += 2000;
			qDebug("Internet error: %s ( waiting %i sec.)", reply->errorString().toLatin1(), w/1000);
			progress->status->setText("Internet error, trying again");
			QApplication::processEvents();
			if (progress->abort)
			{
				delete ml;
				return;
			}
			_sleep(w);
			QApplication::processEvents();
			if (progress->abort)
			{
				delete ml;
				return;
			}
			reply = manager.get(req);
			eventLoop.exec();
			if (!reply->error())
				break;
		}
	}
	progress->status->setText("Internet OK, continue");
	QApplication::processEvents();
	if (progress->abort)
	{
		delete ml;
		return;
	}
	QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
	QJsonObject obj = doc.object();
	QJsonArray jmoves = obj["moves"].toArray();
	if (jmoves.size() < 1)
	{
		delete ml;
		return;
	}
	for (i = 0; i < jmoves.size(); i++)
	{
		QJsonObject jmove = jmoves[i].toObject();
		if (jmove["san"].isString())
		{
			m = cb.getMoveFromText(jmove["san"].toString().toStdString());
			if (!m.empty())
			{
				w = jmove["white"].toInt();
				d = jmove["draws"].toInt();
				b = jmove["black"].toInt();
				m.score = (w + d + b) / games;
				if (m.score > 0)
					ml->push_back(m);
			}
		}
	}
	if (ml->size() < 1)
	{
		delete ml;
		return;
	}

	double gamesp;
	if (first)
		gamesp = playedpfirst->text().toDouble();
	else
		gamesp = playedp->text().toDouble();

	if (gamesp > 0.0)
	{
		games = ml[0].at(0).score * (gamesp/100);
		j = ml->size();
		for (i = 0; i < j; i++)
		{
			if (ml->at(i).score < games)
			{
				ml->erase(i, -1);
				break;
			}
		}
	}

	// Normalize score
	games = ml->last().score;
	for (i = 0; i < ml->size(); i++)
	{
		ml->at(i).score /= games;
		if (ml->at(i).score > 0xffff)
			ml->at(i).score = 0xffff;
	}

	reply->deleteLater();
	if (first && cb.toMove == BLACK)
		first = false;

	for (i = 0; i < ml->size(); i++)
	{
		m = ml->at(i);
		book->add(cb, m, (unsigned short)m.score, 0);
		++count;
		progress->count->setText(itoa(count, sz, 10));
		QApplication::processEvents();
		if (progress->abort)
			return;

		if (mgen.doLegalMove(cb, m))
		{
			importTravel(cb, eventLoop, manager, fullLink, first);
			mgen.undoMove(cb, m);
			if (progress->abort)
			{
				delete ml;
				return;
			}
		}
		else
		{
			qDebug("Illegal move: %s", jmoves[i].toString());
			qDebug("Fen: %s", cb.getFen().c_str());
		}
	}
	delete ml;
}

ImportLCProgress::ImportLCProgress(QWidget* parent)
	:QDialog(parent)
{
	QFont f = font();
	f.setPointSize(10);
	setFont(f);

	QVBoxLayout* vbox;
	QHBoxLayout* hbox;
	QPushButton* button;
	QLabel* label;
	vbox = new QVBoxLayout;
	setWindowTitle("Importing from OpeningExplorer");

	hbox = new QHBoxLayout;
	label = new QLabel("Positions:");
	count = new QLabel;
	hbox->addWidget(label);
	hbox->addWidget(count);
	vbox->addLayout(hbox);

	hbox = new QHBoxLayout;
	label = new QLabel("Status:");
	status = new QLabel;
	hbox->addWidget(label);
	hbox->addWidget(status);
	vbox->addLayout(hbox);

	hbox = new QHBoxLayout;
	button = new QPushButton("Cancel");
	hbox->addWidget(button);
	connect(button, SIGNAL(clicked()), this, SLOT(aborting()));
	vbox->addLayout(hbox);

	setLayout(vbox);
	abort = false;
}

void ImportLCProgress::aborting()
{
	abort = true;
}