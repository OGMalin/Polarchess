#include "../Common/Epd.h"
#include "../Common/ChessBoard.h"
#include "../Common/Utility.h"
#include "../Common/WinFile.h"

using namespace std;

void Epd::set(const std::string& s)
{
  int i,j,k;
  char ch;
  string tmp;
  string line=trunc(s);
  clear();
  board=getWord(line,1);
  if (board.length()<1)
  {
    board="8/8/8/8/8/8/8/8";
    color="w";
    castling="-";
    enpassant="-";
    return;
  }
  color=getWord(line,2);
  if (color.length()<1)
  {
    color="w";
    castling="-";
    enpassant="-";
    return;
  }
  castling=getWord(line,3);
  if (castling.length()<1)
  {
    castling="-";
    enpassant="-";
    return;
  }
  enpassant=getWord(line,4);
  if (enpassant.length()<1)
  {
    enpassant="-";
    return;
  }
  i=board.length()+color.length()+castling.length()+enpassant.length()+4;

  // If FEN check for halfmoveclock and movecount
  tmp=getWord(line,5);
  if (tmp.length()>0)
  {
    if (isdigit(tmp.at(0)))
    {
      hmvc=atoi(tmp.c_str());
      i+=tmp.length()+1;
      tmp=getWord(line,6);
      if (tmp.length()>0)
      {
        if (isdigit(tmp.at(0)))
        {
          fmvn=atoi(tmp.c_str());
          i+=tmp.length()+1;
        }
      }
    }
  }

  if (i>=(int)line.length())
    return;
  line=line.substr(i);
  i=1;
  tmp=trim(getWord(line,i,";"));
  j=tmp.length();
  while (j>0)
  {
    if (tmp.substr(0,4)=="acd ")
      acd=atoi(tmp.substr(4).c_str());
    else if (tmp.substr(0,4)=="acn ")
      acn=atoi(tmp.substr(4).c_str());
    else if (tmp.substr(0,4)=="acs ")
      acs=atoi(tmp.substr(4).c_str());
    else if (tmp.substr(0,3)=="am ")
      am=trim(tmp.substr(3).c_str());
    else if (tmp.substr(0,3)=="bm ")
      bm=trim(tmp.substr(3).c_str());
    else if  ((tmp.at(0)=='c') && (isdigit(tmp.at(1))))
    {
      k=0;
      while (k<10)
      {  
        ch='0'+(char)k;
        if (tmp.at(1)==ch)
        {
          c[k]=stringValue(tmp.substr(3));
          break;
        }
        k++;
      }
    }
    else if (tmp.substr(0,3)=="ce ")
    {
      ce=atoi(tmp.substr(3).c_str());
      bce=true;
    }
    else if (tmp.substr(0,3)=="dm ")
      dm=atoi(tmp.substr(3).c_str());
    else if (tmp.substr(0,11)=="draw_accept")
      draw_accept=true;
    else if (tmp.substr(0,10)=="draw_claim")
      draw_claim=true;
    else if (tmp.substr(0,10)=="draw_offer")
      draw_offer=true;
    else if (tmp.substr(0,11)=="draw_reject")
      draw_reject=true;
    else if (tmp.substr(0,4)=="eco ")
      eco=stringValue(tmp.substr(4));
    else if (tmp.substr(0,5)=="fmvn ")
      fmvn=atoi(tmp.substr(5).c_str());
    else if (tmp.substr(0,5)=="hmvc ")
      hmvc=atoi(tmp.substr(5).c_str());
    else if (tmp.substr(0,3)=="id ")
      id=stringValue(tmp.substr(3));
    else if (tmp.substr(0,4)=="nic ")
      nic=stringValue(tmp.substr(4));
    else if (tmp.substr(0,3)=="pm ")
      pm=trim(tmp.substr(3).c_str());
    else if (tmp.substr(0,3)=="pv ")
      pv=trim(tmp.substr(3).c_str());
    else if (tmp.substr(0,3)=="rc ")
      rc=atoi(tmp.substr(3).c_str());
    else if (tmp.substr(0,6)=="resign")
      resign=true;
    else if (tmp.substr(0,3)=="sm ")
      sm=trim(tmp.substr(3).c_str());
    else if (tmp.substr(0,5)=="tcgs ")
      tcgs=atoi(tmp.substr(5).c_str());
    else if (tmp.substr(0,5)=="tcri ")
      tcri=stringValue(tmp.substr(5));
    else if (tmp.substr(0,5)=="tcsi ")
      tcsi=stringValue(tmp.substr(5));
    else if  ((tmp.at(0)=='v') && (isdigit(tmp.at(1))))
    {
      k=0;
      while (k<10)
      {  
        ch='0'+(char)k;
        if (tmp.at(1)==ch)
        {
          v[k]=stringValue(tmp.substr(3));
          break;
        }
        k++;
      }
    }
    else if (tmp.substr(0,3)=="vm ")
    {
      if (vm.length())
        vm+=' ';
      vm+=trim(tmp.substr(3).c_str());
    }

    i++;
    tmp=trim(getWord(line,i,";"));
    j=tmp.length();
  }
}

std::string Epd::get(bool clean)
{
  char sz[255];
  int i;
  string s=board;
  s+=' ';
  s+=color;
  s+=' ';
  s+=castling;
  s+=' ';
  s+=enpassant;
  s+=' ';
  if (acd&&!clean)
  {
    sprintf(sz,"acd %i; ",acd);
    s+=sz;
  }
  if (acn&&!clean)
  {
    sprintf(sz,"acn %i; ",acn);
    s+=sz;
  }
  if (acs&&!clean)
  {
    sprintf(sz,"acs %i; ",acs);
    s+=sz;
  }
  if (am.length())
  {
    s+="am ";
    s+=am;
    s+="; ";
  }
  if (bm.length())
  {
    s+="bm ";
    s+=bm;
    s+="; ";
  }
  for (i=0;i<10;i++)
  {
    if (c[i].length())
    {
      sprintf(sz,"c%i \"%s\"; ",i,c[i].c_str());
      s+=sz;
    }
  }
  if (bce&&!clean)
  {
    sprintf(sz,"ce %i; ",ce);
    s+=sz;
  }
  if (dm)
  {
    sprintf(sz,"dm %i; ",dm);
    s+=sz;
  }
  if (draw_accept)
    s+="draw_accept; ";
  if (draw_claim)
    s+="draw_claim; ";
  if (draw_offer)
    s+="draw_offer; ";
  if (draw_reject)
    s+="draw_reject; ";
  if (eco.length())
  {
    sprintf(sz,"eco \"%s\"; ",eco.c_str());
    s+=sz;
  }
  if (fmvn)
  {
    sprintf(sz,"fmvn %i; ",fmvn);
    s+=sz;
  }
  if (hmvc)
  {
    sprintf(sz,"hmvc %i; ",hmvc);
    s+=sz;
  }
  if (id.length())
  {
    sprintf(sz,"id \"%s\"; ",id.c_str());
    s+=sz;
  }
  if (nic.length())
  {
    sprintf(sz,"nic \"%s\"; ",nic.c_str());
    s+=sz;
  }
  if (pm.length()&&!clean)
  {
    s+="pm ";
    s+=pm;
    s+="; ";
  }
  if (pv.length()&&!clean)
  {
    s+="pv ";
    s+=pv;
    s+="; ";
  }
  if (rc)
  {
    sprintf(sz,"rc %i; ",rc);
    s+=sz;
  }
  if (resign)
    s+="resign; ";
  if (sm.length())
  {
    s+=sm;
    s+="; ";
  }
  if (tcgs)
  {
    sprintf(sz,"tcgs %i; ",tcgs);
    s+=sz;
  }
  if (tcri.length())
  {
    sprintf(sz,"tcri \"%s\"; ",tcri.c_str());
    s+=sz;
  }
  if (tcsi.length())
  {
    sprintf(sz,"tcsi \"%s\"; ",tcsi.c_str());
    s+=sz;
  }
  for (i=0;i<10;i++)
  {
    if (v[i].length())
    {
		sprintf(sz,"v%i \"%s\"; ",i,v[i].c_str());
      s+=sz;
    }
  }
  if (vm.length())
  {
    i=1;
    while (getWord(vm,i).length())
    {
      s+="vm ";
      s+=getWord(vm,i++);
      if (isNumber(getWord(vm,i)))
      {
         s+=" ";
         s+=getWord(vm,i++);
      }
      s+="; ";
    }
  }
  return trim(s);
}

std::string Epd::getFen()
{
  char sz[20];
  string s=board;
  s+=' ';
  s+=color;
  s+=' ';
  s+=castling;
  s+=' ';
  s+=enpassant;
  s+=' ';
  s+=_itoa_s(hmvc,sz,20,10);
  s+=' ';
  if (fmvn<1)
    s+='1';
  else
    s+=_itoa_s(fmvn,sz,20,10);
  return s;
}

int Epd::compare(Epd& e)
{
  ChessBoard b1,b2;
  b1.setFen(getFen().c_str());
  b2.setFen(e.getFen().c_str());
  return b1.compare(b2);;
}

void Epd::clear()
{
  board="";
  color="";
  castling="";
  enpassant="";
  acd=0;
  acn=0;
  acs=0;
  am="";
  bm="";
  for (int i=0;i<10;i++)
  {
    c[i]="";
    v[i]="";
  }
  ce=0;
  bce=false;
  dm=0;
  draw_accept=false;
  draw_claim=false;
  draw_offer=false;
  draw_reject=false;
  eco="";
  fmvn=0;
  hmvc=0;
  id="";
  nic="";
  pm="";
  pv="";
  rc=0;
  resign=false;
  sm="";
  tcgs=0;
  tcri="";
  tcsi="";
  vm="";
}

void Epd::copy(const Epd& e)
{
  board=e.board;
  color=e.color;
  castling=e.castling;
  enpassant=e.enpassant;
  acd=e.acd;
  acn=e.acn;
  acs=e.acs;
  am=e.am;
  bm=e.bm;
  for (int i=0;i<10;i++)
  {
    c[i]=e.c[i];
    v[i]=e.v[i];
  }
  ce=e.ce;
  bce=e.bce;
  dm=e.dm;
  draw_accept=e.draw_accept;
  draw_claim=e.draw_claim;
  draw_offer=e.draw_offer;
  draw_reject=e.draw_reject;
  eco=e.eco;
  fmvn=e.fmvn;
  hmvc=e.hmvc;
  id=e.id;
  nic=e.nic;
  pm=e.pm;
  pv=e.pv;
  rc=e.rc;
  resign=e.resign;
  sm=e.sm;
  tcgs=e.tcgs;
  tcri=e.tcri;
  tcsi=e.tcsi;
  vm=e.vm;
}

bool Epd::empty()
{
  if (board.length()<1)
    return true;
  return false;
}

void Epd::moveToSan()
{
  int i;
  ChessBoard b;
  ChessMove  m;
  char sz[20];
  string s;
  b.setFen(getFen().c_str());
  if (am.length())
  {
    i=1;
    s="";
    while (getWord(am,i).length())
    {
      m=b.getMoveFromText(getWord(am,i));
      if (!m.empty())
      {
        if (i>1)
          s+=" ";
        s+=b.makeMoveText(m,sz,20,SAN);
      }
      i++;
    }
    am=s;
  }
  if (bm.length())
  {
    i=1;
    s="";
    while (getWord(bm,i).length())
    {
      m=b.getMoveFromText(getWord(bm,i));
      if (!m.empty())
      {
        if (i>1)
          s+=" ";
        s+=b.makeMoveText(m,sz,20,SAN);
      }
      i++;
    }
    bm=s;
  }
  if (pm.length())
  {
    m=b.getMoveFromText(pm);
    if (!m.empty())
      pm=b.makeMoveText(m,sz,20,SAN);
  }
  if (pv.length())
  {
    i=1;
    s="";
    while (getWord(pv,i).length())
    {
      m=b.getMoveFromText(getWord(pv,i));
      if (!m.empty())
      {
        if (i>1)
          s+=" ";
        s+=b.makeMoveText(m,sz,20,SAN);
        b.doMove(m,false);
      }
      i++;
    }
    pv=s;
  }
}

const std::string Epd::stringValue(const std::string& s)
{
  char c;
  string value;
  int index=0,len=s.length();
  value="";
  if (len<1)
    return value;
  if (s.at(0)!='\"') // Allow single word without quote
    return getWord(s,1);
  if (len<3)
    return value;
  while ((c=s.at(index))!='\"')
  {
    index++;
    if (len<=index)
      return value;
    if (s.at(index)=='\\')
    {
      index++;
      if (len<=index)
        return value;
    }
  }

  index++;
  if (len<=index)
    return value;
  while ((c=s.at(index))!='\"')
  {
    index++;
    if (len<=index)
      return value;
    if (c=='\\')
    {
      c=s.at(index);
      index++;
      if (len<=index)
        return value;
    }
    value+=c;
    if (s.length()==index)
      return value;
  }
  return value;
}

bool GetEpd(const std::string& filename, const std::string& id, Epd& epd)
{
  WinFile f;
  string line;
  if (f.open(filename,GENERIC_READ,false,OPEN_EXISTING,false))
  {
    while (f.readLine(line))
    {
      line=trim(line);
      if (line.length() && (line.at(0)!=';'))
      {
        epd.set(line);
        if (epd.id.length() && (epd.id==id))
        {
          f.close();
          return true;
        }
      }
    };
    f.close();
  }
  epd.clear();
  return false;
}
