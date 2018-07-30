//------------------------------------------------------------------------------
/* INFORMATION: */
//------------------------------------------------------------------------------
#ifndef DQCARD_h
#define DQCARD_h

// C++ includes
#include <string>
#include <vector>

#include <sys/types.h>
#include <dirent.h>

#include "DqCSV.hpp"
#include "DqParser.hpp"

class DqCard
{
public:

  /* Constructor, destructor. */ 
  //----------------------------------------------------------------------------
  DqCard(std::string cardSymbol = "card", 
         std::string cardDelimiter = ",",
         std::string cardLineDelimiter = "\n");
  virtual ~DqCard();
  //----------------------------------------------------------------------------

  /* functions. */ 
  bool setting(std::map<std::string, std::string> args);
  
  /* Get functions. */ 
  std::string getCardPath();
  
  std::string getCardSymbol();
  std::string getCardDelimiter();
  std::string getCardLineDelimiter();

  std::string getInfoSymbolBeg();
  std::string getInfoSymbolEnd();

  std::string getModsSymbolBeg();
  std::string getModsSymbolEnd();

  std::string getInputSymbolBeg();
  std::string getInputSymbolEnd();

  std::string getOutputSymbolBeg();
  std::string getOutputSymbolEnd();

  std::string getSystemSymbolBeg();
  std::string getSystemSymbolEnd();

  std::string getParamsSymbolBeg();
  std::string getParamsSymbolEnd();
  
  std::vector<std::string> getSystems();
  
  std::map<std::string, std::string> getParams();
  std::string getLog();
  std::vector<std::string> getInfo();
  std::string getStringInfo();
  std::vector<std::string> getMods();
  std::vector<std::string> getInput();
  std::vector<std::string> getOutput();
  std::vector<std::vector<std::string>> getSysMods();
  //----------------------------------------------------------------------------
  
  /* Set functions. */ 
  bool setCardSymbol(std::string symbol);
  bool setCardDelimiter(std::string symbol);
  bool setCardLineDelimiter(std::string symbol);

  bool setInfoSymbolBeg(std::string symbol);
  bool setInfoSymbolEnd(std::string symbol);

  bool setModsSymbolBeg(std::string symbol);
  bool setModsSymbolEnd(std::string symbol);

  bool setInputSymbolBeg(std::string symbol);
  bool setInputSymbolEnd(std::string symbol);

  bool setOutputSymbolBeg(std::string symbol);
  bool setOutputSymbolEnd(std::string symbol);

  bool setSystemSymbolBeg(std::string symbol);
  bool setSystemSymbolEnd(std::string symbol);

  bool setParamsSymbolBeg(std::string symbol);
  bool setParamsSymbolEnd(std::string symbol);

  bool setSystems(std::vector<std::string> systems);
  //----------------------------------------------------------------------------

protected:

  std::string _cardPath;
  
  std::string _cardSymbol;
  std::string _cardDelimiter;
  std::string _cardLineDelimiter;
  
  std::string _cardInfoSymbolBeg;
  std::string _cardInfoSymbolEnd;

  std::string _cardModsSymbolBeg;
  std::string _cardModsSymbolEnd;
  
  std::string _cardInputSymbolBeg; 
  std::string _cardInputSymbolEnd; 
  
  std::string _cardOutputSymbolBeg; 
  std::string _cardOutputSymbolEnd; 
  
  std::string _cardSystemSymbolBeg; 
  std::string _cardSystemSymbolEnd; 
  
  std::string _cardParamsSymbolBeg; 
  std::string _cardParamsSymbolEnd; 
  
  std::string _cardSystemParamsSymbolBeg; 
  std::string _cardSystemParamsSymbolEnd; 
  
  std::vector<std::string> _systems; 

  std::vector<std::string> _info;
  std::vector<std::string> _mods;
  std::vector<std::string> _input;
  std::vector<std::string> _output;
  std::vector<std::vector<std::string>> _sysMods;
  
  std::map<std::string, std::string> _args;
  std::map<std::string, std::string> _params;
  //----------------------------------------------------------------------------
  
  bool info();
  bool mods();
  bool params();
  bool input();
  bool output();
  bool system(std::vector<std::string> *result, std::string type);
  //----------------------------------------------------------------------------

};
#endif
//------------------------------------------------------------------------------
