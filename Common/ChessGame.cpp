#include "../Common/ChessGame.h"
#include "../Common/Relations.h"
#include <iostream>
#include <vector>
#include "Utility.h"

using namespace std;

ChessGame::ChessGame()
{
  activePosition=-1;
  piecechar="NBRQK";
}

ChessGame::~ChessGame()
{
}

void ChessGame::clear(bool keepheader)
{
  if (!keepheader)
    info.clear();
  else
    info.Result.erase();
  position.clear();
  activePosition=-1;
}

void ChessGame::setStartPosition(const ChessBoard& cb)
{
  ChessGamePosition cgp;
  position.clear();
  cgp.board.copy(cb);
  position.push_back(cgp);
  activePosition=0;
}

void ChessGame::setStartPosition(const char* fen)
{
  ChessGamePosition cgp;
  position.clear();
  cgp.board.setFen(fen);
  position.push_back(cgp);
  activePosition=0;
}

void ChessGame::getStartPosition(ChessBoard& cb)
{
  if (activePosition<0)
    cb.clear();
  else
    cb.copy(position[0].board);
}

bool ChessGame::toStart()
{
  if (activePosition>=0)
    activePosition=0;
  else
    return false;
  return true;
}

bool ChessGame::toEnd(bool variation)
{
  if (activePosition<0)
    return false;
  int p;
  if (variation)
    p=activePosition;
  else
    p=0;
  while (position[p].move.size()>0)
  {
    p=position[p].move[0].posIndex;
    if (p<0)
      break;
  }
  activePosition=p;
  return true;
}

// Delete all moves in a variation.
bool ChessGame::deleteMoves(ChessMove& m)
{
  int mindex,pos;
  ChessBoard cb;

  if (activePosition<0)
    return false;

  getPosition(cb);
  if (!m.empty())
  {
    if ((mindex=position[activePosition].moveExist(m))<0)
      return false;
    pos=position[activePosition].move[mindex].posIndex;
    deletePosition(pos);
    if (!goTo(cb))
      return false;
  }else
  {
    while (!position[activePosition].move.empty())
    {
      pos=position[activePosition].move[0].posIndex;
      deletePosition(pos);
      if (!goTo(cb))
        return false;
    }
  }
  return true;
}

void ChessGame::orderMove(ChessMove& m, int neworder)
{
  int n,current;
  ChessGameMove cgm;
  if ((activePosition<0) || m.empty())
    return;
  if ((current=position[activePosition].moveExist(m))<0)
    return;

  if (neworder>=(int)position[activePosition].move.size())
    neworder=position[activePosition].move.size()-1;

  if (neworder==current)
    return;

  cgm=position[activePosition].move[current];

  // Erase the move
  n=0;
  vector<ChessGameMove>::iterator it=position[activePosition].move.begin();
  while (it!=position[activePosition].move.end())
  {
    if (n==current)
    {
      position[activePosition].move.erase(it);
      break;
    }
    ++n;
    ++it;
  }
  if (n!=current)
    return;

  // Set it in again.
  n=0;
  it=position[activePosition].move.begin();
  while (it!=position[activePosition].move.end())
  {
    if (n==neworder)
    {
      position[activePosition].move.insert(it,cgm);
      return;
    }
    ++n;
    ++it;
  }
}

bool ChessGame::addMove(ChessMove& m)
{
  int i;
  ChessGamePosition cgp;
  ChessGameMove cgm;

  // First check if the move exist, and if, set the position after
  // the move to active position and return.
  if ((i=position[activePosition].moveExist(m))>=0)
  {
    activePosition=position[activePosition].move[i].posIndex;
    return true;
  }

  // Do the move from active position.
  cgp.board.copy(position[activePosition].board);
  if (!cgp.board.doMove(m,true))
    return false;

  // Don't do two nullmove in a row.
  if ((m.moveType&NULL_MOVE) && (activePosition>0))
  {
    i=position[activePosition].fromIndex;
    if ((i>=0) && (cgp.board.compare(position[i].board)==0))
    {
      if ((position[activePosition].move.size()==0) &&
          (position[i].move.size()==1))
      {
        activePosition=i;
        deleteMove(0);
      }else
      {
        activePosition=i;
      }
      return true;
    }
  }

  // Save the move and create a new position
  cgm.move=m;
  cgm.posIndex=position.size();
  cgp.fromIndex=activePosition;
  
  position[activePosition].move.push_back(cgm);
  position.push_back(cgp);
  activePosition=cgm.posIndex;
  return true;
}

bool ChessGame::addMove(const char* sz)
{
  ChessMove m;
  if (strcmp(sz,"null")==0)
    m.moveType=NULL_MOVE;
  else
    m=position[activePosition].board.getMoveFromText(sz);
  if (m.empty())
    return false;
  return addMove(m);
}

bool ChessGame::addMove(const std::string& s)
{
  ChessMove m;
  if (s=="null")
    m.moveType=NULL_MOVE;
  else
    m=position[activePosition].board.getMoveFromText(s);
  if (m.empty())
    return false;
  return addMove(m);
}

void ChessGame::addBoardComment(const char* sz, bool replace)
{
  if (activePosition<0)
    return;
  if (position[activePosition].comment.length()>0)
    position[activePosition].comment+=" ";
  if (replace)
    position[activePosition].comment=sz;
  else
    position[activePosition].comment+=sz;
}

void ChessGame::addBoardComment(std::string& s, bool replace)
{
  if (activePosition<0)
    return;
  if (position[activePosition].comment.length()>0)
    position[activePosition].comment+=" ";
  if (replace)
    position[activePosition].comment=s;
  else
    position[activePosition].comment+=s;
}

void ChessGame::addMoveComment(const char* sz, bool replace)
{
  int i,p;
  if (activePosition<1)
    return;
  if ((p=position[activePosition].fromIndex)<0)
    return;
  if ((i=position[p].moveExist(activePosition))<0)
    return;
  if (replace)
    position[p].move[i].comment=sz;
  else
    position[p].move[i].comment+=sz;
}

void ChessGame::addMoveComment(std::string& s, bool replace)
{
  int i,p;
  if (activePosition<1)
    return;
  if ((p=position[activePosition].fromIndex)<0)
    return;
  if ((i=position[p].moveExist(activePosition))<0)
    return;
  if (replace)
    position[p].move[i].comment=s;
  else
    position[p].move[i].comment+=s;
}

bool ChessGame::getMove(ChessMove& m , std::string& comment, int n)
{
  m.clear();
  comment="";
  if (activePosition<0)
    return false;
  if ((n==-1) && (activePosition>0))
  {
    int p=position[activePosition].fromIndex;
    for (unsigned int i=0;i<position[p].move.size();i++)
    {
      if (position[p].move[i].posIndex==activePosition)
      {
        m=position[p].move[i].move;
        comment=position[p].move[i].comment;
        return true;
      }
    }
    return false;
  }
  if (n<0)
    return false;
  if ((int)position[activePosition].move.size()>n)
  {
    m=position[activePosition].move[n].move;
    comment=position[activePosition].move[n].comment;
    return true;
  }
  return false;
}

bool ChessGame::getMove(std::string& m, unsigned int n)
{
  ChessMove cm;
  char sz[16];
  string comment;
  if (!getMove(cm,comment,n))
    return false;
  m=position[activePosition].board.makeMoveText(cm,sz,16,FIDE);
  int j=m.length();
  for (int i=0;i<j;i++)
  {
    switch (m.at(i))
    {
      case 'N' :
        m[i]=piecechar[0];
        break;
      case 'B' :
        m[i]=piecechar[1];
        break;
      case 'R' :
        m[i]=piecechar[2];
        break;
      case 'Q' :
        m[i]=piecechar[3];
        break;
      case 'K' :
        m[i]=piecechar[4];
        break;
    }
  }
  m+=comment;
  return true;
}

void ChessGame::getComment(std::string& comment)
{
  comment="";
  if (activePosition<0)
    return;
  comment=position[activePosition].comment;
}

bool ChessGame::getPosition(ChessBoard& b)
{
  b.clear();
  if (activePosition<0)
    return false;
  b.copy(position[activePosition].board);
  return true;
}

bool ChessGame::doMove(unsigned int n)
{
  if (activePosition<0)
    return false;
  if (position[activePosition].move.size()>n)
  {
    activePosition=position[activePosition].move[n].posIndex;
    return true;
  }
  return false;
}

void ChessGame::backup(unsigned int n)
{
  unsigned int i=0;
  if (activePosition<1)
    return;
  while (i<n)
  {
    activePosition=position[activePosition].fromIndex;
    if (activePosition==0)
      return;
    ++i;
  }
}

void ChessGame::forward(unsigned int n)
{
  unsigned int i=0;
  if (activePosition<0)
    return;
  int p;
  p=activePosition;
  while ((i<n) && (position[p].move.size()>0))
  {
    p=position[p].move[0].posIndex;
    if (p<0)
      break;
    ++i;
  }
  if (p>=0)
    activePosition=p;
}

int ChessGame::mainMoves()
{
  int i=0,p=0;
  if (activePosition<1)
    return 0;
  while (position[p].move.size()>0)
  {
    p=position[p].move[0].posIndex;
    if (p<0)
      break;
    i++;
  }
  return i;
}

std::string ChessGame::toString()
{
  string s;
  char sz[80];
  s="[Event \"";
  if (info.Event.length()>0)
    s+=info.Event;
  else
    s+="?";
  s+="\"]\n";
  s+="[Site \"";
  if (info.Site.length()>0)
    s+=info.Site;
  else
    s+="?";
  s+="\"]\n";
  s+="[Date \"";
  if (info.Date.length()>0)
    s+=info.Date;
  else
    s+="????.??.??";
  s+="\"]\n";
  s+="[Round \"";
  if (info.Round.length()>0)
    s+=info.Round;
  else
    s+="?";
  s+="\"]\n";
  s+="[White \"";
  if (info.White.length()>0)
    s+=info.White;
  else
    s+="?";
  s+="\"]\n";
  s+="[Black \"";
  if (info.Black.length()>0)
    s+=info.Black;
  else
    s+="?";
  s+="\"]\n";
  s+="[Result \"";
  if (info.Result.length()>0)
    s+=info.Result;
  else
    s+="*";
  s+="\"]\n";
  if (info.BlackElo.length()>0)
  {
    s+="[BlackElo \"";
    s+=info.BlackElo;
    s+="\"]\n";
  }
  if (info.BlackType.length()>0)
  {
    s+="[BlackType \"";
    s+=info.BlackType;
    s+="\"]\n";
  }
  if (info.WhiteElo.length()>0)
  {
    s+="[WhiteElo \"";
    s+=info.WhiteElo;
    s+="\"]\n";
  }
  if (info.WhiteType.length()>0)
  {
    s+="[WhiteType \"";
    s+=info.WhiteType;
    s+="\"]\n";
  }
  if (info.WhiteTimeControl != info.BlackTimeControl)
  {
	  if (info.WhiteTimeControl.length() > 0)
	  {
		  s += "[WhiteTimeControl \"";
		  s += info.WhiteTimeControl;
		  s += "\"]\n";
	  }
	  if (info.BlackTimeControl.length() > 0)
	  {
		  s += "[BlackTimeControl \"";
		  s += info.BlackTimeControl;
		  s += "\"]\n";
	  }
  }
  else
  {
	  if (info.WhiteTimeControl.length() > 0)
	  {
		  s += "[TimeControl \"";
		  s += info.WhiteTimeControl;
		  s += "\"]\n";
	  }
  }
  if (info.Annotator.length()>0)
  {
    s+="[Annotator \"";
    s+=info.Annotator;
    s+="\"]\n";
  }
  if (info.Remark.length()>0)
  {
    s+="[Remark \"";
    s+=info.Remark;
    s+="\"]\n";
  }
  if (position.size()<1)
    return s;

  position[0].board.getFen(sz);
  if (strncmp(sz,"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -",52))
  {
    s+="[SetUp \"1\"]\n";
    s+="[FEN \"";
    s+=sz;
    s+="\"]\n";

  }
  s+="\n";
  if (position[0].comment.length()>0)
  {
    s+="{";
    s+=position[0].comment;
    s+="}\n";
  }
  string moves;
  iterateMoves(moves,0,1,true,true);
  // Make linebreak in movetext
  int it=0;
  while (1)
  {
    if (moves.substr(it).length()<75)
      break;
    it+=70;
    if ((it=moves.find(" ",it,1))==string::npos)
      break;
    moves.replace(it,1,"\n");
  }
  s+=moves;
  s+=" ";
  if (info.Result.length()>0)
    s+=info.Result;
  else
    s+="*";
  s+="\n";
  return s;
};

void ChessGame::iterateMoves(std::string& s, int pos, int mn, bool comment, bool variation)
{
  int i;
  char buf[20];
  ChessBoard cb;
  ChessMove  cm;
  int n=position[pos].move.size();
  if (n>0)
  {
    cb.copy(position[pos].board);
    cm=position[pos].move[0].move;
    // print movenumber if white on move
    if (mn%2)
    {
      if (!_itoa_s(mn/2+1,buf,20,10))
        s+=buf;
      s+=". ";
    }
    // Print main move
    s+=cb.makeMoveText(cm,buf,20,SAN);

    // Move comment (!, ? etc.)
    if (comment)
    {
      s+=position[pos].move[0].comment;
      // Print comment for the resulting position.
      if (position[position[pos].move[0].posIndex].comment.length()>0)
      {
        s+=" {";
        s+=position[position[pos].move[0].posIndex].comment;
        s+=" }";
      }
    }
    s+=' ';
    if (variation)
    {
      // If this is a variation, mark it.
      if (n>1)
        s+=" ( ";
      else
        s+=' ';
      i=1;
      while (i<n)
      {
        if (i>1)
          s+=" ; ";
        if (!_itoa_s((mn-1)/2+1,buf,20,10))
          s+=buf;
        if (mn%2)
          s+=". ";
        else
          s+="... ";
        cm=position[pos].move[i].move;
        s+=cb.makeMoveText(cm,buf,20,SAN);
        if (comment)
        {
          s+=position[pos].move[i].comment;
      
          // Print comment for the resulting position.
          if (position[position[pos].move[i].posIndex].comment.length()>0)
          {
            s+=" {";
            s+=position[position[pos].move[0].posIndex].comment;
            s+="}";
          }
          s+=' ';
        }
        iterateMoves(s,position[pos].move[i].posIndex,mn+1,comment,variation);
        i++;
      }
      if (n>1)
      {
        s+=" ) ";
        if (mn%2)
        {
          if (!_itoa_s((mn-1)/2+1,buf,20,10))
          s+=buf;
          s+="... ";
        }
      }
    }
    iterateMoves(s,position[pos].move[0].posIndex,mn+1,comment,variation);
  }
}

void ChessGame::copy(const ChessGame& g)
{
  clear();
  activePosition=g.activePosition;
  info=g.info;
  for (unsigned int i=0;i<g.position.size();i++)
    position.push_back(g.position[i]);
}


int ChessGame::iterateVariation(int pos, int& current, int variation)
{
  int i,p;
  int n=position[pos].move.size();
  if (n==0)
  {
    if (current==variation)
      return pos;
    return -1;
  }

  i=0;
  while (i<n)
  {
    if (i>0)
      ++current;

    p=iterateVariation(position[pos].move[i].posIndex,current,variation);
    if (p>=0)
      return p;
    ++i;
  }
  return -1;
}

void ChessGame::getCurrentLine(MoveList& ml)
{
  vector<ChessGameMove>::iterator it;
  ml.clear();
  if (activePosition<0)
    return;
  int p,last;
  last=activePosition;
  p=position[last].fromIndex;
  while (p>=0)
  {
    it=position[p].move.begin();
    while (it!=position[p].move.end())
    {
      if (((ChessGameMove)(*it)).posIndex==last)
      {
        ml.push_front(((ChessGameMove)(*it)).move);
        break;
      }
      ++it;
    }
    last=p;
    p=position[last].fromIndex;
  };
}

int ChessGame::getCurrentPly()
{
  int n=0;
  int p=activePosition;
  while (p>0)
  {
    p=position[p].fromIndex;
    ++n;
  }
  return n;
}

std::string ChessGame::getVariation(int n, bool comment)
{
  ChessBoard cb;
  ChessMove  cm;
  string s;
  vector<int> ml;
  char buf[20];
	int v = 0;
  int p,p1,i,moves,mn;
  int it;
  p=iterateVariation(0,v,n);
  if (p<0)
    return "";

  // Put all positions in a list
  it=0;
  while (p>=0)
  {
    ml.push_back(p);
    p=position[p].fromIndex;
    ++it;
  };

  it-=1;

  if (position[0].board.toMove==WHITE)
  {
    mn=1;
    s="";
  }else
  {
    mn=2;
    if (!_itoa_s((int)mn/2+1,buf,20,10))
      s=buf;
    else
      s="";
    s+="... ";
  }
  while (it>=0)
  {
    p=ml[it];
    if (it>0)
      p1=ml[it-1];
    else
      p1=-1;
    moves=position[p].move.size();
    if (moves==0)
      break;
    cm.clear();
    i=0;
    while (i<moves)
    {
      if (position[p].move[i].posIndex==p1)
      {
        cm=position[p].move[i].move;
        if (mn%2)
        {
          if (!_itoa_s((int)mn/2+1,buf,20,10))
            s+=buf;
          s+=". ";
        }
        if (!(cm.moveType&NULL_MOVE))
          s+=position[p].board.makeMoveText(cm,buf,16,SAN);
        if (comment)
        {
          if (position[p].move[i].comment.length())
          {
            if (position[p].move[i].comment.at(0)=='$')
              s+=' ';
            s+=position[p].move[i].comment;
          }
      
          // Print comment for the resulting position.
          if ((p1>=0) && (position[p1].comment.length()>0))
          {
            s+=" { ";
            s+=position[p1].comment;
            s+=" }";
          }
        }
        s+=' ';
        ++mn;
        break;
      }
      ++i;
    }
    if (cm.empty())
      break;
    --it;
  }

  return s;
}

int ChessGame::getVariation(int n, bool comment, ChessGame& variation)
{
  ChessBoard cb;
  ChessMove  cm;
  string s;
  vector<int> ml;
	int v = 0;
	int p, p1, i, moves;
  int it;
  p=iterateVariation(0,v,n);
  if (p<0)
    return 0;

  // Put all positions in a list
  it=0;
  while (p>=0)
  {
    ml.push_back(p);
    p=position[p].fromIndex;
    ++it;
  };

  it-=1;

  variation.clear();
  variation.setStartPosition(position[0].board);

  while (it>=0)
  {
    p=ml[it];
    if (it>0)
      p1=ml[it-1];
    else
      p1=-1;
    moves=position[p].move.size();
    if (moves==0)
      break;
    cm.clear();
    i=0;
    while (i<moves)
    {
      if (position[p].move[i].posIndex==p1)
      {
        cm=position[p].move[i].move;
        if (!(cm.moveType&NULL_MOVE) && variation.addMove(cm))
        {
          if (comment)
          {
            if (position[p].move[i].comment.length())
              variation.addMoveComment(position[p].move[i].comment);
            if ((p1>=0) && (position[p1].comment.length()>0))
              variation.addBoardComment(position[p1].comment);
          }
        }
        break;
      }
      ++i;
    }
    if (cm.empty())
      break;
    --it;
  }

  return 1;
}

void ChessGame::getDate(SYSTEMTIME& st)
{
  ZeroMemory(&st,sizeof(SYSTEMTIME));
  // ccyy.mm.dd
  st.wYear=atoi(info.Date.substr(0,4).c_str());
  if (info.Date.length()<6)
    return;
  st.wMonth=atoi(info.Date.substr(5,2).c_str());
  if (info.Date.length()<9)
    return;
  st.wDay=atoi(info.Date.substr(8,2).c_str());
}

void ChessGame::setDate(const SYSTEMTIME& st)
{
  char sz[32];
  if (st.wYear)
  {
    sprintf_s(sz,32,"%04d",st.wYear);
    info.Date=sz;
  }else
  {
    info.Date="????.??.??";
    return;
  }
  if (st.wMonth)
  {
    sprintf_s(sz,32,".%02d",st.wMonth);
    info.Date+=sz;
  }else
  {
    info.Date+=".??.??";
    return;
  }
  if (st.wDay)
    sprintf_s(sz,32,".%02d",st.wDay);
  else
    strcpy_s(sz,32,".??");
  info.Date+=sz;
}

void ChessGame::setTimeControl(const std::string& tc)
{
	info.WhiteTimeControl = tc;
	info.BlackTimeControl = tc;
}

void ChessGame::setWhiteTimeControl(const std::string& tc)
{
	info.WhiteTimeControl = tc;
}

void ChessGame::setBlackTimeControl(const std::string& tc)
{
	info.BlackTimeControl = tc;
}

const std::string ChessGame::getWhiteTimeControl()
{
	return info.WhiteTimeControl;
}

const std::string ChessGame::getBlackTimeControl()
{
	return info.BlackTimeControl;
}

int ChessGame::isDraw()
{
  if (activePosition<0)
    return 0;

  //repetitiondraw
  int p;
  int rep;
  p=position[activePosition].fromIndex;
  rep=1;
  while (p>=0)
  {
    if (position[p].board.compare(position[activePosition].board)==0)
      ++rep;
    p=position[p].fromIndex;
  }
  if (rep>2)
    return 1;

  
  // 50 move draw
  if (position[activePosition].board.move50draw>99)
    return 2;

  // insufficient material
  int wn,bn,wb[2],bb[2];
  wn=bn=wb[0]=wb[1]=bb[0]=bb[1]=0;
  int sq;
  for (int i=0;i<64;i++)
  {
    sq=SQUARE128(i);
    switch (position[activePosition].board.board[sq])
    {
      case whitepawn: return 0;
      case blackpawn: return 0;
      case whiteknight: wn++;break;
      case blackknight: bn++;break;
      case whitebishop: wb[SQUARECOLOR(sq)]++;break;
      case blackbishop: bb[SQUARECOLOR(sq)]++;break;
      case whiterook: return 0;
      case blackrook: return 0;
      case whitequeen: return 0;
      case blackqueen: return 0;
    }
  }

  // King alone or only one piece is draw
  if ((wn+wb[0]+wb[1]+bn+bb[0]+bb[1])<2)
    return 3;

  // All bishop of same color is draw
  if ((!wn && !bn) &&
     ((!wb[0] && !bb[0]) ||
      (!wb[1] && !bb[1])))
    return 3;
  return 0;
}

bool ChessGame::goTo(int pos)
{
  vector<ChessGamePosition>::iterator it=position.begin();
  int i=0;
  while (it!=position.end())
  {
    if (i==pos)
    {
      activePosition=i;
      return true;
    }
    ++i;
    ++it;
  }
  return false;
}

bool ChessGame::goTo(const ChessBoard& bb, bool strict)
{
  int oldpos=activePosition;

  // Find position in main line
  toEnd();
  while (activePosition>=0)
  {
    if (strict)
    {
      if (position[activePosition].board.compare(bb)==0)
        return true;
    }else
    {
      if (position[activePosition].board.compare(bb)!=1)
        return true;
    }
    activePosition=position[activePosition].fromIndex;
  }

  // Search in all positions.
  int size=position.size();
  activePosition=0;
  while (activePosition<size)
  {
    if (strict)
    {
      if (position[activePosition].board.compare(bb)==0)
        return true;
    }else
    {
      if (position[activePosition].board.compare(bb)!=1)
        return true;
    }
    ++activePosition;
  }
  activePosition=oldpos;
  return false;
}

bool ChessGame::deleteMove(int n)
{
  if ((activePosition<0) || (n<0))
    return false;
  std::vector<ChessGameMove>::iterator it=position[activePosition].move.begin();
  int i=0;
  while (it!=position[activePosition].move.end())
  {
    if (i==n)
    {
      position[activePosition].move.erase(it);
      return true;
    }
    ++i;
    ++it;
  }
  return false;
}

bool ChessGame::deletePosition(int n)
{
  if (!goTo(n))
    return false;
  if (n==0)
  {
    clear(true);
    return true;
  }
  ChessGame g;
  g.setStartPosition(position[0].board);
  CopyByMove(g,0,n);
  clear(true);
  for (unsigned int i=0;i<g.position.size();i++)
    position.push_back(g.position[i]);
  activePosition=0;
  return true;
}

void ChessGame::CopyByMove(ChessGame& newgame,int pos, int exclude)
{
  unsigned int mindex=0;
  string s;
  activePosition=pos;
  getComment(s);
  newgame.addBoardComment(s);
  while (mindex<position[pos].move.size())
  {
    if (position[pos].move[mindex].posIndex!=exclude)
    {
      newgame.addMove(position[pos].move[mindex].move);
      newgame.addMoveComment(position[pos].move[mindex].comment);
      CopyByMove(newgame,position[pos].move[mindex].posIndex,exclude);
      newgame.backup(1);
    }
    ++mindex;
  }
}