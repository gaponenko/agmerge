/// Dear emacs, this is -*-c++-*-
///
/// InputFileGeter is a class that makes it convenient to get a set of
/// strings ("file names") from either the command line, or stdin.
/// 
/// 
/// This is an excerpt from TWIST tta program code,
/// A. Gaponenko, 2002-2004

#include <string>

//================================================================
class InputFileGetter {
  int nargs;
  char * const *args;
  int ifile;
  bool is_stdin;
public:

  /// NARGS is the size or the ARGS array, must be >0.
  /// 
  /// If ARGS[0] is "--stdin", NARGS==1 is expected 
  /// and file names are read from stdin.  Otherwise
  /// file names are taken from ARGS.  
  ///
  /// Typical use:
  /// 
  /// int main(int argc, char *const argv[]) {
  ///    ....;
  ///    // use other options/args
  ///    ...;
  ///    InputFileGetter files(argc - offset, argv + offset);
  ///    ...;
  ///    for(std::string infile = files.get(); !infile.empty(); infile = files.get()) {
  ///       // do somethign with infile...
  ///       ....;
  ///    }
  ///    ...;
  /// }

  InputFileGetter(int NARGS, char * const ARGS[]);

  /// Returns the next file name.  When the input is exhausted returns
  /// an empty string.  Empty strings are not expected on the input
  /// (this is for file names), thus we don't provide an interface
  /// to deal with them.
  std::string get();
};

//EOF
