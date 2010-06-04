// Generate histograms to test the merging code.
// Andrei Gaponenko, 2010

#include <iostream>
#include <cmath>
#include <cstdlib>
#include <memory>

#include "TFile.h"
#include "TProfile.h"
#include "TRandom.h"

//================================================================
int nEntries(int ibin, int nmin, int nmax, int nbins) {
  return nmin * exp(double(ibin)/double(nbins-1) * log(nmax/nmin));
}

//================================================================
int main() {
  TProfile::Approximate(false);

  TFile out1("test1.root", "NEW");
  TFile out2("test2.root", "NEW");
  if(!out1.IsOpen() || !out2.IsOpen()) {
    std::cerr<<"Error opening outputs"<<std::endl;
    exit(1);
  }

  //----------------------------------------------------------------
  const double mean1 = 0.8;
  const double mean2 = 0.2;

  double sigma1, sigma2;
  sigma1 = sigma2 = 0.1;

  const int nbins=5;
  std::auto_ptr<TProfile> prof1(new TProfile("test", "test", nbins, -0.5, nbins-0.5));
  prof1->SetDirectory(0);
  prof1->Sumw2();

  std::auto_ptr<TProfile> prof2(new TProfile("test", "test", nbins, -0.5, nbins-0.5));
  prof2->SetDirectory(0);
  prof2->Sumw2();

  for(int ibin=0; ibin<nbins; ++ibin) {
    int n1 = nEntries(ibin, 5, 50000, nbins);
    for(int k=0; k<n1; ++k) {
      prof1->Fill(ibin, gRandom->Gaus(mean1, sigma1), 1.);
    }

    int n2 = nEntries(nbins-1-ibin, 5, 50000, nbins);
    for(int k=0; k<n2; ++k) {
      prof2->Fill(ibin, gRandom->Gaus(mean2, sigma2), 1.);
    }
  }

  out1.cd();
  prof1->Write();
  out1.Close();

  out2.cd();
  prof2->Write();
  out2.Close();
}
