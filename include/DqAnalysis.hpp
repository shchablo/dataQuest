//------------------------------------------------------------------------------
/* INFORMATION: */
//------------------------------------------------------------------------------
#ifndef DQANALYSIS_h
#define DQANALYSIS_h


#include <iostream>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <vector>
#include <map>

class DqAnalysis
{
public:

  /* Constructor, destructor. */ 
  DqAnalysis();
  virtual ~DqAnalysis();
  //----------------------------------------------------------------------------

  /* Set functions. */ 
  bool setRowDelimiter(std::string delimiter); 
  //----------------------------------------------------------------------------
  
  /* Get functions. */ 
  std::string getRowDelimiter();
  std::string getLogSymbol();
  virtual std::string getLog() {std::string log = ""; return log;};
  bool dropLog(std::map<std::string, std::string>* params); 
  //----------------------------------------------------------------------------
  
  /* Function */ 
  std::string findParam(std::string name, 
                        std::map<std::string, std::string>* params); 
  bool findParam(std::string name, std::string* param,
                std::map<std::string, std::string>* params); 
  //----------------------------------------------------------------------------
  
  /* Loop function */ 
  virtual bool run() {return false;};
  //----------------------------------------------------------------------------
protected:

  std::string _log;
  std::string _logSymbol;
  std::string _rowDelimiter;
};
#endif
//------------------------------------------------------------------------------
