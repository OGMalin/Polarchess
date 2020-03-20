#pragma once

#include "../Common/ChessBoard.h"
#include "../Common/ChessMove.h"
#include "../Common/MoveList.h"

// There is two method to find the knight square
// Both makeKnightMoves and makeNoSlideMoves. Therefore
// this two array of knight moves. One will be removed when
// I have tested speed/size.
static typeSquare knightMoves[0x88][8];

class MoveGenerator // : public BasicBoard
{
protected:
  ChessMove testMove;
  virtual void addPawnMoves(ChessBoard& b, MoveList& ml, typeSquare sq);
  virtual void addPawnPromote(ChessBoard& b, MoveList& ml, ChessMove& m);
  virtual void addNoSlideMoves(ChessBoard& b, MoveList& ml, typeSquare sq, const int* sqadd);
  virtual void addSlideMoves(ChessBoard& b, MoveList& ml, typeSquare sq, const int* sqadd);
  virtual void addKingMoves(ChessBoard& b, MoveList& ml, typeSquare sq);
  virtual void addKnightMoves(ChessBoard& b, MoveList& ml, typeSquare sq);
  virtual void addPawnCaptureMoves(ChessBoard& b, MoveList& ml, typeSquare sq);
  virtual void addNoSlideCaptureMoves(ChessBoard& b, MoveList& ml, typeSquare sq, const int* sqadd);
  virtual void addSlideCaptureMoves(ChessBoard& b, MoveList& ml, typeSquare sq, const int* sqadd);
  virtual void addKnightCaptureMoves(ChessBoard& b, MoveList& ml, typeSquare sq);
  // color is color to do the attack
  virtual bool squareAttacked(ChessBoard& b, typeSquare sq, typeColor color);
  // Initiate the movegeneration
  virtual void init();
public:
  MoveGenerator();
  virtual ~MoveGenerator();
  // color= color to test for
  virtual bool inCheck(ChessBoard& b, typeColor color);
  // Makes all moves without checking
  virtual void makeAllMoves(ChessBoard& b, MoveList& ml);
  // Makes all legal moves
  virtual void makeMoves(ChessBoard& b, MoveList& ml);
  // Make all capture and promote moves without checking
  virtual void makeAllCaptureMoves(ChessBoard& b, MoveList& ml);
  // Make all legal capture and promote moves
  virtual void makeCaptureMoves(ChessBoard& b, MoveList& ml);
  // Do/undo a move without checking
  virtual void doMove(ChessBoard& b, ChessMove& m);
  virtual void undoMove(ChessBoard& b, ChessMove& m);
  virtual void doNullMove(ChessBoard& b, ChessMove& m);
  virtual void undoNullMove(ChessBoard& b, ChessMove& m);
  // Do/undo a move with checking for legality
  virtual bool isLegal(ChessBoard&, ChessMove& m);
  virtual bool doLegalMove(ChessBoard&, ChessMove& m);
};

