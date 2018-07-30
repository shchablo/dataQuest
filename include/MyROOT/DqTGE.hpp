//------------------------------------------------------------------------------
/* INFORMATION: */
//------------------------------------------------------------------------------
#ifndef DqTGE_h
#define DqTGE_h

#include <iostream>
#include <stdio.h>
#include <vector>
#include <fstream>

#include <iomanip> 

#include <TAxis.h>
#include <TGraphErrors.h>
#include "DqParser.hpp"

class DqTGE : public TGraphErrors
{
public:

  std::string getTitleX();
  std::string getTitleY();
  /* Constructor, destructor. */ 
  DqTGE(const char *name, const char *title) {
    
    this->SetName(name);
    this->SetTitle(title);
    
    _isConfig = false; 
    _titleXstr = "_titleX";
    _titleYstr = "_titleY";
  }
  virtual ~DqTGE();
  bool config(std::string name, std::map<std::string, std::string>* params);
  bool setting(double x, double ex, double y, double ey);
	bool sortByX();
	bool plot();
  bool print(std::string mod);
  //----------------------------------------------------------------------------
protected:
    bool _isConfig;
    std::string _titleXstr;
    std::string _titleYstr;
    std::vector<double> _x;
    std::vector<double> _ex;
    std::vector<double> _y;
    std::vector<double> _ey;
};
#endif
//------------------------------------------------------------------------------
