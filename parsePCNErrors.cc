#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <bitset>
#include <exception>
#include <typeinfo>

#include <TString.h>
#include <TH2D.h>
#include <TPad.h>
#include <TCanvas.h>
#include <TTree.h>
#include <TFile.h>

#include <boost/foreach.hpp>

// #include "utils.hh"


template <class T> void test(std::vector<T> &col);

int main(int argc, char *argv[])
{
  if (argc == 1 or argc % 2 != 1) {
    std::cout << "Insufficient/incorrect number of arguments." << std::endl; 
    return 1;
  }

  std::vector<std::string> arguments;
  for (int i = 1; i < argc; ++i) {
    arguments.push_back(argv[i]);
  }

  // program options
  std::string inFile, tmpFile, outFile, cmdstring;

  assert(argc % 2);
  for (unsigned int i = 0; i < arguments.size(); i+=2) {
    TString opt(arguments[i]), value(arguments[i+1]);
    opt.ToLower();
    if ( opt.Contains("--file") )   inFile  = value.Data();
    if ( opt.Contains("--temp") )   tmpFile = value.Data();
    if ( opt.Contains("--output") ) outFile = value.Data();
  }

  if (tmpFile == "") tmpFile = "/tmp/PCNErrors.txt";
  if (outFile == "") outFile = "PCNErrors.root";

  cmdstring = "sed -ne '/^ *|[0-9 |]\\+| *$/ {s/|//gp}' "
    + inFile + " > " + tmpFile;

  if (system(cmdstring.c_str())) { // PCNErrors.org
    std::cout << "Error: sed command failed!" << std::endl;
    return 1;
  }

  TTree ftree("ftree", "PCN error tree");
  ftree.ReadFile(tmpFile.c_str(),
		 "runNo/L:eventID/L:tell1/I:ExpPCN/I:Beetle/I:expbits/C:badbits/C");

  TFile file(outFile.c_str(), "recreate");
  ftree.Write();
  file.Close();

  system(std::string("rm -rf " + tmpFile).c_str());

  return 0;
}


template <class T> void test(std::vector<T> &col)
{
  std::vector<std::string> val;
  val.push_back("00000");
  val.push_back("00001");
  val.push_back("00010");
  val.push_back("00011");

  BOOST_FOREACH(std::string str, val) {
    col.push_back(T(str));
  }

  return;
}


  // std::vector<std::string> val;
  // try {
  //   Parsers::readtable( "bad bits", val, inFile);
  // } catch (std::exception &e) {
  //   std::cout << e.what() << std::endl;
  // }

  // BOOST_FOREACH(std::string bitstring, val){
  //   try {
  //     std::bitset<8> bits(bitstring);
  //     std::cout << bits.to_string<char>() << ", " << typeid(bits).name() << std::endl;
  //   } catch (std::exception &e) {
  //     std::cout << e.what() << std::endl;
  //   }
  // }
  // val.clear();
