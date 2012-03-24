#include <iostream>
#include <vector>
#include <fstream>
#include <cassert>
#include <algorithm>
#include <cctype>
#include <bitset>

#include <TRegexp.h>
#include <TObjArray.h>
#include <TObjString.h>

#include "utils.hh"


TStyle* Style::setStyle()
{
  gStyle->SetOptStat(0);
  gStyle->SetOptTitle(1);
  gStyle->SetPalette(1); // "rainbow" color palette
  gStyle->SetNumberContours(256); // smooth color palette
  gStyle->SetTitleOffset( 1.2, "xy");
  gStyle->SetCanvasPreferGL(true);
  return gStyle;
}


void Hist::rescale(TH1 *h, Double_t factor)
{
  // change X scale
  Double_t xmin = h->GetXaxis()->GetXmin();
  Double_t xmax = h->GetXaxis()->GetXmax();
  h->GetXaxis()->SetLimits(xmin*factor,xmax*factor);
  return;
}


bool Parsers::PrintOpts(TString& opt, TString& format)
{
  opt.ToLower();
  bool doPrint(false);
  if ( opt.Contains("print") ) {
    doPrint = true;
    if ( opt.Contains("png") )          format = "png";
    else if ( opt.Contains("jpg") )     format = "jpg";
    else if ( opt.Contains("ps") )      format = "ps";
    else if ( opt.Contains("pdf") )     format = "pdf";
    else if ( opt.Contains("cscript") ) format = "C";
    else {
      std::cout << "Error   Parsers::PrintOpts(): Bad print option!"
		<< " No known formats found.\n"
		<< "Warning Parsers::PrintOpts(): Printing will be skipped."
		<< std::endl;
      doPrint = false;
    }
  }
  return doPrint;
}


void Parsers::readconf(std::vector<TString> &var, std::vector<TString> &val, std::string fname)
{
  ifstream inFile(fname.c_str());

  while (! inFile.eof()) {
    TString tmp;
    tmp.ReadToken(inFile);
    if (tmp.BeginsWith("#") or tmp.IsWhitespace()) {
      tmp.ReadLine(inFile);
      continue;
    }
    var.push_back(tmp);
    tmp.ReadToken(inFile);
    val.push_back(tmp);
  }
  return;
}


void Parsers::readlist(std::vector<TString> &var, std::string fname)
{
  ifstream inFile(fname.c_str());

  while (! inFile.eof())
    {
      TString tmp;
      tmp.ReadToken(inFile);
      if (tmp.BeginsWith("#") or tmp.IsWhitespace())
        {
          tmp.ReadLine(inFile);
          continue;
        }
      var.push_back(tmp);
    }
  return;
}


std::string& Parsers::replaceAll(std::string& context, const std::string& from, const std::string& to)
{
  size_t lookHere = 0;
  size_t foundHere;
  while((foundHere = context.find(from, lookHere)) != std::string::npos) {
    context.replace(foundHere, from.size(), to);
    lookHere = foundHere + to.size();
  }
  return context;
}


void Parsers::readtable(std::string var, std::vector<std::string> &col, std::string fname)
{
  ifstream inFile(fname.c_str());
  int column(-1);

  while (! inFile.eof()) {
    TString tmp;
    tmp.ReadLine(inFile);
    TRegexp table("^ *|.+| *$"), separator("^ *|[-+]+| *$"), number("^[0-9 ]+$");

    if (tmp.Contains(separator)) continue;
    if (tmp.Contains(table)) {
	TObjArray *tokenArray = tmp.Tokenize("|");
	TString token(dynamic_cast<TObjString*>((*tokenArray)[0])->GetString());
	assert(token);

	if (token.Contains(number) == false) {			  // check if header
	  for(int i = 0; i < tokenArray->GetEntriesFast(); ++i) { // find column
	    token = dynamic_cast<TObjString*>((*tokenArray)[i])->GetString();
	    if (token.Contains(var.c_str())) {
	      column = i;
	      break;
	    }
	  } // end of header loop
	} else {
	  if (column < 0) continue; // first line of table has to be header
	  token = dynamic_cast<TObjString*>((*tokenArray)[column])->GetString();
	  std::string nowspcstr(token.Data());
	  nowspcstr.erase(remove_if(nowspcstr.begin(), nowspcstr.end(), isspace), nowspcstr.end());
	  col.push_back(nowspcstr);
	}
	tokenArray->Delete();
    } else {
	continue;
    }
  }

  return;
}
