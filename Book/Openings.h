#pragma once

#include "DatabaseFormat.h"
#include <QString>

class Openings
{
public:
	Openings();
	~Openings();

	// Open a database
	bool open(const QString& path);

	// Create a database
	bool create(const QString& path);

	// Close the database
	void close();

	// Find a position
	OpeningsDBEntry find(ChessBoard&);

	// Add an engine analyze to a position
	void add(OpeningsDBEntry&, ChessBoard&);

	// Get db path
	QString getPath();

private:
	bool _opened;
	OpeningsDBInfo odi;
};