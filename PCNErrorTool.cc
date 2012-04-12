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
#include <TString.h>

#include "PCNErrorMap.hxx"


int main(int argc, char *argv[])
{
  if (argc == 1 or argc % 2 != 1) {
    std::cout << "Insufficient/incorrect number of arguments."
	      << std::endl << std::endl;
    std::cout << "Usage: ./makePCNErrorMap --input <input ROOT file>"
	      << std::endl << std::endl;
    std::cout << "   --input   Input ROOT file with TTree (compulsory argument)."
	      << std::endl;
    return 1;
  }

  std::vector<std::string> arguments;
  for (int i = 1; i < argc; ++i) {
    arguments.push_back(argv[i]);
  }

  // program options
  std::string inFile;

  assert(argc % 2);
  for (unsigned int i = 0; i < arguments.size(); i+=2) {
    TString opt(arguments[i]), value(arguments[i+1]);
    opt.ToLower();
    if ( opt.Contains("--input") )   inFile  = value.Data();
  }

  TFile file(inFile.c_str(), "read");
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
