#include "DqAnalysis.hpp"

#include <vector>
/* Constructor, destructor. */ 
//------------------------------------------------------------------------------
DqAnalysis::DqAnalysis()
{
  _log = "";
  _rowDelimiter = "\r\n";
  _logSymbol = "#OTHER INFORMATION:";
}

DqAnalysis::~DqAnalysis()
{
}
//------------------------------------------------------------------------------

/* Set functions. */ 
bool DqAnalysis::setRowDelimiter(std::string delimiter) 
{
  if(delimiter.size() > 0) {
    _rowDelimiter = delimiter;
    return true;
  }
  else
    return false;
  return false;

}
//------------------------------------------------------------------------------

/* Get functions. */ 
std::string DqAnalysis::getRowDelimiter() 
{
  return _rowDelimiter;
}
std::string DqAnalysis::getLogSymbol() 
{
  return _logSymbol;
}
bool DqAnalysis::dropLog(std::map<std::string, std::string>* params) 
{
  std::string param;
  param = this->findParam("drop_log", params);
  if(param == "0")
    return false;
  else
    return true;
}
//------------------------------------------------------------------------------
/* Functions. */ 
std::string DqAnalysis::findParam(std::string name, 
                                  std::map<std::string, std::string>* params)
{
  std::string param = "";
  auto it = params->find(name);
  if(it == params->end())
    _log += " WARNING: Can't find param: " + name + _rowDelimiter;
  else
    param = params->find(name)->second;
  
  return param;
}
bool DqAnalysis::findParam(std::string name, std::string *param, 
                           std::map<std::string, std::string>* params) 
{
  auto it = params->find(name);
  if(it == params->end()) { 
    _log += " WARNING: Can't find param: " + name + _rowDelimiter;
    return false;
  }
  else {
    *param = params->find(name)->second;
    return true;
  }
  return false;
}
