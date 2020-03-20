#include <cstdlib>
#include "../Common/MoveGenerator.h"
#include "../Common/Relations.h"

static bool initMoveGen=0;

MoveGenerator::MoveGenerator() 
{
  if (!initMoveGen)
    init();
};

MoveGenerator::~MoveGenerator()
{
};

void MoveGenerator::init()
{
  int i,j;
  typeSquare sq;
  // Generate all knightmoves;
  testMove.clear();
  for (sq=0;sq<128;sq++)
    for (i=0;i<8;i++)
      knightMoves[sq][i]=UNDEF;
  sq=0;
  while(1)
  {
    j=0;
    for (i=0;i<8;i++)
    {
      if (LEGALSQUARE(sq+knightPath[i]))
        knightMoves[sq][j++]=sq+knightPath[i];
    }
    sq++;
    if (sq&8)
    {
      sq+=8;
      if (sq>127)
        break;
    }
  }
}

void MoveGenerator::makeAllCaptureMoves(ChessBoard& b, MoveList& ml)
{
  typeSquare sq=0;
  ml.clear();
  typeColor player=b.toMove;
  // Add all possible capture moves without testing for check
  while(1)
  {
    typePiece piece=b.board[sq];
    if (PIECECOLOR(piece)==player)
    {
      switch (PIECE(piece))
      {
        case PAWN:
          addPawnCaptureMoves(b,ml,sq);
          break;
        case KNIGHT:
//          addNoSlideCaptureMoves(b,ml,sq,knightPath);
          addKnightCaptureMoves(b,ml,sq);
          break;
        case BISHOP:
          addSlideCaptureMoves(b,ml,sq,bishopPath);
          break;
        case ROOK:
          addSlideCaptureMoves(b,ml,sq,rookPath);
          break;
        case QUEEN:
          addSlideCaptureMoves(b,ml,sq,bishopPath);
          addSlideCaptureMoves(b,ml,sq,rookPath);
          break;
        case KING:
          addNoSlideCaptureMoves(b,ml,sq,kingPath);
          break;
      }
    }
    sq++;
    if (sq&8)
    {
      sq+=8;
      if (sq>127)
        break;
    }
  }
}

void MoveGenerator::makeCaptureMoves(ChessBoard& b, MoveList& ml)
{

  typeColor player=b.toMove;
  int other=OTHERPLAYER(player);
  #ifdef _DEBUG_MOVEGEN
    ChessBoard dbgB;
    dbgB.copy(b);
  #endif
  makeAllCaptureMoves(b,ml);
 
  typePiece p=COLORPIECE(player,KING);
  typeSquare sq=0,nsq;
  while(1)
  {
    if (b.board[sq]==p)
      break;
    sq++;
    if (sq&8)
    {
      sq+=8;
      if (sq>127)
        return;
    }
  }

  int it=0;
  
  while (it<ml.size())
  {
    if (b.board[ml[it].fromSquare]==p)
      nsq=ml[it].toSquare;
    else
      nsq=sq;
    doMove(b,ml[it]);
    if (squareAttacked(b,nsq,other))
    {
      undoMove(b,ml[it]);
      ml[it].clear();
    }else
    {
      undoMove(b,ml[it]);
    }
    ++it;
  };

  ml.trunc();
}

void MoveGenerator::makeAllMoves(ChessBoard& b, MoveList& ml)
{
  typeSquare sq=0;
  ml.clear();
  typeColor player=b.toMove;

  // Add all possible moves without testing for check
  while (1)
  {
    typePiece piece=b.board[sq];
    if (PIECECOLOR(piece)==player)
    {
      switch (PIECE(piece))
      {
        case PAWN:
          addPawnMoves(b,ml,sq);
          break;
        case KNIGHT:
          addKnightMoves(b,ml,sq);
          break;
        case BISHOP:
          addSlideMoves(b,ml,sq,bishopPath);
          break;
        case ROOK:
          addSlideMoves(b,ml,sq,rookPath);
          break;
        case QUEEN:
          addSlideMoves(b,ml,sq,bishopPath);
          addSlideMoves(b,ml,sq,rookPath);
          break;
        case KING:
          addKingMoves(b,ml,sq);
          break;
      }
    }
    sq++;
    if (sq&8)
    {
      sq+=8;
      if (sq>127)
        return;
    }
  }
}

void MoveGenerator::makeMoves(ChessBoard& b, MoveList& ml)
{
  typeColor player=b.toMove;
  typeColor other=OTHERPLAYER(player);

  // Make all moves (both legal and illegal)
  makeAllMoves(b,ml);

  // Find the king  
  typePiece p=COLORPIECE(player,KING);
  typeSquare sq=0,nsq;
  while (1)
  {
    if (b.board[sq]==p)
      break;
    sq++;
    if (sq&8)
    {
      sq+=8;
      if (sq>127)
        return;
    }
  }

  int it=0;
  while (it<ml.size())
  {
    if (ml[it].moveType&CASTLE)
    {
      if (squareAttacked(b, ml[it].fromSquare,other))
      {
        ml[it].clear();
        ++it;
        continue;
      }
      if (squareAttacked(b, __max(ml[it].fromSquare,ml[it].toSquare)-1,other))
      {
        ml[it].clear();
        ++it;
        continue;
      }
    }
    if (b.board[ml[it].fromSquare]==p)
      nsq=ml[it].toSquare;
    else
      nsq=sq;
    doMove(b,ml[it]);
    if (squareAttacked(b,nsq,other))
    {
      undoMove(b,ml[it]);
      ml[it].clear();
    }else
    {
      undoMove(b,ml[it]);
    }
    ++it;
  };

  ml.trunc();
  return ;
}

// Is a move legal
bool MoveGenerator::isLegal(ChessBoard& b, ChessMove& m)
{
  typeColor color=b.toMove;

  if (m.empty())
    return false;
  if (m.moveType&CASTLE)
  {
    if (squareAttacked(b, m.fromSquare,OTHERPLAYER(b.toMove)))
      return false;
    if (squareAttacked(b, __max(m.fromSquare,m.toSquare)-1,OTHERPLAYER(b.toMove)))
      return false;
  }
  doMove(b,m);
  if (inCheck(b,color))
  {
    undoMove(b,m);
    return false;
  }
  undoMove(b,m);
  return true;
}

bool MoveGenerator::doLegalMove(ChessBoard& b, ChessMove& m)
{
  typeColor color=b.toMove;

  if (m.moveType&CASTLE)
  {
    if (squareAttacked(b, m.fromSquare,OTHERPLAYER(b.toMove)))
      return false;
    if (squareAttacked(b, __max(m.fromSquare,m.toSquare)-1,OTHERPLAYER(b.toMove)))
      return false;
  }
  doMove(b,m);
  if (inCheck(b,color))
  {
    undoMove(b,m);
    return false;
  }
  return true;
}

// This function check if a square is attacked by a pieces.
// Instead of making all moves and see if one of the pieces hit this square
// I'm puting a piece on the square in see if it hit it self.
bool MoveGenerator::squareAttacked(ChessBoard& b, typeSquare sq, typeColor color)
{
  int i,testSquare;
  typePiece pawn=COLORPIECE(color,PAWN);
  typePiece knight=COLORPIECE(color,KNIGHT);
  typePiece king=COLORPIECE(color,KING);
  typePiece rook=COLORPIECE(color,ROOK);
  typePiece bishop=COLORPIECE(color,BISHOP);
  typePiece queen=COLORPIECE(color,QUEEN);

  // Attacked from diagonals
  for (i=0;i<4;i++)
  {
    testSquare=sq+bishopPath[i];
    while (LEGALSQUARE(testSquare))
    {
      if (b.board[testSquare]!=EMPTY)
      {
        if ((b.board[testSquare]==bishop) || (b.board[testSquare]==queen))
          return true;
        break;
      }
      testSquare+=bishopPath[i];
    }
  }

  // Attacked from rank and files
  for (i=0;i<4;i++)
  {
    testSquare=sq+rookPath[i];
    while (LEGALSQUARE(testSquare))
    {
      if (b.board[testSquare]!=EMPTY)
      {
        if ((b.board[testSquare]==rook) || (b.board[testSquare]==queen))
          return true;
        break;
      }
      testSquare+=rookPath[i];
    }
  }

  // Attacked from knights or king
  for (i=0;i<8;i++)
  {
    testSquare=sq+knightPath[i];
    if (LEGALSQUARE(testSquare))
      if (b.board[testSquare]==knight)
        return true;
    testSquare=sq+kingPath[i];
    if (LEGALSQUARE(testSquare))
      if (b.board[testSquare]==king)
        return true;
  }

  // Attacked from pawns
  testSquare=sq-1+(color?16:-16);
  for (i=0;i<2;i++)
  {
    if (LEGALSQUARE(testSquare))
      if (b.board[testSquare]==pawn)
        return true;
    testSquare+=2;
  }
  return false;
}

bool MoveGenerator::inCheck(ChessBoard& b, typeColor color)
{
  typePiece p=COLORPIECE(color,KING);
  typeSquare sq=0;
  while (1)
  {
    if (b.board[sq]==p)
      break;
    sq++;
    if (sq&8)
    {
      sq+=8;
      if (sq>127)
        return false;
    }
  }
  return squareAttacked(b,sq,OTHERPLAYER(color));
}

void MoveGenerator::addPawnMoves(ChessBoard& b, MoveList& ml, typeSquare sq)
{
  int pawnRow,i,capfile;
  pawnRow=(b.toMove)?-16:16;

  // One square forward;
  testMove.clear();
  testMove.fromSquare=sq;
	testMove.toSquare=testMove.fromSquare+pawnRow;
  if (b.board[testMove.toSquare]==EMPTY)
  {
    if ((testMove.toSquare<a2)||(testMove.toSquare>h7))
    {
      testMove.moveType=PAWNMOVE|PROMOTE;
      addPawnPromote(b,ml,testMove);
    }else
    {
      testMove.moveType=PAWNMOVE;
      ml.push_back(testMove);
    }
  }

  // Capture
  testMove.clear();
  testMove.fromSquare=sq;
  capfile=-1; // Look first to left
  for (i=0;i<2;i++)
  {
    testMove.toSquare=sq+capfile+pawnRow;
    if (LEGALSQUARE(testMove.toSquare))
    {
      if ((b.board[testMove.toSquare]!=EMPTY) && 
          (PIECECOLOR(b.board[testMove.toSquare])!=b.toMove))
			{
        testMove.capturedpiece=b.board[testMove.toSquare];
        if ((testMove.toSquare<a2)||(testMove.toSquare>h7))
        {
          testMove.moveType=PAWNMOVE|CAPTURE|PROMOTE;
          addPawnPromote(b,ml,testMove);
        }else
        {
          testMove.moveType=PAWNMOVE|CAPTURE;
          ml.push_back(testMove);
        }
      }else if (testMove.toSquare==b.enPassant)
      {
        testMove.moveType=PAWNMOVE|CAPTURE|ENPASSANT;
        if (b.toMove==WHITE)
          testMove.capturedpiece=blackpawn;
        else
          testMove.capturedpiece=whitepawn;
        ml.push_back(testMove);
      }
    }
	  capfile=1;  // Look to the right
  }

// Two square forward
  if (((RANK(sq)==1)&&(b.toMove==WHITE)) || 
      ((RANK(sq)==6)&&(b.toMove==BLACK)))
  {
    testMove.clear();
    testMove.fromSquare=sq;
    testMove.moveType=PAWNMOVE|DBLPAWNMOVE;
    if ((b.board[sq+pawnRow]==EMPTY) && 
        (b.board[sq+pawnRow*2]==EMPTY))
    {
  	  testMove.toSquare=testMove.fromSquare+pawnRow*2;
      ml.push_back(testMove);
    }
  }
}

void MoveGenerator::addPawnPromote(ChessBoard& b, MoveList& ml, ChessMove& m)
{
  m.promotePiece=COLORPIECE(b.toMove,QUEEN);
  ml.push_back(m);
  m.promotePiece=COLORPIECE(b.toMove,ROOK);
  ml.push_back(m);
  m.promotePiece=COLORPIECE(b.toMove,BISHOP);
  ml.push_back(m);
  m.promotePiece=COLORPIECE(b.toMove,KNIGHT);
  ml.push_back(m);
};

// Knight and king
void MoveGenerator::addNoSlideMoves(ChessBoard& b, MoveList& ml, typeSquare sq, const int* sqadd)
{
  int i;
  testMove.clear();
  testMove.fromSquare=sq;
  for (i=0;i<8;i++)
  {
    testMove.toSquare=sq+sqadd[i];
    if (LEGALSQUARE(testMove.toSquare))
    {
      if (b.board[testMove.toSquare]==EMPTY)
      {
        ml.push_back(testMove);
      }else if (PIECECOLOR(b.board[testMove.toSquare])!=b.toMove)
      {
        testMove.moveType=CAPTURE;
        testMove.capturedpiece=b.board[testMove.toSquare];
        ml.push_back(testMove);
        testMove.moveType=0;
        testMove.capturedpiece=0;
      }
    }
  }
}

// Queen, bishop and rook
void MoveGenerator::addSlideMoves(ChessBoard& b, MoveList& ml, typeSquare sq, const int* sqadd)
{
  int i;
  testMove.clear();
  testMove.fromSquare=sq;
  for (i=0;i<4;i++)
  {
    testMove.toSquare=sq+sqadd[i];
    while (LEGALSQUARE(testMove.toSquare))
    {
      if (b.board[testMove.toSquare]==EMPTY)
      {
        ml.push_back(testMove);
      }else if (PIECECOLOR(b.board[testMove.toSquare])!=b.toMove)
      {
        testMove.moveType=CAPTURE;
        testMove.capturedpiece=b.board[testMove.toSquare];
        ml.push_back(testMove);
        testMove.moveType=0;
        testMove.capturedpiece=0;
        break;
      }else
      {
        break;
      }
      testMove.toSquare+=sqadd[i];
    }
  }
}

void MoveGenerator::addKingMoves(ChessBoard& b, MoveList& ml, typeSquare sq)
{
  typeCastle ctl;
  addNoSlideMoves(b,ml,sq,kingPath);

  // castle
  testMove.clear();
  testMove.fromSquare=sq;
  testMove.moveType=CASTLE;
  // Make black and white castle rights look the same.
  ctl=(b.toMove==WHITE)?b.castle:b.castle>>2;
  if (ctl&whitekingsidecastle)
  {
    if ((b.board[sq+1]==EMPTY) && 
        (b.board[sq+2]==EMPTY))
    {
      testMove.toSquare=sq+2;
      ml.push_back(testMove);
    }
  }
  if (ctl&whitequeensidecastle)
  {
    if ((b.board[sq-1]==EMPTY) && 
        (b.board[sq-2]==EMPTY) && 
        (b.board[sq-3]==EMPTY))
    {
      testMove.toSquare=sq-2;
      ml.push_back(testMove);
    }
  }
}

void MoveGenerator::addKnightMoves(ChessBoard& b, MoveList& ml, typeSquare sq)
{
  int i=0;
  testMove.clear();
  testMove.fromSquare=sq;
  while (i<8)
  {
    if (knightMoves[sq][i]==UNDEF)
      break;
    testMove.toSquare=knightMoves[sq][i++];
    if (b.board[testMove.toSquare]==EMPTY)
    {
      ml.push_back(testMove);
    }else if (PIECECOLOR(b.board[testMove.toSquare])!=b.toMove)
    {
      testMove.moveType=CAPTURE;
      testMove.capturedpiece=b.board[testMove.toSquare];
      ml.push_back(testMove);
      testMove.moveType=0;
      testMove.capturedpiece=0;
    }
  }
}

void MoveGenerator::addKnightCaptureMoves(ChessBoard& b, MoveList& ml, typeSquare sq)
{
  int i=0;
  testMove.clear();
  testMove.fromSquare=sq;
  testMove.moveType=CAPTURE;
  while (i<8)
  {
    if (knightMoves[sq][i]==UNDEF)
      break;
    testMove.toSquare=knightMoves[sq][i++];
    if ((b.board[testMove.toSquare]!=EMPTY) && 
        (PIECECOLOR(b.board[testMove.toSquare])!=b.toMove))
    {
      testMove.capturedpiece=b.board[testMove.toSquare];
      ml.push_back(testMove);
    }
  }
}

void MoveGenerator::addPawnCaptureMoves(ChessBoard& b, MoveList& ml, typeSquare sq)
{
  int pawnRow,i,capfile;
  pawnRow=(b.toMove)?-16:16;
  testMove.clear();
  testMove.fromSquare=sq;

  // Promote is also change on material
	testMove.toSquare=testMove.fromSquare+pawnRow;
  if (b.board[testMove.toSquare]==EMPTY)
  {
    if ((testMove.toSquare<a2)||(testMove.toSquare>h7))
    {
      testMove.moveType=PAWNMOVE|PROMOTE;
      addPawnPromote(b,ml,testMove);
      testMove.clear();
      testMove.fromSquare=sq;
    }
  }

  capfile=-1; // Look first to left
  for (i=0;i<2;i++)
  {
    testMove.toSquare=sq+capfile+pawnRow;
    if (LEGALSQUARE(testMove.toSquare))
    {
      if ((b.board[testMove.toSquare]!=EMPTY) && 
          (PIECECOLOR(b.board[testMove.toSquare])!=b.toMove))
			{
        testMove.capturedpiece=b.board[testMove.toSquare];
        if ((testMove.toSquare<a2)||(testMove.toSquare>h7))
        {
          testMove.moveType=PAWNMOVE|CAPTURE|PROMOTE;
          addPawnPromote(b,ml,testMove);
        }else
        {
          testMove.moveType=PAWNMOVE|CAPTURE;
          ml.push_back(testMove);
        }
      }else if (testMove.toSquare==b.enPassant)
      {
        testMove.moveType=PAWNMOVE|CAPTURE|ENPASSANT;
        if (b.toMove==WHITE)
          testMove.capturedpiece=blackpawn;
        else
          testMove.capturedpiece=whitepawn;
        ml.push_back(testMove);
      }
    }
	  capfile=1;  // Look to the right
  }
}

// Knight and king
void MoveGenerator::addNoSlideCaptureMoves(ChessBoard& b, MoveList& ml, typeSquare sq, const int* sqadd)
{
  int i;
  testMove.clear();
  testMove.fromSquare=sq;
  testMove.moveType=CAPTURE;
  for (i=0;i<8;i++)
  {
    testMove.toSquare=sq+sqadd[i];
    if (LEGALSQUARE(testMove.toSquare))
    {
      if ((b.board[testMove.toSquare]!=EMPTY) && 
          (PIECECOLOR(b.board[testMove.toSquare])!=b.toMove))
      {
        testMove.capturedpiece=b.board[testMove.toSquare];
        ml.push_back(testMove);
      }
    }
  }
}

// Queen, bishop and rook capture
void MoveGenerator::addSlideCaptureMoves(ChessBoard& b, MoveList& ml, typeSquare sq, const int* sqadd)
{
  int i;
  testMove.clear();
  testMove.fromSquare=sq;
  testMove.moveType=CAPTURE;
  for (i=0;i<4;i++)
  {
    testMove.toSquare=sq+sqadd[i];
    while (LEGALSQUARE(testMove.toSquare))
    {
      if (b.board[testMove.toSquare]!=EMPTY)
      {
        if  (PIECECOLOR(b.board[testMove.toSquare])!=b.toMove)
        {
          testMove.capturedpiece=b.board[testMove.toSquare];
          ml.push_back(testMove);
        }
        break;
      }
      testMove.toSquare+=sqadd[i];
    }
  }
}

void MoveGenerator::undoMove(ChessBoard& b, ChessMove& m)
{
  // Move back the piece
  b.board[m.fromSquare]=b.board[m.toSquare];
  // If it was a capture set the capture piece back
  if (m.moveType&CAPTURE)
  {
    if (m.moveType&ENPASSANT)
    {
      if (b.toMove==BLACK)
        b.board[m.toSquare-16]=blackpawn;
      else
        b.board[m.toSquare+16]=whitepawn;
      b.board[m.toSquare]=EMPTY;
    }else
    {
      b.board[m.toSquare]=m.capturedpiece;
    }
  }else
  {
    b.board[m.toSquare]=EMPTY;
  }
  // Was it a promotion
  if (m.moveType&PROMOTE)
  {
    if (b.toMove==WHITE)
      b.board[m.fromSquare]=blackpawn;
    else
      b.board[m.fromSquare]=whitepawn;
  }
  // Set back the rook on castle
  if (m.moveType&CASTLE)
  {
    switch (m.toSquare)
    {
      case c1:
        b.board[d1]=EMPTY;
        b.board[a1]=whiterook;
        break;
      case g1:
        b.board[f1]=EMPTY;
        b.board[h1]=whiterook;
        break;
      case c8:
        b.board[d8]=EMPTY;
        b.board[a8]=blackrook;
        break;
      case g8:
        b.board[f8]=EMPTY;
        b.board[h8]=blackrook;
        break;
    }
  }
  // Restore board condition
  b.enPassant=m.oldEnPassant;
  b.castle=m.oldCastle;
  b.move50draw=m.oldMove50draw;
  // Give back the move
  b.toMove=OTHERPLAYER(b.toMove);
}

void MoveGenerator::doMove(ChessBoard& b, ChessMove& m)
{
  int pawnRow;

  // Housekeeping for undoMove
  m.oldCastle=b.castle;
  m.oldEnPassant=b.enPassant;
  m.oldMove50draw=b.move50draw;

  // Move piece
  if (m.moveType&PROMOTE)
    b.board[m.toSquare]=m.promotePiece;
  else
    b.board[m.toSquare]=b.board[m.fromSquare];
  b.board[m.fromSquare]=EMPTY;

  // Remove pawn on EnPassant move
  pawnRow=(b.toMove)?-16:16;
  if (m.moveType&ENPASSANT)
    b.board[m.toSquare-pawnRow]=EMPTY;

  // Set new ep for doble pawnmoves. (only if possible)
  if (m.moveType&DBLPAWNMOVE)
  {
    if (LEGALSQUARE(m.toSquare-1) &&
      (b.board[m.toSquare-1]==COLORPIECE(OTHERPLAYER(b.toMove),PAWN)))
      b.enPassant=m.toSquare-pawnRow;
    else if ((LEGALSQUARE(m.toSquare+1)) &&
       (b.board[m.toSquare+1]==COLORPIECE(OTHERPLAYER(b.toMove),PAWN)))
      b.enPassant=m.toSquare-pawnRow;
    else
      b.enPassant=UNDEF;
  }else
  {
    b.enPassant=UNDEF;
  }
  // Move rook on castle
  if (m.moveType&CASTLE)
  {
    switch (m.toSquare)
    {
      case c1:  // White queenside castle
        b.board[a1]=EMPTY;
        b.board[d1]=whiterook;
        break;
      case g1:  // White kingside castle
        b.board[h1]=EMPTY;
        b.board[f1]=whiterook;
        break;
      case c8:  // Black queenside castle
        b.board[a8]=EMPTY;
        b.board[d8]=blackrook;
        break;
      case g8:  // Black kingside castle
        b.board[h8]=EMPTY;
        b.board[f8]=blackrook;
        break;
    }
  }

  // Fifty moves rule
  if ((m.moveType&(PAWNMOVE|CAPTURE))==0)
    b.move50draw++;
  else
    b.move50draw=0;

  // Set new castle rights
  if (b.board[m.toSquare]==whiteking) // White king has moved
    b.castle&= ~(whitekingsidecastle|whitequeensidecastle);
  if (b.board[m.toSquare]==blackking) // Black king has moved
    b.castle&= ~(blackkingsidecastle|blackqueensidecastle);

  if (b.castle&whitequeensidecastle)
    if ((m.fromSquare==a1) || // a1 rook has moved or captured
        (m.toSquare==a1))
      b.castle&= ~whitequeensidecastle;

  if (b.castle&whitekingsidecastle) 
    if ((m.fromSquare==h1) || // h1 rook has moved or captured
        (m.toSquare==h1))
      b.castle&= ~whitekingsidecastle;

  if (b.castle&blackqueensidecastle)
    if ((m.fromSquare==a8) || // a8 rook has moved or captured
        (m.toSquare==a8)) 
      b.castle&= ~blackqueensidecastle;

  if (b.castle&blackkingsidecastle)
    if ((m.fromSquare==h8) || // h8 rook has moved or captured
        (m.toSquare==h8))
      b.castle&= ~blackkingsidecastle;

  // Hands over the move
  b.toMove=OTHERPLAYER(b.toMove);
}

void MoveGenerator::undoNullMove(ChessBoard& b, ChessMove& m)
{
  // Restore board condition
  b.enPassant=m.oldEnPassant;
  b.castle=m.oldCastle;
  b.move50draw=m.oldMove50draw;
  // Give back the move
  b.toMove=OTHERPLAYER(b.toMove);
}

void MoveGenerator::doNullMove(ChessBoard& b, ChessMove& m)
{
  // Housekeeping for undoNullMove
  m.oldCastle=b.castle;
  m.oldEnPassant=b.enPassant;
  m.oldMove50draw=b.move50draw;

  m.moveType=NULL_MOVE;

  b.move50draw++;
  b.enPassant=UNDEF;

  // Hands over the move
  b.toMove=OTHERPLAYER(b.toMove);
}



