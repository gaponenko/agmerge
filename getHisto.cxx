#include "getHisto.h"

#include <iostream>
#include <stdexcept>
#include <string>
#include "TH1.h"
#include "TFile.h"

TH1* getHisto(const std::string& filename, const std::string& histoname) {

  TFile fff(filename.c_str(), "READ");

  TObject *obj = fff.Get(histoname.c_str());
  if(!obj) {
    throw std::runtime_error("getHisto(): no object \""+histoname+"\""
			     " in file \""+filename+"\""
			     );
  }
  
  TH1* h = dynamic_cast<TH1*>(obj->Clone());
  if(!h) {
    throw std::runtime_error("getHisto(): object \""+histoname+"\""
			     " in file \""+filename+"\""
			     " is not a TH1"
			     );
  }

  h->SetDirectory(0);
  return h;
}

