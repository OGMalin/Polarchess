#ifndef __Epd_h
#define __Epd_h

#include <string>
#include <map>

class Epd
{
public:
  std::string board;
  std::string color;
  std::string castling;
  std::string enpassant;
  int acd;               // analysis count: depth (Dann Corbit)
  int acn;               // analysis count: nodes
  int acs;               // analysis count: seconds
  std::string am;        // avoid move(s)
  std::string bm;        // best move(s)
  std::string c[10];     // comments (string values)
  int ce;                // centipawn evaluation
  bool bce;              // ce in use;
  int dm;                // direct mate fullmove count
  bool draw_accept;      // accept a draw offer
  bool draw_claim;       // claim a draw
  bool draw_offer;       // offer a draw
  bool draw_reject;      // reject a draw offer
  std::string eco;       // 'Encyclopedia of Chess Openings' opening code (string value)
  int fmvn;              // fullmove number
  int hmvc;              // halfmove clock
  std::string id;        // position identification (string value)
  std::string nic;       // 'New In Chess' opening code (string value)
  std::string pm;        // predicted move
  std::string pv;        // predicted variation
  int rc;                // repetition count
  bool resign;           // game resignation
  std::string sm;        // supplied move
  int tcgs;              // telecommunication: game selector
  std::string tcri;      // telecommunication: receiver identification
  std::string tcsi;      // telecommunication: sender identification
  std::string v[10];     // variation name
  std::string vm;        // CA's extention of epd
  Epd(){clear();};
  Epd(const Epd& e){copy(e);};
  virtual ~Epd(){};
  void set(const std::string& s);
  std::string get(bool clean=false);
  std::string getFen();
  int compare(Epd& e);
  void clear();
  void copy(const Epd& e);
  bool empty();
  void moveToSan();
  const std::string stringValue(const std::string& s);
};

extern bool GetEpd(const std::string& filename, const std::string& id, Epd& epd);

#endif // __Epd_h