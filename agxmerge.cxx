// Combine histogram files with weights that are computed from the
// specified cross sections.  Another ingredient for the weight
// computation, the number of processed events from the sample, is
// automatically looked up from the input files.
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
#include "getHisto.h"

//================================================================
namespace {
  template<class Number>
  Number getNumber(const std::string& str) {
    std::istringstream is(str);
    Number res(0);
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

    double w = filename.empty() ? 0. : getNumber<double>(wstr);
    return make_pair(w, filename);
  }

  //----------------------------------------------------------------
  double getNumEvents(const std::string& filename,
		      const std::string& refhisto,
		      unsigned refbin)
  {
    std::auto_ptr<TH1> hh(getHisto(filename, refhisto));
    if((refbin<1u) || (refbin > unsigned(hh->GetNbinsX()))) {
      throw std::runtime_error("getNumEvents(): refbin out of range");
    }

    return hh->GetBinContent(refbin);
  }

  //----------------------------------------------------------------
  double computeWeight(double targetLumi, double xsec, double numEvents) {
    // L(target)/L(sample)
    return targetLumi * xsec / numEvents;
  }

}

//================================================================
std::string usage() {
  return 
    "Usage:\n"
    "       agxmerge --targetLumi L \\\n"
    "                --refh path/histo --refbin int \\\n"
    "                --out outfile.root \\\n"
    "                  xs1 infile1.root xs2 infile2.root ...\n"
    "\n"
    "or do\n"
    "\n"
    "      agxmerge --targetLumi L \n"
    "               --refh path/histo --refbin int \\\n"
    "               --out outfile.root \\\n"
    "               --stdin\n"
    "\n"
    "and feed in cross section and file name pairs.\n"
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

  if(argc<10) {
    std::cerr<<"Too few arguments"<<std::endl;
    exit(2);
  }

  if(std::string(argv[1]) != "--targetLumi") {
    std::cerr<<"Wrong arg1: should be \"--targetLumi\""<<std::endl;
    exit(2);
  }

  if(std::string(argv[3]) != "--refh") {
    std::cerr<<"Wrong arg3: should be \"--refh\""<<std::endl;
    exit(2);
  }

  if(std::string(argv[5]) != "--refbin") {
    std::cerr<<"Wrong arg5: should be \"--refbin\""<<std::endl;
    exit(2);
  }

  if(std::string(argv[7]) != "--out") {
    std::cerr<<"Wrong arg7: should be \"--out\""<<std::endl;
    exit(2);
  }

  try {
    double targetLumi = getNumber<double>(argv[2]);
    
    const std::string refhisto(argv[4]);
    const unsigned refbin(getNumber<unsigned>(argv[6]));
    const std::string outfilename(argv[8]);
    
    TFile outfile(outfilename.c_str(), "NEW");
    if(!outfile.IsOpen()) {
      throw std::runtime_error("Can't open output file \""+std::string(argv[1])+"\"");
    }
    
    unsigned const offset = 9; // [8]==outfile
    InputFileGetter files(argc - offset, argv + offset);

    double xs1 = getNumber<double>(files.get());
    const std::string firstInFile = files.get();
    if(firstInFile.empty()) {
      throw std::runtime_error("ERROR: first input file name is an empty string");
    }
    double numEvents1 = getNumEvents(firstInFile, refhisto, refbin);
    double w1 = computeWeight(targetLumi, xs1, numEvents1);
    FileMergeObjects fo(firstInFile, w1);

    for(WFPair inputs = nextPair(files); 
	!inputs.second.empty();
	inputs = nextPair(files) )
      {
	double numEvents = getNumEvents(inputs.second, refhisto, refbin);
	double w = computeWeight(targetLumi, inputs.first, numEvents);
	fo.addFile(inputs.second, w);
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
