/**
 * @file   PCNErrorTool.cc
 * @author Suvayu Ali <Suvayu.Ali@cern.ch>
 * @date   Sat Mar 24 21:14:53 2012
 * 
 * @brief  Make PCN error maps from dumped tree
 *
 *         compile as:
 *         $ g++ -o makePCNErrorMap -Wall $(root-config --cflags --libs) PCNErrorTool.cc PCNErrorMap.cxx
 * 
 */

// STL
#include <iostream>
#include <string>
#include <sstream>

// for debugging
#include <cassert>
#include <exception>

// ROOT classes
#include <TH2D.h>
#include <TCanvas.h>
#include <TTree.h>
#include <TFile.h>
#include <TStyle.h>
#include <TROOT.h>

#include "PCNErrorMap.hxx"


int main()
{
  TFile file("PCNErrors.root", "read");
  TTree *ftree = dynamic_cast<TTree*>(file.Get("ftree"));

  long long runNo, eventID;
  int tell1, Beetle, ExpPCN;
  char cexpbits[9], cbadbits[9]; // 8 bits + null string
  std::string expbits, badbits;

  ftree->SetBranchAddress("runNo"  , &runNo  );
  ftree->SetBranchAddress("eventID", &eventID);
  ftree->SetBranchAddress("tell1"  , &tell1  );
  ftree->SetBranchAddress("Beetle" , &Beetle );
  ftree->SetBranchAddress("ExpPCN" , &ExpPCN );
  ftree->SetBranchAddress("expbits", cexpbits);
  ftree->SetBranchAddress("badbits", cbadbits);

  unsigned long nentries(ftree->GetEntries());

  PCNErrorMap *errmap = new PCNErrorMap(128); // excluding the 4 pileup sensors
  for (unsigned int i = 0; i < nentries; ++i) {
    ftree->GetEntry(i);

    try {
      expbits = cexpbits;
      badbits = cbadbits;

      PCNError err(expbits,badbits);
      errmap->Fill(tell1, Beetle, err);
    } catch (std::exception &e) {
      std::cout << e.what() << std::endl;
    }
  }

  gStyle->SetOptStat(0);
  gStyle->SetPalette(1);
  gStyle->SetNumberContours(256);
  errmap->Draw("colz");

  TCanvas *canvas = dynamic_cast<TCanvas*>(gROOT->FindObject("canvas"));
  canvas->Print(".png");

  // house cleaning
  delete errmap;
  file.Close();
  return 0;
}
