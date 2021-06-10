#include "Player.h"
#include <QSettings>
#include <QDialog>
#include <QInputDialog>
#include <QWidget>

Player::Player(const QString playername)
{
	isDirty = false;
	clear();
	sName = playername;
	load();
}

Player::~Player()
{
	save();
}

void Player::clear()
{
	sName.clear();
	nBulleElo = 0;
	nBulleGames = 0;
	nBlitzElo = 0;
	nBlitzGames = 0;
	nRapidElo = 0;
	nRapidGames = 0;
	nClassicalElo = 0;
	nClassicalGames = 0;
}

bool Player::save()
{
	if (!isDirty)
		return true;
	if (sName.isEmpty())
		return false;
	QSettings settings;
	settings.beginGroup("players");
	settings.beginWriteArray(sName);
	settings.setValue("BulleElo", nBulleElo);
	settings.setValue("BulleGames", nBlitzGames);
	settings.setValue("BlitzElo", nBlitzElo);
	settings.setValue("BlitzGames", nBlitzGames);
	settings.setValue("RapidElo", nRapidElo);
	settings.setValue("RapidGames", nRapidGames);
	settings.setValue("ClassicalElo", nClassicalElo);
	settings.setValue("ClassicalGames", nClassicalGames);
	settings.endArray();
	settings.endGroup();
	isDirty = false;
	return true;
}

bool Player::load(const QString playername)
{
	if (playername.isEmpty())
	{
		if (sName.isEmpty())
			return false;
	}else
	{
		sName = playername;
	}
	QSettings settings;
	settings.beginGroup("players");
	if (settings.beginReadArray(sName) < 1)
		return false;
	nBulleElo = settings.value("BulletElo", 0).toInt();
	nBulleGames =settings.value("BulleGames", 0).toInt();
	nBlitzElo = settings.value("BlitzElo", 0).toInt();
	nBlitzGames = settings.value("BlitzGames", 0).toInt();
	nRapidElo = settings.value("RapidElo", 0).toInt();
	nRapidGames = settings.value("RapidGames", 0).toInt();
	nClassicalElo = settings.value("ClassicalElo", 0).toInt();
	nClassicalGames = settings.value("ClassicalGames", 0).toInt();
	settings.endArray();
	settings.endGroup();
	isDirty = false;
	return true;
}

void Player::newPlayer(QWidget* parent)
{
	QInputDialog input(parent);
	input.setWindowTitle(tr("Add new player"));
	input.setCancelButtonText(tr("Cancel"));
	input.setInputMode(QInputDialog::TextInput);
	input.setLabelText(tr("New name"));
	input.resize(input.minimumSizeHint());
	if (input.exec() == QDialog::Rejected)
		return;
	QString p = input.textValue();
	if (p.isEmpty())
		return;
	if (load(p))
		return;
	clear();
	name(p);
}

const QStringList Player::getAll()
{
	QSettings settings;
	settings.beginGroup("players");
	QStringList list = settings.childGroups();
	settings.endGroup();
	return list;
}

const void Player::remove(const QString playername)
{
	if (playername.isEmpty())
		return;
	QString name = playername;
	QSettings settings;
	settings.beginGroup("players");
	settings.remove(name);
	settings.endGroup();
}