// This is an excerpt from TWIST tta program code,
// A. Gaponenko, 2002-2004

#include "InputFileGetter.h"

#include <stdexcept>
#include <sstream>
#include <iostream>

//================================================================
InputFileGetter::InputFileGetter(int NARGS, char * const ARGS[])
  : nargs(NARGS), args(ARGS), ifile(0)
{
  if(nargs<1) {
    throw std::runtime_error("InputFileGetter(): nargs < 1");
  }
  is_stdin = (std::string(args[0]) == "--stdin");
  if(is_stdin && (nargs > 1) ) {
    throw std::runtime_error("InputFileGetter(): unexpected input after --stdin");
  }
}

//================================================================
std::string InputFileGetter::get() {
  if(is_stdin) {
    std::string str;
    std::cin>>str;
    return str;
  }
  else {
    if(ifile<nargs) {
      return args[ifile++];
    }
    else {
      return "";
    }
  }
}

//================================================================
//EOF
