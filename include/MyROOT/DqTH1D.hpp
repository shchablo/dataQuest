//------------------------------------------------------------------------------
/* INFORMATION: */
//------------------------------------------------------------------------------
#ifndef DqTH1D_h
#define DqTH1D_h

#include <TH1D.h>
#include "DqParser.hpp"

class DqTH1D : public TH1D
{
public:

  /* Constructor, destructor. */ 
  DqTH1D(const char *name, const char *title, Int_t nbinsx, Double_t xlow, Double_t xup):TH1D(name, title, nbinsx, xlow, xup) {
    _isConfig = false;
    _mod = "default";
    _modStr = "_mod";
    _begStr = "_beg";
    _endStr = "_end";
    _sizeStr = "_size";
    _titleXstr = "_titleX";
    _titleYstr = "_titleY";
    _chamber = 0;
    _size = 0; _isSize = true;
    _beg = 1; _isBeg = true;
    _end = -1; _isEnd = true;
  }
  virtual ~DqTH1D();
  bool config(std::string name, std::map<std::string, std::string>* params);
  bool setting(double value);
  bool setting();
  //----------------------------------------------------------------------------
protected:

  bool _isConfig; 
  
  std::string _mod; 
  
  std::string _modStr; 
  std::string _begStr;
  std::string _endStr;
  std::string _sizeStr;
  std::string _titleXstr;
  std::string _titleYstr;
 
  double _chamber;
  int _size;  bool _isSize;
  double _beg; bool _isBeg;
  double _end;  bool _isEnd;
};
#endif
//------------------------------------------------------------------------------
