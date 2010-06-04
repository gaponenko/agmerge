/// Dear emacs, this is -*-c++-*-
///
/// A. Gaponenko, 2010

#ifndef AGMERGEHELPERS_H
#define AGMERGEHELPERS_H

class TH1;

namespace agmerge {

  // Change a histogram filled with {x_i, w_i} in such a way that the
  // result is equivalent to filling the histo with {x_i, weight*w_i}
  void applyWeight(TH1* histo, double weight);

  // Transform rhs as in applyWeight() above, and add
  // the result to lhs.
  //
  // (Somewhat like TH1::Add(TH1*, TH1*, double, double)
  // but with correct handling of profile histos
  // and without all the crust.)
  void addTo(TH1* lhs, TH1* rhs, double rhs_weight);
}

#endif/*AGMERGEHELPERS_H*/
