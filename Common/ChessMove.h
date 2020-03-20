#pragma once

#include "../Common/defs.h"

class ChessMove
{
public:
  int fromSquare;
  int toSquare;
  unsigned char moveType;
  int promotePiece;
  int capturedpiece;
  int oldCastle;
  int oldEnPassant;
  int oldMove50draw;
  int score;
  ChessMove();
  ChessMove(const ChessMove& m);
  virtual ~ChessMove();
  ChessMove& operator=(const ChessMove& m);
  void clear();
  bool empty();
  friend bool operator==(const ChessMove& m1, const ChessMove& m2);
  friend bool operator!=(const ChessMove& m1, const ChessMove& m2);
};


