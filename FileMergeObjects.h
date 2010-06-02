/// Dear emacs, this is -*-c++-*-
///
/// Histogram merging code for hadd-like applications.
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

  typedef std::vector<TH1*> HistList;
  HistList m_hists;

  void init(TDirectory& dir);
public:
  FileMergeObjects(const std::string& filename);
  FileMergeObjects(TDirectory& dir);

  void addFile(const std::string& filename);
  void addDirectory(TDirectory& dir);

  void write(TDirectory *outdir) const;
};


#endif/*FILEMERGEOBJECTS_H*/
