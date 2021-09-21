#include "Book.h"

Book::Book()
	: PolyglotBook()
{

}

Book::~Book()
{
	close();
}