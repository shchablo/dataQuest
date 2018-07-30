//------------------------------------------------------------------------------
/* INFORMATION: */
//------------------------------------------------------------------------------
#ifndef DQCONFIGURE_h
#define DQCONFIGURE_h

// C++ includes
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>

class DqConfigure
{
public:

  /* Constructor, destructor. */ 
  DqConfigure(std::vector<std::string> args, 
              std::string typeSymbol = "type", std::string delimiter = "=", std::string argSymbol = "-");
  DqConfigure(std::string typeSymbol = "type", std::string delimiter = "=", std::string argSymbol = "-");
  virtual ~DqConfigure();
  //----------------------------------------------------------------------------

  /* Functions. */ 
  std::string type();
  std::string log(std::vector<std::string> logs, std::string rowDelimiter);
  bool logToFile(std::string log, std::string path);
  bool setting(std::vector<std::string> args);
  //----------------------------------------------------------------------------
  
  /* Set functions. */ 
  bool setTypeSymbol(std::string symbol);
  bool setArgSymbol(std::string symbol);
  bool setDelimiter(std::string symbol);
  //----------------------------------------------------------------------------
  
    /* Get functions. */ 
  std::string getTypeSymbol();
  std::string getArgSymbol();
  std::string getDelimiter();
  std::map<std::string, std::string> getArgs();
  std::string getNumRun();
  //----------------------------------------------------------------------------

protected:

  std::string _time;
  std::string _run;
  std::string _typeSymbol;
  std::string _delimiter;
  std::string _argSymbol;
  std::map<std::string, std::string> _args;

private:
	
  /* Functions. */ 
	bool config(std::vector<std::string> *args);
  //----------------------------------------------------------------------------
};
#endif
//------------------------------------------------------------------------------
