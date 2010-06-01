#include <string>
#include <stdexcept>

#include "TDirectory.h"

#include "RootUtils.h"

//#include <iostream>

//================================================================
ChdirTemp::ChdirTemp(TDirectory* newdir) {
  if(!gDirectory) {
    throw std::runtime_error("ChdirTemp(dir): gDirectory==0");
  }
  saved = gDirectory;

  if(newdir->cd() != kTRUE) {
    throw std::runtime_error("ChdirTemp(dir): Could not cd() to the new dir.");
  }
}

//================================================================
ChdirTemp::~ChdirTemp() {
  if(saved) {
    //std::cerr<<"AG: ~ChdirTemp(): "<<__FILE__<<", line "<<__LINE__<<std::endl;
    saved->cd(); // no exceptions in destructors, but see cd_back()
  }
}

//================================================================
void ChdirTemp::cd_back() {
  if(saved->cd() != kTRUE) {
    throw std::runtime_error("ChdirTemp::cd_back(): could not cd");
  }
}

//================================================================
ChdirTemp::ChdirTemp(const std::string&dirname) {
  if(!gDirectory) {
    throw std::runtime_error("ChdirTemp(string): gDirectory==0");
  }    
  saved = gDirectory;
  
  if(gDirectory->cd(dirname.c_str()) != kTRUE) {
    throw std::runtime_error("ChdirTemp(string): Could not cd() to subdir '"+dirname+"'");
  }
}

//================================================================
std::string rootDirName(const std::string& path) {
  std::string::size_type slashpos = path.rfind('/');
  return (slashpos == std::string::npos) ? "" : path.substr(0,slashpos);
}

//================================================================
std::string rootBaseName(const std::string& path) {
  std::string::size_type slashpos = path.rfind('/');
  return (slashpos == std::string::npos) ? path : path.substr(slashpos+1);
}

//================================================================
//EOF
