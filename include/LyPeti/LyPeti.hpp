//------------------------------------------------------------------------------
/* INFORMATION: */
//------------------------------------------------------------------------------
#ifndef LYPETI_h
#define LYPETI_h

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <map>
#include <vector>
#include <limits>
#include <algorithm>

#include "DqAnalysis.hpp"
#include "DqCSV.hpp"
#include "DqRoot.hpp"

#include "LyPetiEvent.hpp"
#include "LyPetiRead.hpp"
#include "LyPetiAnalysis.hpp"

#include "DqProcessBar.hpp"
#include "DqParser.hpp"

class LyPeti : public DqAnalysis
{
public:

  /* Constructor, destructor. */ 
  //----------------------------------------------------------------------------
  LyPeti(std::string sysMods, 
               std::map<std::string, std::string> params,
               std::vector<std::string> inputs,
               std::vector<std::string> outputs);
  virtual ~LyPeti();
  //----------------------------------------------------------------------------
  
  /* Get functions. */ 
  virtual std::string getLog();
  //----------------------------------------------------------------------------
  
  /* analysis params and input files */
  std::vector<int> parsNumRuns(std::vector<std::string> files); 
  int numRun(std::string file); 
  //----------------------------------------------------------------------------
  
  /* Loop function */ 
  //----------------------------------------------------------------------------
  bool analysis();
  bool offSet(int chamber, int offSetRun, 
              std::map<int, double>* offsets, std::map<int, double>* offsetsHR, std::map<int, double>* offsetsLR,
              std::map<std::string, DqTGE*>* TGEs, DqRoot* dataRoot);
  bool binToROOT();
  //----------------------------------------------------------------------------
  
  virtual bool run();
protected:
	
  std::string _mod;
  std::map<std::string, std::string> _params;
  std::vector<std::string> _inputs;
  std::vector<std::string> _outputs;
};

#endif
//------------------------------------------------------------------------------
