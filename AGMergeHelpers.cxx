#include "AGMergeHelpers.h"

// Fix urgent stuff without rewriting ROOT
#define private public
#define protected public
#include "TH1.h"
#include "TProfile.h"
#include "TProfile2D.h"
#include "TProfile3D.h"
#undef protected
#undef private

#include <string>
#include <stdexcept>

// Here is a part of the ROOT class hierarchy that
// we want to handle.  Check types from the more specific
// to the more general, giving preference to the more
// frequently used 1 than 2 than 3-dimensional histos.
//
// TH1 <-- TH1D <-- TProfile
//   ^
//   L---  TH1F
//   |
//   L---  TH2  <-- TH2D <-- TProfile2D
//   |
//   L---  TH2F
//   |
//   L---  TH3  <-- TH3D <-- TProfile3D
//   |
//   L---  TH3F


namespace agmerge {

  //================================================================
  // Profile cases:  brain dead ROOT places 
  // into fSumw2 a variable linear in weight!
  //
  // (fArray)       w*y    ==> weight * w*y
  // (fSumw2)       w*y*y  ==> weight * w*y*y
  // (fBinEntries)  w      ==> weight * w
  // (fBinSumw2)    w2     ==> weight*weight*w2
  template<class T> 
  bool applyWeightProfile(TH1* histo, double weight) {
    T* hh = dynamic_cast<T*>(histo);
    if(hh) {

      if(!hh->fBinSumw2.fN) {
	hh->Sumw2(); // ROOT 5.22.00/h did not have fBinSumw2 array at all, create it here
      }

      const double w2 = weight*weight;
      for(int i=0; i<hh->fN; ++i) {
	hh->fArray[i] *= weight;
	hh->fBinEntries.fArray[i] *= weight;
	hh->fSumw2.fArray[i] *= weight;
	hh->fBinSumw2.fArray[i] *= w2;
      }

      return true;
    }
    return false;
  }

  //----------------------------------------------------------------
  // "Normal" TH cases: just do
  // 
  // (fArray)   sumw  ==> weight * sumw
  // (fSumw2)   sumw2 ==> weight*weight * sumw2

  template<class T> 
  bool applyWeightHisto(TH1* histo, double weight) {
    T* hh = dynamic_cast<T*>(histo);
    if(hh) {

      for(int i=0; i<hh->fN; ++i) {
	hh->fArray[i] *= weight;
      }

      const double w2 = weight*weight;
      if(hh->fSumw2.fN) {
	for(int i=0; i<hh->fN; ++i) {
	  hh->fSumw2.fArray[i] *= w2;
	}
      }

      return true;
    }
    return false;
  }

  
  //----------------------------------------------------------------
  void applyWeight(TH1* histo, double weight) {
    if(! (applyWeightProfile<TProfile3D>(histo, weight) ||
	  applyWeightProfile<TProfile2D>(histo, weight) ||
	  applyWeightProfile<TProfile>(histo, weight) ||
	  applyWeightHisto<TH1D>(histo, weight) ||
	  applyWeightHisto<TH1F>(histo, weight) ||
	  applyWeightHisto<TH2D>(histo, weight) ||
	  applyWeightHisto<TH2F>(histo, weight) ||
	  applyWeightHisto<TH3D>(histo, weight) ||
	  applyWeightHisto<TH3F>(histo, weight) 
	  ) )
      {
	throw std::runtime_error("agmerge::applyWeight(TH1,weight): suport for objects "
				 "of class "+std::string(histo->ClassName())+" is not implemented"
				 );
      }
  }

  //================================================================
  template<class T> 
  bool addToProfile(TH1* h1, TH1* h2, double weight) {
    T* lhs = dynamic_cast<T*>(h1);
    T* rhs = dynamic_cast<T*>(h2);
    if(lhs && rhs) {
      if(lhs->fN != rhs->fN) {
	throw std::runtime_error("agmerge::addToProfile(TH1*, TH1*, double): "
				 "incompatible histos - different num bins"
				 );
      }

      if(!lhs->fBinSumw2.fN) {
	lhs->Sumw2();
	rhs->Sumw2();
      }

      const double w2 = weight*weight;
      for(int i=0; i<lhs->fN; ++i) {
	lhs->fArray[i] += weight * rhs->fArray[i];
	lhs->fBinEntries.fArray[i] += weight * rhs->fBinEntries.fArray[i];
	lhs->fSumw2.fArray[i] += weight * rhs->fSumw2.fArray[i];
	lhs->fBinSumw2.fArray[i] += w2 * rhs->fBinSumw2.fArray[i];
      }

      return true;
    }
    return false;
  }

  //----------------------------------------------------------------
  template<class T> 
  bool addToHisto(TH1* h1, TH1* h2, double weight) {
    T* lhs = dynamic_cast<T*>(h1);
    const T* rhs = dynamic_cast<const T*>(h2);
    if(lhs && rhs) {

      if(lhs->fN != rhs->fN) {
	throw std::runtime_error("agmerge::addToHisto(TH1*, TH1*, double): "
				 "incompatible histos - different num bins"
				 );
      }

      for(int i=0; i<lhs->fN; ++i) {
	lhs->fArray[i] += weight * rhs->fArray[i];
      }

      const double w2 = weight*weight;
      if(lhs->fSumw2.fN) {
	if(!rhs->fSumw2.fN) {
	  throw std::runtime_error("agmerge::addToHisto(TH1*lhs, TH1*rhs, double): "
				   "incompatible histos - no fSumw2 in rhs"
				   );
	}

	for(int i=0; i<lhs->fN; ++i) {
	  lhs->fSumw2.fArray[i] += w2 * rhs->fSumw2.fArray[i];
	}
      }

      return true;
    }
    return false;
  }

  //----------------------------------------------------------------
  void addTo(TH1* lhs, TH1* rhs, double weight) {
    if(! (addToProfile<TProfile3D>(lhs, rhs, weight) ||
	  addToProfile<TProfile2D>(lhs, rhs, weight) ||
	  addToProfile<TProfile>(lhs, rhs, weight) ||
	  addToHisto<TH1D>(lhs, rhs, weight) ||
	  addToHisto<TH1F>(lhs, rhs, weight) ||
	  addToHisto<TH2D>(lhs, rhs, weight) ||
	  addToHisto<TH2F>(lhs, rhs, weight) ||
	  addToHisto<TH3D>(lhs, rhs, weight) ||
	  addToHisto<TH3F>(lhs, rhs, weight) 
	  ) )
      {
	throw std::runtime_error("agmerge::addTo(TH1,TH1,weight): suport for "
				 "combination of classes "+
				 std::string(lhs->ClassName())+
				 ", "+std::string(rhs->ClassName())+
				 " is not implemented"
				 );
      }
  }

  //================================================================
}
