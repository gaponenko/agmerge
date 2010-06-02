// Histogram merging application.  Like ROOT's hadd but with cleaner
// code and without a limit on the number of input histo files.
// On the other hand I only merge histograms and don't care about
// other types of objects - hadd does a more careful job there.
// 
// Andrei Gaponenko, 2010


#include <string>
#include <stdexcept>
#include <iostream>
#include <cstdlib>
#include <memory>

#include "TFile.h"
#include "TH1.h"

#include "InputFileGetter.h"
#include "RootUtils.h"
#include "FileMergeObjects.h"

//================================================================
std::string usage() {
  return 
    "Usage:\n"
    "       agmerge outfile.root infile1.root infile2.root ...\n"
    "\n"
    "or do\n"
    "\n"
    "      agmerge outfile.root --stdin\n"
    "\n"
    "and feed it the file names.\n"
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

    const std::string firstInFile = files.get();
    if(firstInFile.empty()) {
      throw std::runtime_error("ERROR: first input file name is an empty string");
    }

    FileMergeObjects fo(firstInFile);
    for(std::string infile = files.get(); !infile.empty(); infile = files.get()) {
      fo.addFile(infile);
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
