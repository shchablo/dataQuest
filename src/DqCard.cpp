#include "DqCard.hpp"

/* Constructor, destructor. */ 
//------------------------------------------------------------------------------
DqCard::DqCard(std::string cardSymbol, 
               std::string cardDelimiter,
               std::string cardLineDelimiter)
{
  _cardSymbol = cardSymbol;
  _cardDelimiter = cardDelimiter;
  _cardLineDelimiter = cardLineDelimiter;
  
  _cardInfoSymbolBeg = "#INFORMATION:";
  _cardInfoSymbolEnd = "#INFORMATION END.";
  
  _cardModsSymbolBeg = "#MODS:";
  _cardModsSymbolEnd = "#MODS END.";
  
  _cardInputSymbolBeg = "#INPUT:";
  _cardInputSymbolEnd = "#INPUT END.";
  
  _cardOutputSymbolBeg = "#OUTPUT:";
  _cardOutputSymbolEnd = "#OUTPUT END.";
  
  _cardSystemSymbolBeg = "#SYSTEM:";
  _cardSystemSymbolEnd = "#SYSTEM END.";
  
  _cardParamsSymbolBeg = "#PARAMETERS:";
  _cardParamsSymbolEnd = "#PARAMETERS END.";
  
  _cardPath = "";
  
}

DqCard::~DqCard()
{
}

/* Functions. */ 
bool DqCard::setting(std::map<std::string, std::string> args)
{
  std::string cardSymbol = _cardSymbol;
  std::vector<std::string> systems = _systems;

  auto it = args.find(cardSymbol);
  if(it == args.end()) 
    return false;
  _cardPath = args.find(cardSymbol)->second;
  _args = args;

  // get info
  info();

  // get mods
  bool isMods = mods();
  if(!isMods)
    return false;

  // get params
  params(); 
 
  // get input
  input(); 
    
  // get output
  output();
    
  // get system
  std::vector<std::string> tmpMods;
  bool isSystemMods = false;
  for(unsigned int i = 0; i < systems.size(); i++) {
    tmpMods.clear();
    isSystemMods = false;
    isSystemMods = system(&tmpMods, systems.at(i));
    if(isSystemMods)
      _sysMods.push_back(tmpMods);
    else {
      tmpMods.clear();
      _sysMods.push_back(tmpMods);
    }
  }
  return true;
}
//------------------------------------------------------------------------------

/* Get functions. */ 
std::string DqCard::getCardPath() 
{
  return _cardPath;
}
std::string DqCard::getCardSymbol()
{
  return _cardSymbol;
}
std::string DqCard::getCardDelimiter()
{
  return _cardDelimiter;
}
std::string DqCard::getCardLineDelimiter()
{
  return _cardLineDelimiter;
}

std::string DqCard::getInfoSymbolBeg()
{
  return _cardInfoSymbolBeg;
}
std::string DqCard::getInfoSymbolEnd()
{
  return _cardInfoSymbolEnd;
}

std::string DqCard::getModsSymbolBeg()
{
  return _cardModsSymbolBeg;
}
std::string DqCard::getModsSymbolEnd()
{
  return _cardModsSymbolEnd;
}

std::string DqCard::getInputSymbolBeg()
{
  return _cardInputSymbolBeg;
}
std::string DqCard::getInputSymbolEnd()
{
  return _cardInputSymbolEnd;
}

std::string DqCard::getOutputSymbolBeg()
{
  return _cardOutputSymbolBeg;
}
std::string DqCard::getOutputSymbolEnd()
{
  return _cardOutputSymbolEnd;
}

std::string DqCard::getSystemSymbolBeg()
{
  return _cardSystemSymbolBeg;
}
std::string DqCard::getSystemSymbolEnd()
{
  return _cardSystemSymbolEnd;
}

std::string DqCard::getParamsSymbolBeg()
{
  return _cardParamsSymbolBeg;
}
std::string DqCard::getParamsSymbolEnd()
{
  return _cardParamsSymbolEnd;
}

std::vector<std::string> DqCard::getSystems()
{
  return _systems;
}

std::map<std::string, std::string> DqCard::getParams()
{
  return _params;
}

std::string DqCard::getLog()
{
  std::string cardSymbol = _cardSymbol;
  std::string cardLineDelimiter = _cardLineDelimiter;
  std::string cardDelimiter = _cardDelimiter;
  std::string cardInfoSymbolBeg = _cardInfoSymbolBeg; 
  std::string cardInfoSymbolEnd = _cardInfoSymbolEnd; 
  std::string cardModsSymbolBeg = _cardModsSymbolBeg; 
  std::string cardModsSymbolEnd = _cardModsSymbolEnd; 
  std::string cardInputSymbolBeg = _cardInputSymbolBeg; 
  std::string cardInputSymbolEnd = _cardInputSymbolEnd;
  std::string cardOutputSymbolBeg = _cardOutputSymbolBeg; 
  std::string cardOutputSymbolEnd = _cardOutputSymbolEnd;
  std::string cardSystemSymbolBeg = _cardSystemSymbolBeg; 
  std::string cardSystemSymbolEnd = _cardSystemSymbolEnd;
  std::string cardParamsSymbolBeg = _cardParamsSymbolBeg; 
  std::string cardParamsSymbolEnd = _cardParamsSymbolEnd; 
  
  std::string result = "";
  // info
  result = cardInfoSymbolBeg + cardLineDelimiter;
  for(unsigned int  i = 0; i < _info.size(); i++)
    result += (_info.at(i) + cardLineDelimiter);
  result += cardInfoSymbolEnd  + cardLineDelimiter;
  
  result += cardLineDelimiter;

  // mods
  result += cardModsSymbolBeg + cardLineDelimiter;
  for(unsigned int  i = 0; i < _mods.size(); i++)
    result += _mods.at(i) + cardDelimiter;
  result += cardLineDelimiter + cardModsSymbolEnd  + cardLineDelimiter;
  
  result += cardLineDelimiter;
  
  // params
  result += cardParamsSymbolBeg + cardLineDelimiter;
  for(auto& tmp: _params)
    result += (tmp.first + cardDelimiter);
  result += cardLineDelimiter;
  for(auto& tmp: _params)
    result += (tmp.second + cardDelimiter);
  result += cardLineDelimiter + cardParamsSymbolEnd + cardLineDelimiter;
  
  result += cardLineDelimiter;
 
  // input
  result += cardInputSymbolBeg + cardLineDelimiter;
  for(unsigned int  i = 0; i < _input.size(); i++)
   result += _input.at(i) + cardLineDelimiter;
  result += cardLineDelimiter + cardInputSymbolEnd  + cardLineDelimiter;
  
  result += cardLineDelimiter;
  
  // output
  result += cardOutputSymbolBeg + cardLineDelimiter;
  for(unsigned int  i = 0; i < _output.size(); i++)
    result += _output.at(i) + cardLineDelimiter;
  result += cardLineDelimiter + cardOutputSymbolEnd  + cardLineDelimiter;

  result += cardLineDelimiter;
  
  // system
  result += cardSystemSymbolBeg + cardLineDelimiter;
  for(unsigned int i = 0; i < _systems.size(); i++) {
    result += _systems.at(i)  + cardDelimiter;
    for(unsigned int j = 0; j < _sysMods.at(i).size(); j++)
      result += _sysMods.at(i).at(j)  + cardDelimiter;
    result += cardLineDelimiter;
  }
   result += cardSystemSymbolEnd  + cardLineDelimiter;
   return result;
}
std::vector<std::string> DqCard::getInfo()
{
  return _info;
}
std::string DqCard::getStringInfo()
{
  std::string result = "";
  for(unsigned int i = 0; i < _info.size(); i++) {
    if(i == (_info.size() - 1))
      result += _info.at(i);
    else
      result += _info.at(i) + _cardLineDelimiter;
  }
  return result;
}

std::vector<std::string> DqCard::getMods()
{
  return _mods;
}

std::vector<std::string> DqCard::getInput()
{
  return _input;
}
std::vector<std::string> DqCard::getOutput()
{
  return _output;
}
std::vector<std::vector<std::string>> DqCard::getSysMods()
{
  return _sysMods;
}
//------------------------------------------------------------------------------

/* Set functions. */ 
bool DqCard::setCardSymbol(std::string symbol)
{
  if(symbol.size() == 0)
    return false;
  else {
    _cardSymbol = symbol;
    return true;
  }
}
bool DqCard::setCardDelimiter(std::string symbol)
{
  if(symbol.size() == 0)
    return false;
  else {
    _cardDelimiter = symbol;
    return true;
  }
}
bool DqCard::setCardLineDelimiter(std::string symbol)
{
  if(symbol.size() == 0)
    return false;
  else {
    _cardLineDelimiter = symbol;
    return true;
  }
}

bool DqCard::setInfoSymbolBeg(std::string symbol)
{
  if(symbol.size() == 0)
    return false;
  else {
    _cardInfoSymbolBeg = symbol;
    return true;
  }
}
bool DqCard::setInfoSymbolEnd(std::string symbol)
{
  if(symbol.size() == 0)
    return false;
  else {
    _cardInfoSymbolEnd = symbol;
    return true;
  }
}

bool DqCard::setModsSymbolBeg(std::string symbol)
{
  if(symbol.size() == 0)
    return false;
  else {
    _cardModsSymbolBeg = symbol;
    return true;
  }
}
bool DqCard::setModsSymbolEnd(std::string symbol)
{
  if(symbol.size() == 0)
    return false;
  else {
    _cardModsSymbolEnd = symbol;
    return true;
  }
}

bool DqCard::setInputSymbolBeg(std::string symbol)
{
  if(symbol.size() == 0)
    return false;
  else {
    _cardInputSymbolBeg = symbol;
    return true;
  }
}
bool DqCard::setInputSymbolEnd(std::string symbol)
{
  if(symbol.size() == 0)
    return false;
  else {
    _cardInputSymbolEnd = symbol;
    return true;
  }
}

bool DqCard::setOutputSymbolBeg(std::string symbol)
{
  if(symbol.size() == 0)
    return false;
  else {
    _cardOutputSymbolBeg = symbol;
    return true;
  }
}
bool DqCard::setOutputSymbolEnd(std::string symbol)
{
  if(symbol.size() == 0)
    return false;
  else {
    _cardOutputSymbolEnd = symbol;
    return true;
  }
}

bool DqCard::setSystemSymbolBeg(std::string symbol)
{
  if(symbol.size() == 0)
    return false;
  else {
    _cardSystemSymbolBeg = symbol;
    return true;
  }
}
bool DqCard::setSystemSymbolEnd(std::string symbol)
{
  if(symbol.size() == 0)
    return false;
  else {
    _cardSystemSymbolEnd = symbol;
    return true;
  }
}

bool DqCard::setParamsSymbolBeg(std::string symbol)
{
  if(symbol.size() == 0)
    return false;
  else {
    _cardParamsSymbolBeg = symbol;
    return true;
  }
}
bool DqCard::setParamsSymbolEnd(std::string symbol)
{
  if(symbol.size() == 0)
    return false;
  else {
    _cardParamsSymbolEnd = symbol;
    return true;
  }
}

bool DqCard::setSystems(std::vector<std::string> systems)
{
  _systems = systems;
  return true;
}
//------------------------------------------------------------------------------

/* Protected functions. */ 
bool DqCard::info()
{
  
  std::string cardSymbol = _cardSymbol;
  std::string cardDelimiter = _cardDelimiter;
  std::string cardLineDelimiter = _cardLineDelimiter;
  std::string cardInfoSymbolBeg = _cardInfoSymbolBeg;
  std::string cardInfoSymbolEnd = _cardInfoSymbolEnd;

  auto it = _args.find(cardSymbol);
  if(it == _args.end()) 
    return false;
  std::string cardPath = _args.find(cardSymbol)->second;
  
  // create stream and open file
  DqCSV inCSV(cardPath, cardDelimiter, cardLineDelimiter);
  bool isOpenInCSV = inCSV.open(std::fstream::in);
  if(!isOpenInCSV) 
    return false;
    
  unsigned int numRows = 0;
  bool isNumRows = false;
  isNumRows = inCSV.numRows(&numRows);
  if(!isNumRows) 
    return false;
  
  std::vector<std::string> line;
  bool isReadRow = false;
  bool isRead = false;
  for(unsigned int i = 0; i < numRows; i++) {
    isReadRow = inCSV.readRow(i, &line);
    if(!isReadRow) 
      return false;

    for(unsigned int j = 0; j < line.size(); j++) {
      if(line.at(j) == cardInfoSymbolEnd)
        isRead = false;

      if(isRead && !line.at(j).empty())
        _info.push_back(line.at(j));
      
      if(line.at(j) == cardInfoSymbolBeg)
        isRead = true;
    }
    line.clear();
  }
  if(isOpenInCSV) {
    bool isCloseInCSV = false; 
    isCloseInCSV = inCSV.close();  
    if(!isCloseInCSV) 
      return false;
  return true;
  }
  return false;
}
bool DqCard::mods()
{
  
  std::string cardSymbol = _cardSymbol;
  std::string cardDelimiter = _cardDelimiter;
  std::string cardLineDelimiter = _cardLineDelimiter;
  std::string cardModsSymbolBeg = _cardModsSymbolBeg;
  std::string cardModsSymbolEnd = _cardModsSymbolEnd;
  
  auto it = _args.find(cardSymbol);
  if(it == _args.end()) 
    return false;

  std::string cardPath = _args.find(cardSymbol)->second;

  // create stream and open file
  DqCSV inCSV(cardPath, cardDelimiter, cardLineDelimiter);
  bool isOpenInCSV = inCSV.open(std::fstream::in);
  
  if(!isOpenInCSV)
    return false;

  unsigned int numRows = 0;
  bool isNumRows = false;
  isNumRows = inCSV.numRows(&numRows);
  if(!isNumRows) 
    return false;
  
  std::vector<std::string> line;
  bool isReadRow = false;
  bool isRead = false;
  for(unsigned int i = 0; i < numRows; i++) {
    isReadRow = inCSV.readRow(i, &line);
    if(!isReadRow) 
      return false;

    for(unsigned int j = 0; j < line.size(); j++) {
      if(line.at(j) == cardModsSymbolEnd)
        isRead = false;

      if(isRead && !line.at(j).empty()) {
        _mods.push_back(line.at(j));
      }
      
      if(line.at(j) == cardModsSymbolBeg)
        isRead = true;
    }
    line.clear();
  }

  if(isOpenInCSV) {
    bool isCloseInCSV = false; 
    isCloseInCSV = inCSV.close();  
    if(!isCloseInCSV) 
      return false;
  return true;
  }
  return false;
}
bool DqCard::params()
{
  std::vector<std::string> names;
  std::vector<std::string> values;
  std::string cardSymbol = _cardSymbol;
  std::string cardDelimiter = _cardDelimiter;
  std::string cardLineDelimiter = _cardLineDelimiter;
  std::string cardParamsSymbolBeg = _cardParamsSymbolBeg;
  std::string cardParamsSymbolEnd = _cardParamsSymbolEnd;
  auto it = _args.find(cardSymbol);
  if(it == _args.end()) 
    return false;

  std::string cardPath = _args.find(cardSymbol)->second;

  // create stream and open file
  DqCSV inCSV(cardPath, cardDelimiter, cardLineDelimiter);
  bool isOpenInCSV = inCSV.open(std::fstream::in);
  
  if(!isOpenInCSV)
    return false;

  unsigned int numRows = 0;
  bool isNumRows = false;
  isNumRows = inCSV.numRows(&numRows);
  if(!isNumRows) 
    return false;

  std::vector<std::string> line;
  bool isReadRow = false;
  bool isRead = false;
  bool isNames = false;
  for(unsigned int i = 0; i < numRows; i++) {
    isReadRow = inCSV.readRow(i, &line);
    if(!isReadRow) 
      return false;
    
    if(isRead) {
      if(isNames == true)
        isNames = false;
      else
        isNames = true;
    }
    for(unsigned int j = 0; j < line.size(); j++) {
      if(line.at(j) == cardParamsSymbolEnd)
        isRead = false;

      if(isRead && !line.at(j).empty()) {
        if(isNames)
          names.push_back(line.at(j));
        else
          values.push_back(line.at(j));
      }
      
      if(line.at(j) == cardParamsSymbolBeg)
        isRead = true;
    }
    if(names.size() == values.size()) {
      for(unsigned int j = 0; j < names.size(); j++) {
        auto it = _args.find(names.at(j));
        if(it == _args.end()) {
          _params.insert(std::pair <std::string, std::string> (names.at(j), values.at(j)));
//          std::cout << names.at(j) << ": " << values.at(j) << std::endl;  
        }
        else {
          _params.insert(std::pair <std::string, std::string> 
                        (names.at(j), _args.find(names.at(j))->second));
//          std::cout << names.at(j) << ": " << _args.find(names.at(j))->second << std::endl;  
        }

      }
      names.clear();
      values.clear();
    }
    line.clear();
  }

  if(isOpenInCSV) {
    bool isCloseInCSV = false; 
    isCloseInCSV = inCSV.close();  
    if(!isCloseInCSV) 
      return false;
  return true;
  }
  return false;
}
bool DqCard::input()
{
  std::string cardSymbol = _cardSymbol;
  std::string cardDelimiter = _cardDelimiter;
  std::string cardLineDelimiter = _cardLineDelimiter;
  std::string cardInputSymbolBeg = _cardInputSymbolBeg;
  std::string cardInputSymbolEnd = _cardInputSymbolEnd;

  auto it = _args.find(cardSymbol);
  if(it == _args.end()) 
    return false;

  std::string cardPath = _args.find(cardSymbol)->second;

  // create stream and open file
  DqCSV inCSV(cardPath, cardDelimiter, cardLineDelimiter);
  bool isOpenInCSV = inCSV.open(std::fstream::in);
  
  if(!isOpenInCSV)
    return false;

  unsigned int numRows = 0;
  bool isNumRows = false;
  isNumRows = inCSV.numRows(&numRows);
  if(!isNumRows) 
    return false;
  
  std::vector<std::string> line;
  bool isReadRow = false;
  bool isRead = false;
  for(unsigned int i = 0; i < numRows; i++) {
    isReadRow = inCSV.readRow(i, &line);
    if(!isReadRow) 
      return false;

    for(unsigned int j = 0; j < line.size(); j++) {
      if(line.at(j) == cardInputSymbolEnd)
        isRead = false;

      if(isRead && !line.at(j).empty())
        _input.push_back(line.at(j));
      
      if(line.at(j) == cardInputSymbolBeg)
        isRead = true;
    }
    line.clear();
  }
  // try to find all files in directions.
  DqParser parser;
  for(unsigned int i = 0; i < _input.size(); i++) {
    std::vector<std::string> pathes = parser.checkPath(_input.at(i));  
    if(pathes.size() > 0) {
      _input.erase(_input.begin() + i);
      _input.insert(_input.end(), pathes.begin(), pathes.end());
    }
  }
  
  if(isOpenInCSV) {
    bool isCloseInCSV = false; 
    isCloseInCSV = inCSV.close();  
    if(!isCloseInCSV) 
      return false;
  return true;
  }
  return false;
}
bool DqCard::output()
{
  std::string cardSymbol = _cardSymbol;
  std::string cardDelimiter = _cardDelimiter;
  std::string cardLineDelimiter = _cardLineDelimiter;
  std::string cardOutputSymbolBeg = _cardOutputSymbolBeg;
  std::string cardOutputSymbolEnd = _cardOutputSymbolEnd;

  auto it = _args.find(cardSymbol);
  if(it == _args.end()) 
    return false;

  std::string cardPath = _args.find(cardSymbol)->second;

  // create stream and open file
  DqCSV inCSV(cardPath, cardDelimiter, cardLineDelimiter);
  bool isOpenInCSV = inCSV.open(std::fstream::in);
  
  if(!isOpenInCSV)
    return false;

  unsigned int numRows = 0;
  bool isNumRows = false;
  isNumRows = inCSV.numRows(&numRows);
  if(!isNumRows) 
    return false;
  
  std::vector<std::string> line;
  bool isReadRow = false;
  bool isRead = false;
  for(unsigned int i = 0; i < numRows; i++) {
    isReadRow = inCSV.readRow(i, &line);
    if(!isReadRow) 
      return false;

    for(unsigned int j = 0; j < line.size(); j++) {
      if(line.at(j) == cardOutputSymbolEnd)
        isRead = false;

      if(isRead && !line.at(j).empty())
        _output.push_back(line.at(j));
      
      if(line.at(j) == cardOutputSymbolBeg)
        isRead = true;
    }
    line.clear();
  }

  if(isOpenInCSV) {
    bool isCloseInCSV = false; 
    isCloseInCSV = inCSV.close();  
    if(!isCloseInCSV) 
      return false;
  return true;
  }
  return false;
}
bool DqCard::system(std::vector<std::string> *result, std::string type)
{
  std::string cardSymbol = _cardSymbol;
  std::string cardDelimiter = _cardDelimiter;
  std::string cardLineDelimiter = _cardLineDelimiter;
  std::string cardSystemSymbolBeg = _cardSystemSymbolBeg;
  std::string cardSystemSymbolEnd = _cardSystemSymbolEnd;

  auto it = _args.find(cardSymbol);
  if(it == _args.end()) 
    return false;
  std::string cardPath = _args.find(cardSymbol)->second;

  // create stream and open file
  DqCSV inCSV(cardPath, cardDelimiter, cardLineDelimiter);
  bool isOpenInCSV = inCSV.open(std::fstream::in);
  
  if(!isOpenInCSV)
    return false;

  unsigned int numRows = 0;
  bool isNumRows = false;
  isNumRows = inCSV.numRows(&numRows);
  if(!isNumRows) 
    return false;
  
  std::vector<std::string> line;
  bool isReadRow = false;
  bool isRead = false;
  bool isType = false;
  for(unsigned int i = 0; i < numRows; i++) {
    isReadRow = inCSV.readRow(i, &line);
    if(!isReadRow) 
      return false;

    for(unsigned int j = 0; j < line.size(); j++) {
      if(line.at(j) == cardSystemSymbolEnd)
        isRead = false;

      if(isRead && !line.at(j).empty()) {
        if(line.at(j) == type) {
          isType = true;
          continue;
        }
          if(isType)
            result->push_back(line.at(j));
      }

      if(line.at(j) == cardSystemSymbolBeg)
        isRead = true;
    }
    isType = false;
    line.clear();
  }

  if(isOpenInCSV) {
    bool isCloseInCSV = false; 
    isCloseInCSV = inCSV.close();  
    if(!isCloseInCSV) 
      return false;
  return true;
  }
  return false;
}
//------------------------------------------------------------------------------
