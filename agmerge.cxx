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
#include "TDirectory.h"
#include "TList.h"
#include "TKey.h"
#include "TH1.h"

#include "InputFileGetter.h"
#include "RootUtils.h"

//================================================================
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

//================================================================
FileMergeObjects::FileMergeObjects(const std::string& filename) 
  : Named("")
{
  TFile infile(filename.c_str(), "READ");
  if(!infile.IsOpen()) {
    throw std::runtime_error("FileMergeObjects(): can't open input file \""+filename+"\"");
  }
  
  std::cout<<"FileMergeObjects(): initializing from file "<<filename<<std::endl;
  init(infile);
  
  infile.Close();
}

//================================================================
FileMergeObjects::FileMergeObjects(TDirectory& dir) 
  : Named(dir.GetName(), dir.GetTitle())
{
  //std::cout<<"FileMergeObjects ctr from dir "<<dir.GetName()<<std::endl;
  init(dir);
}

//================================================================
void FileMergeObjects::init(TDirectory& dir) {
  TList *keyList = dir.GetListOfKeys();
  if(keyList) {
    //std::cerr<<"Printing key list of length "<<keyList->GetSize()<<std::endl;
    //keyList->Print();

    //----------------------------------------------------------------
    TIter it(keyList);
    TKey *key(0);
    while( key = (TKey*)it() ) {
      // std::cout<<"init(): key name = "<<key->GetName()<<", title = "<<key->GetTitle()<<std::endl;
      
      std::auto_ptr<TObject> obj(dir.Get(key->GetName()));
      
      TDirectory *subdir = dynamic_cast<TDirectory*>(obj.get());
      if(subdir) {
	// std::cout<<"init(): adding subdir for key name = "<<key->GetName()<<", title = "<<key->GetTitle()<<std::endl;
	m_subDirs.push_back(*subdir);
      } // subdir
      else {
	TH1 *hist = dynamic_cast<TH1*>(obj.get());
	if(hist) {
	  hist->SetDirectory(0);
	  m_hists.push_back(hist);
	  obj.release();
	} // hist
	else {
	  std::cout<<"WARNING: ignoring non-TH1 object for key name = "
		   <<key->GetName()<<", title = "<<key->GetTitle()
		   <<std::endl;
	}
      } // not subdir
    } // while(key)
  }
  else {
    throw std::runtime_error("FileMergeObjects(): NULL key list from TDirectory::GetListOfKeys()");
  }
}

//================================================================
void FileMergeObjects::addFile(const std::string& filename) {
  std::cout<<"FileMergeObjects: adding file "<<filename<<std::endl;
  TFile f(filename.c_str(), "READ");
  addDirectory(f);
  f.Close();
}

//================================================================
void FileMergeObjects::addDirectory(TDirectory& dir) {
  ChdirTemp wd(&dir);  

  //std::cout<<"FileMergeObjects: adding directory "<<dir.GetName()<<std::endl;

  //----------------------------------------------------------------
  for(DirList::iterator i = m_subDirs.begin(); i!=m_subDirs.end(); ++i) {
    const std::string subdirname(i->getDirName());
    //std::cout<<"Getting subdir "<<subdirname<<std::endl;
    TDirectory *subdir = dynamic_cast<TDirectory*>(dir.Get(subdirname.c_str()));
    if(subdir) {
      i->addDirectory(*subdir);
    }
    else {
      throw std::runtime_error("FileMergeObjects::addDirectory(): NULL for subdir \""+subdirname+"\"");
    }
  }

  //----------------------------------------------------------------
  for(HistList::iterator i = m_hists.begin(); i!=m_hists.end(); ++i) {
    const std::string histname((*i)->GetName());
    //std::cout<<"Getting histo "<<histname<<std::endl;
    std::auto_ptr<TH1> hh(dynamic_cast<TH1*>(dir.Get(histname.c_str())));
    if(hh.get()) {
      // Note: 
      //   TH1::Add(TH1*, double) is affected by TH1::Scale()
      //   TH1::Add(TH1*, TH1*, double, double) is not
      (*i)->Add(*i, hh.get(), 1., 1.);
    }
    else {
      throw std::runtime_error("FileMergeObjects::addDirectory(): NULL for histo \""+histname+"\"");
    }
  }

  //----------------
  wd.cd_back();
}

//================================================================
void FileMergeObjects::write(TDirectory *dir) const {
  ChdirTemp wd(dir);
  TDirectory *subdir(dir);

  //----------------------------------------------------------------
  if(getDirName().length()) {
    subdir = dir->mkdir(getDirName().c_str(), getDirTitle().c_str());
    if(!subdir) {
      throw std::runtime_error("FileMergeObjects::write(): Could not do TDirectory::mkdir()");
    }
    if(subdir->cd() != kTRUE) {
      throw std::runtime_error("FileMergeObjects::write(): Could not cd() to subdir");
    }
  }

  //----------------------------------------------------------------
  for(DirList::const_iterator i=m_subDirs.begin(); i!=m_subDirs.end(); ++i) {
    if(subdir->cd() != kTRUE) {
      throw std::runtime_error("FileMergeObjects::write(): Could not cd() to subdir");
    }
    i->write(subdir);
  }
  
  for(HistList::const_iterator i=m_hists.begin(); i!=m_hists.end(); ++i) {
    (*i)->Write();
  }

  wd.cd_back();

  if(subdir != dir) {
    delete(subdir);
  }
}

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
    
    std::cout<<"Writing output: \t"<<std::endl;
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
