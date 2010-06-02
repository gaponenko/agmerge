#include "FileMergeObjects.h"

#include <stdexcept>
#include <iostream>
#include <memory>

#include "TDirectory.h"
#include "TFile.h"
#include "TList.h"
#include "TKey.h"
#include "TH1.h"


//================================================================
FileMergeObjects::FileMergeObjects(const std::string& filename, double weight) 
  : Named("")
{
  TFile infile(filename.c_str(), "READ");
  if(!infile.IsOpen()) {
    throw std::runtime_error("FileMergeObjects(): can't open input file \""+filename+"\"");
  }
  
  std::cout<<"FileMergeObjects(): initializing from file "<<filename
	   <<", weight="<<weight
	   <<std::endl;
  init(infile, weight);
  
  infile.Close();
}

//================================================================
FileMergeObjects::FileMergeObjects(TDirectory& dir, double weight) 
  : Named(dir.GetName(), dir.GetTitle())
{
  //std::cout<<"FileMergeObjects ctr from dir "<<dir.GetName()<<std::endl;
  init(dir, weight);
}

//================================================================
void FileMergeObjects::init(TDirectory& dir, double weight) {
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
	m_subDirs.push_back(FileMergeObjects(*subdir,weight));
      } // subdir
      else {
	TH1 *hist = dynamic_cast<TH1*>(obj.get());
	if(hist) {
	  hist->SetDirectory(0);
	  hist->Scale(weight);
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
void FileMergeObjects::addFile(const std::string& filename, double weight) {
  std::cout<<"FileMergeObjects: adding file "<<filename
	   <<", weight = "<<weight
	   <<std::endl;
  TFile f(filename.c_str(), "READ");
  addDirectory(f, weight);
  f.Close();
}

//================================================================
void FileMergeObjects::addDirectory(TDirectory& dir, double weight) {
  ChdirTemp wd(&dir);  

  //std::cout<<"FileMergeObjects: adding directory "<<dir.GetName()<<std::endl;

  //----------------------------------------------------------------
  for(DirList::iterator i = m_subDirs.begin(); i!=m_subDirs.end(); ++i) {
    const std::string subdirname(i->getDirName());
    //std::cout<<"Getting subdir "<<subdirname<<std::endl;
    TDirectory *subdir = dynamic_cast<TDirectory*>(dir.Get(subdirname.c_str()));
    if(subdir) {
      i->addDirectory(*subdir, weight);
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
      (*i)->Add(*i, hh.get(), 1., weight);
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
}

//================================================================
//EOF
