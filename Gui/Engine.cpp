#include "Engine.h"
#include <string>
#include <QApplication>

using namespace std;

Engine::Engine()
{
	readyok = false;
}

Engine::~Engine()
{
	unload();
}

bool Engine::load(QString& path)
{
	QMetaObject::Connection con;


	con = connect(&uci, SIGNAL(engineStarted()), SLOT(slotEngineStarted()));
	con = connect(&uci, SIGNAL(engineStoped()), SLOT(slotEngineStoped()));
	con = connect(&uci, SIGNAL(engineMove(const QString&, const QString&)), SLOT(slotEngineMove(const QString&, const QString&)));
	//	con = connect(&uci, SIGNAL(engineInfo(const EngineInfo&)), SLOT(slotEngineInfo(const EngineInfo&)));
	if (!uci.load(path))
		return false;
	return true;
}

void Engine::unload()
{
	readyok = false;
	uci.disconnect();
	uci.unload();
}

void Engine::search(ChessBoard& board, MoveList& moves, SEARCHTYPE searchtype, int wtime, int winc, int btime, int binc, int movestogo)
{
		if (!uci.isLoaded())
			return;
		QString cmd;
		QStringList list;
		//ChessBoard b;
		int i;
		currentBoard = board;
		for (i = 0; i < moves.size(); i++)
		{
			list.append(currentBoard.makeMoveText(moves.at(i),UCI).c_str());
			if (!currentBoard.doMove(moves.at(i), true))
				break;
		}
		string s=board.getFen();
		cmd = "position";

		if (board.isStartposition())
			cmd += " startpos";
		else
			cmd += QString(" fen ") + board.getFen(true).c_str();
		if (list.size())
		{
			cmd += " moves";
			QStringList::iterator lit = list.begin();
			while (lit != list.end())
			{
				cmd += " ";
				cmd += *lit;
				++lit;
			}
		}
		// If engine playing white it is most probaly not ready yet.
		cmd += "\n";
		if (readyok)
			uci.write(cmd);
		else
			waitCommand += cmd;
		cmd="go";
		if (searchtype == PONDER_SEARCH)
			cmd += " ponder";
		if (searchtype == INFINITE_SEARCH)
		{
			cmd += " infinite";
		}
		else
		{
			cmd += " wtime " + QString().setNum(wtime);
			cmd += " btime " + QString().setNum(btime);
			cmd += " winc " + QString().setNum(winc);
			cmd += " binc " + QString().setNum(binc);
		}
		if (movestogo)
			cmd += "movestogo " + QString().setNum(movestogo);
		cmd += "\n";
		if (readyok)
			uci.write(cmd);
		else
			waitCommand += cmd;
}

void Engine::slotEngineStarted()
{
	int i,j;
	personality = uci.getOption(QString("Personality"));
	skill = uci.getOption(QString("Skill"));
	autoskill = uci.getOption(QString("Auto Skill"));
	limitstrength = uci.getOption(QString("UCI_LimitStrength"));
	elo = uci.getOption(QString("UCI_Elo"));
	ownbook = uci.getOption(QString("OwnBook"));
	bookfile = uci.getOption(QString("Book File"));

	name = uci.name();
	i = name.indexOf("64");
	if (i > 0)
		name.truncate(i);
	i = name.indexOf("32");
	if (i > 0)
		name.truncate(i);
	name = name.trimmed();
	author = uci.author();

	// Try to find rating
	if (name.indexOf("Komodo") >= 0)
	{
		EngineRating er;
		if (skill.name == "Skill")
		{
			if (skill.spin.max == 25)
			{
				if (personality.name == "Personality")
				{
					for (i = 0; i < 25; i++)
					{
						for (j = 0; j < personality.combo.var.size(); j++)
						{
							er.skill = i;
							er.personality = personality.combo.var[j];
							er.rating = (er.skill + 1) * 125 + 200;
							er.category = BULLET;
							rating.push_back(er);
							er.category = BLITZ;
							er.rating -= 100;
							rating.push_back(er);
							er.category = RAPID;
							er.rating -= 100;
							rating.push_back(er);
							er.category = CLASSICAL;
							er.rating -= 100;
							rating.push_back(er);
						}
					}
				}
				else
				{
					for (i = 0; i < 25; i++)
					{
						er.skill = i;
						er.personality.clear();
						er.rating = (er.skill + 1) * 125 + 200;
						er.category = BULLET;
						rating.push_back(er);
						er.category = BLITZ;
						er.rating -= 100;
						rating.push_back(er);
						er.category = RAPID;
						er.rating -= 100;
						rating.push_back(er);
						er.category = CLASSICAL;
						er.rating -= 100;
						rating.push_back(er);
					}
				}
			}
			else if (skill.spin.max == 20)
			{
				for (i = 0; i < 20; i++)
				{
					er.skill = i;
					er.personality.clear();
					er.rating = (er.skill + 1) * 142 + 200;
					er.category = BULLET;
					rating.push_back(er);
					er.category = BLITZ;
					er.rating -= 100;
					rating.push_back(er);
					er.category = RAPID;
					er.rating -= 100;
					rating.push_back(er);
					er.category = CLASSICAL;
					er.rating -= 100;
					rating.push_back(er);
				}
			}
		}
	}

	if (!waitCommand.isEmpty())
	{
		uci.write(waitCommand);
		waitCommand.clear();
	}
	readyok = true;
	emit engineStarted();
}

void Engine::slotEngineStoped()
{
	uci.disconnect();
	emit engineStoped();
}

void Engine::slotEngineMove(const QString& smove, const QString& sponder)
{
	emit engineMove(smove, sponder);
}

QString Engine::getPath()
{
	return uci.path();
}

int Engine::getRating(int skill, QString& personality, int category)
{
	for (int i = 0; i < rating.size(); i++)
		if ((rating[i].skill == skill) && (rating[i].personality == personality) && (rating[i].category==category))
			return rating[i].rating;
	return 0;
}