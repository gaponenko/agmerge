/// Dear emacs, this is -*-c++-*-
///
/// Histogram merging code for hadd-like applications.
/// 
/// Note that objects of FileMergeObjects class
/// can not be freely copied by the client code.
/// Copy ctr is not declared private because it is used internally.
///
/// A. Gaponenko, 2002-2004

#ifndef FILEMERGEOBJECTS_H
#define FILEMERGEOBJECTS_H

#include <string>
#include <vector>

#include "RootUtils.h"

class TH1;
class TDirectory;

class FileMergeObjects : public Named {
  typedef std::vector<FileMergeObjects> DirList;
  DirList m_subDirs;

  // FIXME: use WHCloner instead of bare pointers?
  typedef std::vector<TH1*> HistList;
  HistList m_hists;

  void init(TDirectory& dir, double weight);

public:
  FileMergeObjects(const std::string& filename, double weight=1.);
  FileMergeObjects(TDirectory& dir, double weight);

  void addFile(const std::string& filename, double weight=1.);
  void addDirectory(TDirectory& dir, double weight);

  void write(TDirectory *outdir) const;
};


#endif/*FILEMERGEOBJECTS_H*/
