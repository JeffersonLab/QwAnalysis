#ifndef QOPTIONS_H
#define QOPTIONS_H

/*! \brief Qoptions is a generic ascii read-in program for handling
program options.
*/
///
/// \ingroup QwTrackingAnl
class Qoptions{

 public:

  Qoptions();
  void Get(const char *optionsfile);

 private:

  int debug; // debug level

};


#endif
