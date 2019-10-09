#pragma once


/** Databaseformat

Testset
	id			integer
		id for this testgroup

	name		text
		Name of the testset

	description	text
		Short description of this test.

	type		integer
		Type of testset

Position table
	id		integer
		Id for this position.

	fen		text
		testposition

	setid	integer
		Point to test group table which this test belong.

	move	text
		List of moves whith score for position.

*/

#include <QObject>
#include <QString>

class TestSet : public QObject
{
	Q_OBJECT

public:
	TestSet(QObject *parent = 0);
	~TestSet();
	bool create(const QString& path);

private:
	bool opened;
};
