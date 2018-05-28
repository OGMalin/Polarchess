#pragma once

#include <string>
#include <vector>
#include "../Common/ChessBoard.h"
#include "../Common/ChessMove.h"
#include "../Common/MoveList.h"
#include <windows.h>

struct ChessGameMove
{
  size_t posIndex;
  ChessMove move;
  std::string comment;         
};

struct ChessGamePosition
{
  size_t fromIndex;
  ChessBoard board;
  std::string comment;
  std::vector<ChessGameMove> move;
  ChessGamePosition():fromIndex(-1){};

  size_t moveExist(ChessMove& m)
  {
    size_t i=0;
    while (i<move.size())
    {
      if (move[i].move==m)
        return i;
      i++;
    }
    return -1;
  }

  size_t moveExist(size_t n)
  {
    size_t i=0;
    while (i<move.size())
    {
      if (move[i].posIndex==n)
        return i;
      i++;
    }
    return -1;
  }
  ChessGamePosition& operator=(const ChessGamePosition& b)
  {
    unsigned int i;
    fromIndex=b.fromIndex;
    board=b.board;
    comment=b.comment;
    move.clear();
    for (i=0;i<b.move.size();i++)
      move.push_back(b.move[i]);
    return *this;
  };
};

struct ChessGameInfo
{
  std::string   Event;
  std::string   Site;
  std::string   White;
  std::string   Black;
  std::string   Round;
  std::string   Date;
  std::string   Result;
  std::string   WhiteElo;
  std::string   BlackElo;
  std::string   WhiteType;  // Human or computer
  std::string   BlackType;  // Human or computer
  std::string   TimeControl;
  std::string   ECO;
  std::string   Opening;
  std::string   Variation;
  std::string   SubVariation;
  std::string   Remark;
  std::string   Annotator;

  void clear()
  {
    Event.erase();
    Site.erase();
    White.erase();
    Black.erase();
    Round.erase();
    Date.erase();
    Result.erase();
    WhiteElo.erase();
    BlackElo.erase();
    WhiteType.erase();
    BlackType.erase();
    TimeControl.erase();
    ECO.erase();
    Opening.erase();
    Variation.erase();
    SubVariation.erase();
    Remark.erase();
    Annotator.erase();
  };

  ChessGameInfo& operator=(const ChessGameInfo& b)
  { 
    Event=b.Event;
    Site=b.Site;
    White=b.White;
    Black=b.Black;
    Round=b.Round;
    Date=b.Date;
    Result=b.Result;
    WhiteElo=b.WhiteElo;
    BlackElo=b.BlackElo;
    WhiteType=b.WhiteType;
    BlackType=b.BlackType;
    TimeControl=b.TimeControl;
    ECO=b.ECO;
    Opening=b.Opening;
    Variation=b.Variation;
    SubVariation=b.SubVariation;
    Remark=b.Remark;
    Annotator=b.Annotator;
    return *this;
  };
};

class ChessGame
{
public:
  std::vector<ChessGamePosition> position;
  ChessGameInfo info;
  ChessGame();
  virtual ~ChessGame();
  void clear(bool keepheader=false);
  void copy(const ChessGame& g);
  void getStartPosition(ChessBoard& cb);
  void setStartPosition(const ChessBoard& cb);
  void setStartPosition(const char* fen);
  bool toStart();
  bool toEnd(bool variation=false);
  bool goTo(const ChessBoard& cb, bool strict=true);
  void setDate(const SYSTEMTIME& st);
  void getDate(SYSTEMTIME& st);
  void setTimeControl(int m1,int t1,int i1=0,int m2=0,int t2=0,int i2=0,int m3=0,int t3=0,int i3=0);
  void getTimeControl(int& m1,int& t1,int& i1,int& m2,int& t2,int& i2,int& m3,int& t3,int& i3);
  // Return number of halfmoves in mainline.
  size_t  mainMoves();
  std::string toString();
  // Return a variation as a pgn string.
  std::string getVariation(int n, bool comment);
  // Return a variation as a game. Return 0 if variation doesn't exist, else 1.
  int getVariation(int n, bool comment, ChessGame& variation);
  ChessGame& operator=(const ChessGame& b)
  { copy(b); return *this;};
  // 1=3-fold draw, 2=50 move draw.
  int isDraw();
  void backup(unsigned int n);
  void forward(unsigned int n);
  // if n==-1 it return (in m) the move leading to this position.
  bool getMove(ChessMove& m , std::string& comment, int n);
  bool getMove(std::string& m , unsigned int n);
  void getComment(std::string& comment);
  bool getPosition(ChessBoard& cb);
  size_t  getPosition(){return activePosition;};
  void getCurrentLine(MoveList& ml);
  int  getCurrentPly();
  bool addMove(ChessMove& m);
  bool addMove(const char* sz);
  bool addMove(const std::string& s);
  void addBoardComment(const char* sz, bool replace=false);
  void addBoardComment(std::string& s, bool replace=false);
  void addMoveComment(const char* sz, bool replace=false);
  void addMoveComment(std::string& s, bool replace=false);
  bool deleteMoves(ChessMove& m);
  // Change the order of a move.
  void orderMove(ChessMove& m, size_t neworder);
  bool goTo(size_t pos);
  void CopyByMove(ChessGame& newgame,size_t pos, size_t exclude);
//private:
  bool deleteMove(int n);
  bool deletePosition(size_t n);
  size_t activePosition;
  std::string piecechar;
  bool doMove(unsigned int n);
  void iterateMoves(std::string& s, size_t pos, int mn, bool comment, bool variation);
  size_t iterateVariation(size_t pos, int& current, int variation);
};

