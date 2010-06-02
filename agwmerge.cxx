// Combine histogram files using the specified weights.
// 
// Andrei Gaponenko, 2010


#include <string>
#include <stdexcept>
#include <iostream>
#include <cstdlib>
#include <memory>
#include <utility>
#include <sstream>

#include "TFile.h"
#include "TH1.h"

#include "InputFileGetter.h"
#include "RootUtils.h"
#include "FileMergeObjects.h"

//================================================================
namespace {
  double getDouble(const std::string& str) {
    std::istringstream is(str);
    double res(0);
    if(!(is>>res)) {
      throw std::runtime_error("ERROR: expected number, got \""+str+"\"");
    }
    std::string tmp;
    if(is>>tmp) {
      throw std::runtime_error("ERROR: trailing garbage after the number: \""+str+"\"");
    }
    return res;
  }


  //----------------------------------------------------------------
  typedef std::pair<double, std::string> WFPair;
  WFPair nextPair(InputFileGetter& fg) {
    std::string wstr = fg.get();
    std::string filename = fg.get();
    if(!wstr.empty() && filename.empty()) {
      throw std::runtime_error("ERROR: got weight \"+wstr+\" but no filename");
    }

    double w = filename.empty() ? 0. : getDouble(wstr);
    return make_pair(w, filename);
  }
}

//================================================================
std::string usage() {
  return 
    "Usage:\n"
    "       agwmerge outfile.root w1 infile1.root w2 infile2.root ...\n"
    "\n"
    "or do\n"
    "\n"
    "      agwmerge outfile.root --stdin\n"
    "\n"
    "and feed in weight and file name pairs.\n"
    ;
}

//================================================================
int main(int argc, char* const argv[]) {
  TH1::AddDirectory(kFALSE);

  for(int i=1; i<argc; ++i) {
    if( (argv[i] == std::string("-h")) ||
	(argv[i] == std::string("--help")) ) {
      std::cout<<usage()<<std::endl;
      exit(1);
    }
  }

  if(argc<3) {
    std::cerr<<usage()<<std::endl;
    exit(2);
  }

  try {
    TFile outfile(argv[1], "NEW");
    if(!outfile.IsOpen()) {
      throw std::runtime_error("Can't open output file \""+std::string(argv[1])+"\"");
    }
    
    unsigned const offset = 2; // [1]==outfile
    InputFileGetter files(argc - offset, argv + offset);

    double w1 = getDouble(files.get());
    const std::string firstInFile = files.get();
    if(firstInFile.empty()) {
      throw std::runtime_error("ERROR: first input file name is an empty string");
    }
    FileMergeObjects fo(firstInFile, w1);

    for(WFPair inputs = nextPair(files); 
	!inputs.second.empty();
	inputs = nextPair(files) )
      {
	fo.addFile(inputs.second, inputs.first);
      }
    
    std::cout<<"Writing output\t"<<std::endl;
    fo.write(&outfile);

    std::cout<<"Closing output file \t"<<std::endl;
    outfile.Close();
  }
  catch(std::exception& e) {
    std::cerr<<"ERROR: got std::exception: "<<e.what()<<std::endl;
  }
  catch(...) {
    std::cerr<<"ERROR: got a non-standard exception"<<std::endl;
  }

  return 0;
}

//================================================================
