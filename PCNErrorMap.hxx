#ifndef __PCNERRORMAP_HXX
#define __PCNERRORMAP_HXX


#include <string>
#include <bitset>
#include <map>

#include <TH2D.h>
// #include <TCanvas.h>


class PCNError {
public:

  enum _PCN_T { kPCN, kXOR, kBAD };
  typedef std::bitset<8> eightbits;

  PCNError(unsigned int pcn, unsigned int pcnxor);
  PCNError(std::string pcnbits, std::string xorbits);
  ~PCNError();

  eightbits getBits(_PCN_T bittype=kPCN);
  void      setBits(unsigned int pcn, unsigned int pcnxor);
  void      setBits(std::string pcnbits, std::string xorbits);

  friend class PCNErrorMap;

private:

  unsigned int _pcn;
  unsigned int _xor;
};


class PCNErrorMap {
public:

  typedef std::map<unsigned int, TH2D*> TH2DMap;
  typedef std::map<unsigned int, long>  longMap;

  PCNErrorMap(unsigned int tell1s);
  ~PCNErrorMap();

  void Fill(unsigned int tell1id, unsigned int beetle, PCNError err);
  void Draw(std::string opts);

private:

  // counters
  longMap ErrCounter;

  // PCN error maps
  TH2D    hBeetleMap;
  TH2DMap hperBeetleBitMap;
};


#endif	// __PCNERRORMAP_HXX
