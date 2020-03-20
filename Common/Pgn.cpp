//
#include <math.h>
#include "../Common/Utility.h"
#include "../Common/Pgn.h"
#include <iostream>
#include <algorithm>
#include "../Common/MoveList.h"
#include "../Common/MoveGenerator.h"
#include "../Common/NagValues.h"

using namespace std;

Pgn::Pgn()
{
}

Pgn::~Pgn()
{
  close();
}

bool Pgn::open(const std::string& fname, bool readonly)
{
  nextGame=1;
  if (readonly)
    return file.open(fname,GENERIC_READ,false,OPEN_EXISTING,false);
  else
    return file.open(fname,GENERIC_WRITE,false,OPEN_ALWAYS,false);
}

void Pgn::close()
{
  file.close();
}

int Pgn::size()
{
  string line;
  int nr;
  if (!file.isOpen())
    return -1;
  DWORD fp=file.dwFilepointer;
  file.setFilepointer(0);
  nr=0;

  while (file.readLine(line))
  {
    if (line.substr(0,7)=="[Event ")
      ++nr;
  }

  file.setFilepointer(fp);
  return nr;
}

bool Pgn::read(ChessGame& game, DWORD index, int maxmove, bool expandnag, const char* sz, bool onlyheader)
{
  string sGame,sHeader,line;
  string termination;
  int start=0,len=0,n;
  game.clear();
  
  if (!index) // no game 0
    return false;
  
  if (!sz)
  {
    if (nextGame>index)
    {
      nextGame=1;
      file.setFilepointer(0);
    }
  
    // Find the next game in file
    while (1)
    {
      if (!file.readLine(line))
        return false;
      if (line.substr(0,7)=="[Event ")
      {
        if (nextGame==index)
        {
          nextGame++;
          sHeader=line;
          sHeader+=' ';
          break;
        }
        nextGame++;
      }
    }

    // Read the header
    while (1)
    {
      if (!file.readLine(line))
        break;
      line=trim(line);
      if (line.length())
      {
        if (line.substr(0,7)=="[Event ")
        {
          file.putBack(line);
          break;
        }
        if (line.at(0)!='[')
        {
          file.putBack(line);
          break;
        }
        sHeader+=line;
        sHeader+=' ';
      }else
      {
        break;
      }
    }
  }else
  {
    start=0;
    len=strlen(sz);
    n=1;
    // Find the next game in file
    while (1)
    {
      if (start>=len)
        return false;
      line=getLine(&sz[start]);
      start+=line.length()+1;
      line=trim(line);
      if (line.length())
      {
        if (line.substr(0,7)=="[Event ")
        {
          if (n==index)
          {
            sHeader=line;
            sHeader+=' ';
            break;
          }
        }
        ++n;
      }
    }

    // Read the header
    while (1)
    {
      if (start>=len)
        break;
      line=getLine(&sz[start]);
      start+=line.length()+1;
      line=trim(line);
      if (line.length())
      {
        if (line.at(0)!='[')
        {
          sGame=line;
          sGame+=' ';
          break;
        }
        sHeader+=line;
        sHeader+=' ';
      }else
      {
        break;
      }
    }
  }

  interpretHeader(sHeader,game);

  if (onlyheader)
    return true;

  // Find the terminationmarker
  if (game.info.Result.find("1-0")!=string::npos)
    termination="1-0";
  else if (game.info.Result.find("0-1")!=string::npos)
    termination="0-1";
  else if (game.info.Result.find("1/2-1/2")!=string::npos)
    termination="1/2-1/2";
  else if (game.info.Result.find("*")!=string::npos)
    termination="*";
  else
    termination="";


  // Read the movetext
  if (!sz)
  {
    while (1)
    {
      if (!file.readLine(line))
        break;
      line=trim(line);
      if (line.length())
      {
        if (line.substr(0,7)=="[Event ")
        {
          file.putBack(line);
          break;
        }
        sGame+=line;
        sGame+=' ';
      }
    }
  }else
  {
    while (1)
    {
      if (start>=len)
        break;
      line=getLine(&sz[start]);
      start+=line.length()+1;
      line=trim(line);
      if (line.length())
      {
        if (line.substr(0,7)=="[Event ")
          break;
        sGame+=line;
        sGame+=' ';
      }
    }
  }

  // Fix a normal pgn-error 
  string::size_type f;
  while ((f=sGame.find("0-0-0"))!=string::npos)
    sGame.replace(f,5,"O-O-O");
  while ((f=sGame.find("0-0"))!=string::npos)
    sGame.replace(f,3,"O-O");

  interpretGame(sGame,game,maxmove,0,termination,expandnag);
  return true;
}

void Pgn::interpretHeader(std::string sHeader, ChessGame& game)
{
  int token;
  int setup;
  int index;
  string value;
  char fen[256];
  setup=0;
  index=0;
  *fen=0;
  game.setStartPosition("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq 0 1");
  while ((token=nextHeaderToken(sHeader,value,index))!=PGN_eof)
  {
    switch (token)
    {
      case PGN_black :
        game.info.Black=value;
        break;
      case PGN_date :
        game.info.Date=value;
        break;
      case PGN_event :
        game.info.Event=value;
        break;
      case PGN_result :
        game.info.Result=value;
        break;
      case PGN_round :
        game.info.Round=value;
        break;
      case PGN_site :
        game.info.Site=value;
        break;
      case PGN_white :
        game.info.White=value;
        break;
      case PGN_setup :
        if (value=="1")
        {
          setup=1;
          if (*fen)
            game.setStartPosition(fen);
        }else
        {
          setup=0;
        }
        break;
      case PGN_fen :
        strcpy_s(fen,256,value.c_str());
        if (setup)
          game.setStartPosition(fen);
        break;
      case PGN_eco :
        game.info.ECO=value;
        break;
      case PGN_opening1 :
        game.info.Opening=value;
        break;
      case PGN_opening2 :
        game.info.Variation=value;
        break;
      case PGN_opening3 :
        game.info.SubVariation=value;
        break;
      case PGN_whiteelo :
        game.info.WhiteElo=value;
        break;
      case PGN_blackelo :
        game.info.BlackElo=value;
        break;
      case PGN_remark :
        game.info.Remark=value;
        break;
      case PGN_annotator :
        game.info.Annotator=value;
        break;
    };
  };
}

int Pgn::nextHeaderToken(std::string& s, std::string& value, int& index)
{
  if (index >= (int)s.length())
    return PGN_eof;
  getValue(s,value,'[',']',index);
  return tagValue(value);
}

int Pgn::tagValue(std::string& value)
{
  int ret=PGN_tag;
  if (value.substr(0,6)=="Event ")
    ret = PGN_event;
  if (value.substr(0,5)=="Site ")
    ret = PGN_site;
  if (value.substr(0,6)=="Round ")
    ret = PGN_round;
  if (value.substr(0,6)=="White ")
    ret = PGN_white;
  if (value.substr(0,6)=="Black ")
    ret = PGN_black;
  if (value.substr(0,5)=="Date ")
    ret = PGN_date;
  if (value.substr(0,7)=="Result ")
    ret = PGN_result;
  if (value.substr(0,6)=="SetUp ")
    ret = PGN_setup;
  if (value.substr(0,4)=="FEN ")
    ret = PGN_fen;
  if (value.substr(0,4)=="ECO ")
    ret = PGN_eco;
  if (value.substr(0,8)=="Opening ")
    ret = PGN_opening1;
  if (value.substr(0,10)=="Variation ")
    ret = PGN_opening2;
  if (value.substr(0,13)=="SubVariation ")
    ret = PGN_opening3;
  if (value.substr(0,9)=="WhiteElo ")
    ret = PGN_whiteelo;
  if (value.substr(0,9)=="BlackElo ")
    ret = PGN_blackelo;
  if (value.substr(0,7)=="Remark ")
    ret = PGN_remark;
  if (value.substr(0,10)=="Annotator ")
    ret = PGN_annotator;
  if (ret==PGN_tag)
    return ret;
  stringValue(value);
  return ret;
}

void Pgn::stringValue(std::string& value)
{
  char c;
  string s=value;
  int index=0;
  value="";
  while ((c=s.at(index))!='\"')
  {
    index++;
    if (s.at(index)=='\\')
      index++;
    if (s.length()==index)
      return;
  }

  index++;
  while ((c=s.at(index))!='\"')
  {
    index++;
    if (c=='\\')
    {
      c=s.at(index);
      index++;
    }
    value+=c;
    if (s.length()==index)
      return;
  }
  return;
}

void Pgn::interpretGame(std::string sGame, ChessGame& game, int maxmove, int movenumber, const std::string& termination, bool expandnag)
{
  int token;
  //char sz[256];
  int savePosition,index;
  bool illegalmove=false;
  string value;
  ChessMove nullmove;
  nullmove.moveType=NULL_MOVE;
  index=0;
  while ((token=nextToken(sGame,value,index))!=PGN_eof)
  {
    switch (token)
    {
      case PGN_move :
        if (illegalmove)
          return;
        if ((movenumber/2)>=maxmove)
          return;
//        strcpy(sz,value.c_str());
        if (!game.addMove(value))
        {
          game.addMove(nullmove);
          game.addBoardComment("Illegal move: ");
          game.addBoardComment(value);
          cerr << "Illegal move: " << movenumber/2+1 << ".";
          if (game.position[game.activePosition].board.toMove==WHITE)
            cerr << "..";
          cerr << value.c_str() << endl;
          illegalmove=true;
        }
        movenumber++;
        break;
      case PGN_bracket : // Not in use
        break;
      case PGN_variation :
        savePosition=game.activePosition;
        game.backup(1);
        movenumber--;
        interpretGame(value,game,maxmove,movenumber,string(""),expandnag);
        game.activePosition=savePosition;
        movenumber++;
        break;
      case PGN_comment :
        expandNags(value);
        if (moveComment(value))
          game.addMoveComment(value);
        else
          game.addBoardComment(value);
        break;
      case PGN_shortcomment :
        game.addMoveComment(value);
        break;
      case PGN_nag :
        if (expandnag)
        {
          if (!convertNag(value))
            break;
        }else
        {
          value='$'+value;
        }
        if (moveComment(value))
          game.addMoveComment(value);
        else
          game.addBoardComment(value);
        break;
      case PGN_symbol :
        if (termination.length())
          if (value==termination)
            return;
        break;
    };
  };
}

int Pgn::nextToken(std::string& s, std::string& value, int& index)
{
  char c;
  if ((int)s.length()<=index)
    return PGN_eof;
  if (index==string::npos)
    return PGN_eof;

  value="";
  while (1)
  {
    c=s.at(index);
    switch (c)
    {
      case '{' :                      // Comment
		string::size_type f;
		getValue(s,value,'{','}',index);
		convertControlCharacter(value, false);
        return PGN_comment;
      case '<':                       // Bracket
        getValue(s,value,'<','>',index);
        return PGN_bracket;
      case '(':                       // Variation
        ravValue(s,value,index);
        return PGN_variation;
      case '$':                       // NAG value
        if (++index>=(int)s.length()) return PGN_nag;
        integerValue(s,value,index);
        return PGN_nag;
      case '!':
      case '?':
        while ((c=='!') || (c=='?'))
        {
          value+=c;
          if (++index>=(int)s.length()) return PGN_shortcomment;
          c=s.at(index);
        }
        return PGN_shortcomment;
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
        integerValue(s,value,index);
        return PGN_integer;
      case '.':
        value=c;
        index++;
        return PGN_period;
      default: // Move or symbol
        if (isSymbol(c))
        {
          if (isMove(s,index))
          {
            while (existIn(c,"NBRQKabcdefgh12345678xoO-="))
            {
              value+=c;
              if (++index>=(int)s.length()) return PGN_move;
              c=s.at(index);
              // Check for promotion since this could also be equal position.
              if (c=='=')
                if (index>=((int)s.length()-1))
                  if (existIn(s.at(index+1),"NBRQ"))
                    index++;
            }
            return PGN_move;
          }
          while (isSymbol(c))
          {
            value+=c;
            if (++index>=(int)s.length()) return PGN_symbol;
            c=s.at(index);
          }
          return PGN_symbol;
        }
        break;
    }
    if (++index>=(int)s.length()) return PGN_eof;
  };
}

void Pgn::getValue(std::string& s, std::string& value, char start, char end, int& index)
{
  char c;
  value="";
  if ((int)s.length()<=index)
    return;
  while ((c=s.at(index))!=start)
  {
    if (++index>=(int)s.length()) return;
    if (s.at(index)=='\\')
      if (++index>=(int)s.length()) return;
    if (s.length()==index)
      return;
  }
  if (++index>=(int)s.length()) return;
  while ((c=s.at(index))!=end)
  {
    if (++index>=(int)s.length()) return;
    if (c=='\\')
    {
      c=s.at(index);
      if (++index>=(int)s.length()) return;
    }
    value+=c;
  }
  value=trim(value);
  index++;
}

// Get variation
void Pgn::ravValue(std::string& s, std::string& value, int& index)
{
  char c,d;
  int count;
  string v;
  value="";
  while ((c=s.at(index))!='(')
  {
    index++;
    if (s.at(index)=='\\')
      index++;
    if (s.length()==index) return;
  }
  if (++index>=(int)s.length()) return;
  count=1;
  while (1)
  {
    c=s.at(index);
    switch (c)
    {
      case ')' :
        count--;
        if (!count)
        {
          index++;
          return;
        }
        value+=c;
        break;
      case '(':
        count++;
        value+=c;
        break;
      case '{':
        value+=c;
        if (++index>=(int)s.length()) return;
        while ((d=s.at(index))!='}')
        {
          value+=d;
          if (++index>=(int)s.length()) return;
        }
        value+=d;
        break;
      case '<':
        value+=c;
        if (++index>=(int)s.length()) return;
        while ((d=s.at(index))!='>')
        {
          value+=d;
          if (++index>=(int)s.length()) return;
        }
        value+=d;
        break;
      default :
        value+=c;
    }
    if (++index>=(int)s.length()) return;
  }
}

void Pgn::integerValue(std::string& s, std::string& value, int& index)
{
  char sz[16];
  int i=atoi(s.substr(index).c_str());
  if (!_itoa_s(i,sz,16,10))
    value=sz;
  if (i==0)
    index+=1;
  else
    index+=(int)log10((double)i)+1;
}

bool Pgn::moveComment(std::string& s)
{
  if ((s.length()>2) || (s.length()<1))
    return false;
  if ((s.at(0)=='!') || (s.at(0)=='?'))
    return true;
  return false;
}

bool Pgn::isSymbol(char c)
{
  if ((c>='A') && (c<='Z'))
    return true;
  if ((c>='a') && (c<='z'))
    return true;
  if ((c>='0') && (c<='9'))
    return true;
  if (c=='_')
    return true;
  if (c=='+')
    return true;
  if (c=='#')
    return true;
  if (c=='=')
    return true;
  if (c==':')
    return true;
  if (c=='-')
    return true;
  return false;
}

bool Pgn::isMove(std::string& s, int& index)
{
  char c1;
  char c2;
  if ((int)s.length()<(index+2))
    return false;
  c1=s.at(index);
  c2=s.at(index+1);
  if (existIn(c1,"NBRQKabcdefgh"))
    if (existIn(c2,"abcdefgh12345678x"))
      return true;
  if ((s.substr(index,3)=="o-o") || 
      (s.substr(index,3)=="O-O"))
    return true;
  return false;
}

int Pgn::convertNag(std::string& s)
{
  #include "NagValues.h"
  int nag=atoi(s.c_str());
  if ((nag<NagValueSize) && (nag>0))
    s=NagValues[nag];
  else
    s="";
  return s.length();
}

void Pgn::expandNags(std::string& s)
{
  int i=0;
  int j;
  int nagSize;
  string sNag;
  while ((i=s.find_first_of('$',i))!=string::npos)
  {
    j=s.find(' ',i);
    if (j!=string::npos)
      sNag=s.substr(i+1,j-i+1);
    else
      sNag=s.substr(i+1);
    nagSize=sNag.length()+1;
    if (convertNag(sNag))
      s.replace(i,nagSize,sNag);
    i=j;
  }
}

std::string Pgn::toString(ChessGame g, bool strict)
{
  string s,movetext;
  char sz[256];

  // Add std header
  s="[Event \"";
  if (!g.info.Event.length())
    s+="?";
  else
    s+=g.info.Event;
  s+="\"]\r\n[Site \"";
  if (!g.info.Site.length())
    s+="?";
  else
    s+=g.info.Site;
  s+="\"]\r\n[Date \"";
  if (!g.info.Date.length())
    s+="????.??.??";
  else
    s+=g.info.Date;
  s+="\"]\r\n[Round \"";
  if (!g.info.Round.length())
    s+="?";
  else
    s+=g.info.Round;
  s+="\"]\r\n[White \"";
  if (!g.info.White.length())
    s+="?";
  else
    s+=g.info.White;
  s+="\"]\r\n[Black \"";
  if (!g.info.Black.length())
    s+="?";
  else
    s+=g.info.Black;
  s+="\"]\r\n[Result \"";
  if (!g.info.Result.length())
  {
    s+="*";
  }else
  {
    if (g.info.Result=="½-½")
      s+="1/2-1/2";
    else
      s+=g.info.Result;
  }
  s+="\"]\r\n";

  // Add optional headers (alphabetical)
  if (!strict)
  {
    if (g.info.Annotator.length()>0)
    {
      s+="[Annotator \"";
      s+=g.info.Annotator;
      s+="\"]\r\n";
    }
    if (g.info.BlackElo.length()>0)
    {
      s+="[BlackElo \"";
      s+=g.info.BlackElo;
      s+="\"]\r\n";
    }
    if (g.info.BlackType.length()>0)
    {
      s+="[BlackType \"";
      s+=g.info.BlackType;
      s+="\"]\r\n";
    }
    if (g.info.ECO.length()>0)
    {
      s+="[ECO \"";
      s+=g.info.ECO;
      s+="\"]\r\n";
    }
    if (g.info.Opening.length()>0)
    {
      s+="[Opening \"";
      s+=g.info.Opening;
      s+="\"]\r\n";
    }
    if (g.info.Remark.length()>0)
    {
      s+="[Remark \"";
      s+=g.info.Remark;
      s+="\"]\r\n";
    }
    if (g.info.SubVariation.length()>0)
    {
      s+="[SubVariation \"";
      s+=g.info.SubVariation;
      s+="\"]\r\n";
    }
    if (g.info.WhiteTimeControl.length()>0)
    {
      s+="[TimeControl \"";
      s+=g.info.WhiteTimeControl;
      s+="\"]\r\n";
    }
    if (g.info.Variation.length()>0)
    {
      s+="[Variation \"";
      s+=g.info.Variation;
      s+="\"]\r\n";
    }
    if (g.info.WhiteElo.length()>0)
    {
      s+="[WhiteElo \"";
      s+=g.info.WhiteElo;
      s+="\"]\r\n";
    }
    if (g.info.WhiteType.length()>0)
    {
      s+="[WhiteType \"";
      s+=g.info.WhiteType;
      s+="\"]\r\n";
    }
  }
  if (g.activePosition<0)
    return s;
  // Add header for 'non' standard startposition
  g.position[0].board.getFen(sz);
  if (strncmp(sz,"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq",50))
  {
    s+="[SetUp \"1\"]\r\n";
    s+="[FEN \"";
    s+=sz;
    s+="\"]\r\n";
  }

  s+="\r\n";

  // Interprete movetext
  if (g.toStart())
    getMovetext(movetext,g);

  // Add result to the end of movetext
  if (!g.info.Result.length())
  {
    movetext+="*";
  }else
  {
    if (g.info.Result=="½-½")
      movetext+="1/2-1/2";
    else
      movetext+=g.info.Result;
  }

  breakLines(movetext,75);

  s+=movetext;
  return s;
}

void Pgn::getMovetext(std::string& movetext, ChessGame& g, int mn)
{
  char sz[20];
  ChessBoard cb;
  ChessMove  cm;
  int move;
  string s;
  // Add comment for this position;
  g.getComment(s);
  if (s.length())
  {
    movetext+="{ ";
	convertControlCharacter(s, true);
	movetext+=s;
    movetext+=" } ";
  }

  g.getPosition(cb);

  move=0;
  while (g.getMove(cm,s,move))
  {
    if (move>0)
    {
      movetext+="( ";
      if (cb.toMove!=WHITE)
      {
        if (!_itoa_s((mn+1)/2,sz,20,10))
          movetext+=sz;
        movetext+="... ";
      }
    }
    if (cb.toMove==WHITE)
    {
      if (!_itoa_s((mn+1)/2,sz,20,10))
        movetext+=sz;
      movetext+=". ";
    }
    movetext+=cb.makeMoveText(cm,sz,20,SAN);
    if (s.length())
      movetext+=s;
    movetext+=' ';

    // Loop through the variations first.
    if (move==0)
    {
      ++move;
      continue;
    }

    g.doMove(move);
    getMovetext(movetext, g, mn+1);
    g.backup(1);
    ++move;
    movetext+=") ";
  }
  
  // Do the main line
  if (g.doMove(0))
  {
    if ((move>1) && (cb.toMove==WHITE))
    {
      if (!_itoa_s((mn+1)/2,sz,20,10))
        movetext+=sz;
      movetext+="... ";
    }
    getMovetext(movetext, g, mn+1);
    g.backup(1);
  }
}

bool Pgn::deleteGame(DWORD index)
{
  char sz[MAX_PATH];
  char temppath[MAX_PATH];
  WinFile temp;
  string line;
  int nr;
  DWORD start_fp,end_fp;

  if (!file.isOpen())
    return false;

  GetTempPathA(MAX_PATH,temppath);
  GetTempFileNameA(temppath,"PCP",0,sz);
  if (!temp.open(sz,GENERIC_WRITE,false,CREATE_ALWAYS,false))
    return false;

  file.setFilepointer(0);
  nr=0;

  start_fp=0;
  while (file.readLine(line))
  {
    if (line.substr(0,7)=="[Event ")
      ++nr;
    if (nr==index)
      break;
    start_fp=file.dwFilepointer;
  }
  
  if (nr!=index)
  {
    temp.deleteFile();
    return false;
  }

  end_fp=start_fp;
  while (file.readLine(line))
  {
    if (line.substr(0,7)=="[Event ")
      break;
    end_fp=file.dwFilepointer;
  }
 
  file.setFilepointer(0);
  while (file.dwFilepointer<start_fp)
  {
    if (!file.read(sz,1))
    {
      temp.deleteFile();
      return false;
    }
    if (!temp.write(sz,1))
    {
      temp.deleteFile();
      return false;
    }
  }

  file.setFilepointer(end_fp);

  while (file.read(sz,1))
    temp.write(sz,1);
 
  file.close();
  temp.close();
  CopyFileA(temp.sFilename.c_str(),file.sFilename.c_str(),FALSE);
  temp.deleteFile();
  file.reopen();
  return true;
}

bool Pgn::replaceGame(DWORD index, ChessGame& game)
{
  char sz[MAX_PATH];
  char temppath[MAX_PATH];
  WinFile temp;
  string line;
  int nr;
  DWORD start_fp,end_fp;

  if (!file.isOpen())
    return false;

  GetTempPathA(MAX_PATH,temppath);
  GetTempFileNameA(temppath,"PCP",0,sz);
  if (!temp.open(sz,GENERIC_WRITE,false,CREATE_ALWAYS,false))
    return false;

  file.setFilepointer(0);
  nr=0;

  start_fp=0;
  while (file.readLine(line))
  {
    if (line.substr(0,7)=="[Event ")
      ++nr;
    if (nr==index)
      break;
    start_fp=file.dwFilepointer;
  }
  
  if (nr!=index)
  {
    temp.deleteFile();
    return false;
  }

  end_fp=start_fp;
  while (file.readLine(line))
  {
    if (line.substr(0,7)=="[Event ")
      break;
    end_fp=file.dwFilepointer;
  }
 
  file.setFilepointer(0);
  while (file.dwFilepointer<start_fp)
  {
    if (!file.read(sz,1))
    {
      temp.deleteFile();
      return false;
    }
    if (!temp.write(sz,1))
    {
      temp.deleteFile();
      return false;
    }
  }

  line=toString(game,false);

  line+="\n\n";
  temp.write(line.c_str(),line.length());

  file.setFilepointer(end_fp);

  while (file.read(sz,1))
    temp.write(sz,1);
 
  file.close();
  temp.close();
  CopyFileA(temp.sFilename.c_str(),file.sFilename.c_str(),FALSE);
  temp.deleteFile();
  file.reopen();
  return true;
}

bool Pgn::appendGame(ChessGame& game)
{
  DWORD fp;
  int i;
  if (!file.isOpen())
    return false;

  string s,line1,line2;

  fp=file.size();
  if (fp>0)
  {
    i=fp-20;
    if (i<0)
      fp=0;
    else
      fp=(DWORD)i;
    file.setFilepointer(fp);
    while (file.readLine(s))
    {
      line1=line2;
      line2=s;
    };
  }

  if (line2!="")
  {
    file.write("\n",1);
  }
  
  if (line1!="")
  {
    file.write("\n",1);
  }
  
  s=toString(game,false);
  s+='\n';
  file.write(s.c_str(),s.length());
  
  return true;
}

void Pgn::convertControlCharacter(std::string& s, bool from)
{
	string::size_type f;
	string c;
	int i;
	if (from)
	{
		while ((f = s.find("\n")) != string::npos)
			s.replace(f, 1, "^010 ");
		while ((f = s.find("\r")) != string::npos)
			s.replace(f, 1, "^013 ");
		while ((f = s.find("\t")) != string::npos)
			s.replace(f, 1, "^009 ");
	}
	else
	{
		f = 0;
		while ((f = s.find("^",f)) != string::npos)
		{
			if ((s.size() > f + 1) && (isNumber(s.substr(f + 1, 1), 10)))
			{
				i = stoi(s.substr(f + 1, 3));
				c = (char)i;
				i = 2;
				if ((s.size() > f + i) && (isNumber(s.substr(f + i, 1), 10)))
				{
					++i;
					if (isNumber(s.substr(f + i, 1), 10))
						++i;
				}
				if ((s.size() > f + i) && (s.at(f + i) == ' '))
					++i;
				s.replace(f, i, c);
			}
			else
			{
				++f;
				if (f >= s.size())
					break;
			}
		}
	}
}
