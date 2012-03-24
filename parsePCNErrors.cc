/**
 * @file   parsePCNErrors.cc
 * @author Suvayu Ali <Suvayu.Ali@cern.ch>
 * @date   Sat Mar 24 02:20:55 2012
 *
 * @brief  CLI utility to parse Vetra log files and dump ROOT trees.
 *         Parsing equivalent to: (parse only rows with numbers or white space)
 *         $ sed -ne '/^ *|[0-9 |]\\+| *$/ {s/|//gp}' logfile > space-separated-tempfile
 *
 * 	   compile as:
 *	   $ g++ -o parsePCNErrors -Wall $(root-config --cflags --libs) parsePCNErrors.cc
 *
 */

// STL
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <exception>
#include <cassert>

// may need later
#include <bitset>
#include <cstdio>

// ROOT classes
#include <TString.h>
#include <TTree.h>
#include <TFile.h>

// BOOST classes
#include <boost/foreach.hpp>


template <class T> void test(std::vector<T> &col);

int main(int argc, char *argv[])
{
  if (argc == 1 or argc % 2 != 1) {
    std::cout << "Insufficient/incorrect number of arguments."
	      << std::endl << std::endl;
    std::cout << "Usage: ./parsePCNErrors [options] --file <input log file>"
	      << std::endl << std::endl;
    std::cout << "   --file    Log file with input data in an ascii table, e.g."
	      << std::endl;
    std::cout << "             | col1 | col2 |" << std::endl;
    std::cout << "             |------+------|" << std::endl;
    std::cout << "             |   34 | 1010 |" << std::endl << std::endl;
    std::cout << "   --header  Table header string for TTree::ReadFile() (default below)."
	      << std::endl;
    std::cout << "             runNo/L:eventID/L:tell1/I:ExpPCN/I:Beetle/I:expbits/C:badbits/C"
	      << std::endl << std::endl;
    std::cout << "   --temp    Temporary file to use (default: /tmp/PCNErrors.txt)."
	      << std::endl << std::endl;
    std::cout << "   --output  ROOT file to dump TTree (default: PCNErrors.root)."
	      << std::endl;
    return 1;
  }

  std::vector<std::string> arguments;
  for (int i = 1; i < argc; ++i) {
    arguments.push_back(argv[i]);
  }

  // program options
  std::string inFile, tmpFile, outFile, header, cmdstring;

  assert(argc % 2);
  for (unsigned int i = 0; i < arguments.size(); i+=2) {
    TString opt(arguments[i]), value(arguments[i+1]);
    opt.ToLower();
    if ( opt.Contains("--file") )   inFile  = value.Data();
    if ( opt.Contains("--temp") )   tmpFile = value.Data();
    if ( opt.Contains("--output") ) outFile = value.Data();
    if ( opt.Contains("--header") ) header  = value.Data();
  }

  if (tmpFile == "") tmpFile = "/tmp/PCNErrors.txt";
  if (outFile == "") outFile = "PCNErrors.root";
  if (header  == "")
    header = "runNo/L:eventID/L:tell1/I:ExpPCN/I:Beetle/I:expbits/C:badbits/C";

  // parse only rows with numbers or white space
  cmdstring = "sed -ne '/^ *|[0-9 |]\\+| *$/ {s/|//gp}' "
    + inFile + " > " + tmpFile;

  if (system(cmdstring.c_str())) { // PCNErrors.org
    std::cout << "Error: sed command failed!" << std::endl;
    return 1;
  }

  TTree ftree("ftree", "PCN error tree");
  ftree.ReadFile(tmpFile.c_str(), header.c_str());

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
