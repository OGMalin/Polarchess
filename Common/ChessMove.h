#pragma once

#include "defs.h"

class ChessMove
{
public:
  typeSquare fromSquare;
  typeSquare toSquare;
  unsigned char moveType;
  typePiece promotePiece;
  typePiece capturedpiece;
  typeCastle oldCastle;
  typeSquare oldEnPassant;
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


