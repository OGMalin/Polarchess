#pragma once

#include <windows.h>
#include <string>
#include "../Common/WinFile.h"
#include "../Common/ChessGame.h"

class Pgn
{
protected:
  enum { 
      PGN_unknown,      // Unknown or none token.
      PGN_string,       // String token ("abc")
      PGN_integer,      // Integer token
      PGN_period,       // Period token.
      PGN_termination,  // Termination def.= *, 1-0, 0-1, 1/2-1/2.
      PGN_tag,          // Unknown TAG. ("[xxx]")
      PGN_event,        // Event tag.
      PGN_site,         // Site tag.
      PGN_date,         // Date tag.
      PGN_round,        // Round tag.
      PGN_white,        // White tag.
      PGN_black,        // Black tag.
      PGN_result,       // Result tag.
      PGN_setup,        // Setup tag.
      PGN_fen,          // Start position if Setup==1
      PGN_eco,          // ECO tag
      PGN_opening1,     // Opening tag
      PGN_opening2,     // Variation tag
      PGN_opening3,     // SubVariation tag
      PGN_whiteelo,
      PGN_blackelo,
      PGN_remark,
      PGN_annotator,
      PGN_variation,    // Variation ("(xxx)").
      PGN_bracket,      // Bracked ("<xxx>").
      PGN_nag,          // NAG ("$")
      PGN_comment,      // Comment. ("{vbvbbvb}")
      PGN_symbol,       // Symbol as not could get into PGN_move eg. +#
      PGN_shortcomment, // ! !?
      PGN_move,         // Moves
      PGN_eof           // End of string.
  };
  int nextHeaderToken(std::string& s, std::string& value,int& index);
  int nextToken(std::string& s, std::string& value,int& index);
  int tagValue(std::string& value);
  void stringValue(std::string& value);
  void getValue(std::string&s, std::string& value, char start, char end, int& index);
  void ravValue(std::string& s, std::string& value, int& index);
  void integerValue(std::string& s, std::string& value, int& index);
  bool isSymbol(char c);
  bool isMove(std::string& s, int& index);
  void interpretHeader(std::string sHeader, ChessGame& game);
  void interpretGame(std::string sGame, ChessGame& game, int maxmove, int movenumber, const std::string& termination, bool expandnag);
  bool moveComment(std::string& s);
  int  convertNag(std::string& s);
  void expandNags(std::string& s);
  DWORD nextGame;
public:
  WinFile file;
  Pgn();
  virtual ~Pgn();
  virtual bool open(const std::string& fname, bool readonly);
  virtual void close();
  virtual bool read(ChessGame& game, DWORD index, int maxmove=999, bool expandnag=true, const char* sz=NULL, bool onlyheader=false);
  virtual std::string toString(ChessGame game, bool strict=false);
  virtual void getMovetext(std::string& movetext, ChessGame& g, int movenumber=1);
  virtual int  size();
  virtual bool deleteGame(DWORD index);
  virtual bool replaceGame(DWORD index, ChessGame& game);
  virtual bool appendGame(ChessGame& game);
};

