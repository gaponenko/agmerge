// Dear emacs, this is -*-c++-*-

// Since ROOT writes to "the current directory", one has to remember 
// to cd() all the time to the right one.  
// ChdirTemp helps to keep track of the calls. 
// It can be used, for example, in write(TDirectory *) function of an 
// object which calls ROOT's Write() on ROOT objects.
//
// A.Gaponenko, 2004


#ifndef ROOTUTILS_H
#define ROOTUTILS_H

#include <string>

class TDirectory;

//================================================================
class ChdirTemp {
  TDirectory *saved;
public:
  ChdirTemp(TDirectory* newdir);
  ChdirTemp(const std::string&dirname);

  ~ChdirTemp();

  void cd_back();

};

//================================================================
class Named {
  std::string dirname;
  std::string dirtitle;
public:
  Named(const std::string dn, const std::string dt="") : dirname(dn), dirtitle(dt) {
    if(!dirtitle.length()) {
      dirtitle = dirname;
    }
  }

  const std::string& getDirName() const { return dirname; }
  const std::string& getDirTitle() const { return dirtitle; }
  void setNameTitle(const std::string& newname, const std::string &newtitle = "") {
    dirname = newname;
    dirtitle = newtitle;
    if(!dirtitle.length()) {
      dirtitle = dirname;
    }
  }
  
};

//================================================================
// "" insead of "." given by unix utils
std::string rootDirName(const std::string& path);
std::string rootBaseName(const std::string& path);

//================================================================

#endif/*ROOTUTILS_H*/
