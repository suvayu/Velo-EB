/**
 * @file   PCNErrorMap.cxx
 * @author Suvayu Ali <Suvayu.Ali@cern.ch>
 * @date   Sat Mar 24 22:17:37 2012
 *
 * @brief  Implementation file for PCNError and PCNErrorMap
 *
 *
 */

#include <iostream>
#include <sstream>

// for debugging
#include <cassert>
#include <exception>

#include <TCanvas.h>
#include <TAxis.h>

#include "PCNErrorMap.hxx"


//////////////////////////////
// PCNError implementations //
//////////////////////////////


PCNError::PCNError(unsigned int pcn, unsigned int pcnxor) :
  _pcn(pcn), _xor(pcnxor) {}


PCNError::PCNError(std::string pcnbits, std::string xorbits) :
  _pcn(PCNError::eightbits(pcnbits).to_ulong()),
  _xor(PCNError::eightbits(xorbits).to_ulong()) {}


PCNError::~PCNError() {}


PCNError::eightbits PCNError::getBits(_PCN_T bittype) const
{
  unsigned int value(-1);
  if (bittype == kPCN) value = _pcn;
  else if (bittype == kXOR) value = _xor;
  else if (bittype == kBAD) value = _pcn^_xor;
  else value = _pcn;
  return std::bitset<8> (value);
}


void PCNError::setBits(unsigned int pcn, unsigned int pcnxor)
{
  _pcn = pcn;
  _xor = pcnxor;
  return;
}


void PCNError::setBits(std::string pcnbits, std::string xorbits)
{
  _pcn = PCNError::eightbits(pcnbits).to_ulong();
  _xor = PCNError::eightbits(xorbits).to_ulong();
  return;
}


/////////////////////////
// Key implementations //
/////////////////////////


Key::Key(unsigned int tell1id, unsigned int beetle) :
  _key(tell1id|(beetle<<kTELL1ID)) {}


Key::~Key() {}


unsigned int Key::get(_BITWORD type) const
{
  unsigned int num(-1);
  if (type == kBEETLE) num = _key&kBEETLE;
  if (type == kTELL1ID) num = _key>>kTELL1ID;
  return num; 
}


Key::operator unsigned int () const { return _key; }


/////////////////////////////////
// PCNErrorMap implementations //
/////////////////////////////////


PCNErrorMap::PCNErrorMap(unsigned int tell1s) :
  _debug(false),
  hBeetleMap("hBeetleMap", "PCN error map", tell1s+2, -1.5, tell1s+0.5, 18, -1.5, 16.5)
{
  hBeetleMap.SetXTitle("Tell1 id");
  hBeetleMap.SetYTitle("Beetle no.");
  hBeetleMap.GetXaxis()->SetTitleSize(0.05);
  hBeetleMap.GetYaxis()->SetTitleSize(0.05);
}


PCNErrorMap::~PCNErrorMap()
{
  ErrCounter.clear();
  hperBeetleBitMap.clear();
}


void PCNErrorMap::setDebug(bool debug) { _debug = debug; }


void PCNErrorMap::Fill(unsigned int tell1id, unsigned int beetle, PCNError err)
{
  PCNError::eightbits pcnbits(err.getBits(PCNError::kPCN)),
    xorbits(err.getBits(PCNError::kXOR));

  if (xorbits.any()) {
    // assuming the map elements are initialised to 0
    unsigned int key(tell1id|(beetle<<8));
    ErrCounter[key]++;

    hBeetleMap.Fill(tell1id, beetle);

    std::stringstream coords, hnum;
    coords << "(" << tell1id << "," << beetle << ")";
    hnum   << tell1id << "_" << beetle;
    std::string hname("hperBeetleBitMap_" + hnum.str()),
      htitle("Per Beetle PCN error map " + coords.str());

    if (hperBeetleBitMap[key] == NULL) {
      int xbins(10), ybins(4);	// two empty bins on either side for aesthetic reasons
      hperBeetleBitMap[key] = new TH2D(hname.c_str(), htitle.c_str(),
				     xbins, -1.5, 8.5, ybins, -1.5, 2.5);
      hperBeetleBitMap[key]->SetXTitle("PCN bits with errors");
      hperBeetleBitMap[key]->SetYTitle("Correct value for bad PCN bit");

      // nicer axis title and labels
      TAxis *xaxis = hperBeetleBitMap[key]->GetXaxis();
      TAxis *yaxis = hperBeetleBitMap[key]->GetYaxis();

      std::stringstream lbl;
      for(int i = 1; i <= xbins; ++i) {
	if (i == 1 or i == xbins) lbl.str("");
	else lbl << xbins-1-i;
	xaxis->SetBinLabel(i, lbl.str().c_str());
	lbl.str("");
      }
      xaxis->SetLabelSize(0.06);
      xaxis->SetTitleSize(0.05);

      for(int i = 1; i <= ybins; ++i) {
	if (i == 1 or i == ybins) lbl.str("");
	else lbl << i-2;
	yaxis->SetBinLabel(i, lbl.str().c_str());
	lbl.str("");
      }
      yaxis->SetLabelSize(0.06);
      yaxis->SetTitleSize(0.05);
    }

    // store bits in the reverse order (MSB -> LSB)
    for (unsigned int i = 0; i < 8; ++i) { // 8 bits
      if (xorbits.test(7-i)) hperBeetleBitMap[key]->Fill(i,pcnbits[7-i]);
    }
  }
  return;
}


void PCNErrorMap::Draw(std::string opts)
{
  unsigned int nhists(hperBeetleBitMap.size() + 1), csize(0);
  if (nhists % 2) csize = nhists + 1;
  else csize = nhists;

  TCanvas *canvas = new TCanvas("canvas", "PCN error maps", 1600, 200*csize);
  canvas->Divide(2, csize/2);

  canvas->cd(1);
  hBeetleMap.Draw(opts.c_str());

  TH2DMap::const_iterator histItr = hperBeetleBitMap.begin();
  for(unsigned int i = 2; i <= nhists; ++i) {
    canvas->cd(i);
    histItr->second->Draw(opts.c_str());
    if (_debug) histItr->second->Print("all");
    ++histItr;
  }
  return;
}
