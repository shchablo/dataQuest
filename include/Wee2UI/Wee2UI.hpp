//------------------------------------------------------------------------------
/* INFORMATION: */
//------------------------------------------------------------------------------
#ifndef WEE2UI_h
#define WEE2UI_h


#include <iostream>
#include <stdio.h>
#include <string.h>
#include <map>
#include <vector>

#include "DqAnalysis.hpp"
#include "DqCSV.hpp"
#include "DqRoot.hpp"

class Wee2UI : public DqAnalysis
{
public:

  /* Constructor, destructor. */ 
  //----------------------------------------------------------------------------
  Wee2UI(std::string sysMods, 
               std::map<std::string, std::string> params,
               std::vector<std::string> inputs,
               std::vector<std::string> outputs);
  virtual ~Wee2UI();
  //----------------------------------------------------------------------------

  /* Loop function */ 
  //----------------------------------------------------------------------------
  bool plot_SCscan();
  virtual bool run();
  //----------------------------------------------------------------------------
protected:

  std::string _mod;
  std::map<std::string, std::string> _params;
  std::vector<std::string> _inputs;
  std::vector<std::string> _outputs;
};
#endif
//------------------------------------------------------------------------------
