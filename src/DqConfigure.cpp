#include "DqConfigure.hpp"

/* Constructor, destructor. */ 
DqConfigure::DqConfigure(std::vector<std::string> args, 
                     std::string typeSymbol, std::string delimiter, std::string argSymbol)
{
  _typeSymbol = typeSymbol;
  _delimiter = delimiter;
  _argSymbol = argSymbol;
	config(&args);
}

DqConfigure::DqConfigure(std::string typeSymbol, std::string delimiter, std::string argSymbol)
{
  _typeSymbol = typeSymbol;
  _delimiter = delimiter;
  _argSymbol = argSymbol;
}

DqConfigure::~DqConfigure()
{
}
//------------------------------------------------------------------------------

/* Functions. */ 
std::string DqConfigure::type() 
{
  auto it = _args.find(_typeSymbol);
  if(it != _args.end()) 
    return _args.find(_typeSymbol)->second;
  else
    return "";
}

std::string DqConfigure::log(std::vector<std::string> logs, std::string rowDelimiter) 
{
	std::string result = "";	
  for(unsigned int i = 0; i < logs.size(); i++)
		result += logs.at(i);
	result += "RUN: " + _run + rowDelimiter;
	result += "TIME: " + _time; 
  return result;
}
bool DqConfigure::logToFile(std::string log, std::string path)
{
	std::ofstream file;
  file.open(path);
  if(!file.is_open())
    return false;
  file << log;
  file.close();
	return true;
	
}

bool DqConfigure::setting(std::vector<std::string> args) 
{
	config(&args);
  return true;
}
// private
bool DqConfigure::config(std::vector<std::string> *args) 
{
  std::string name = "", value = "", tmp = "";
  std::size_t pos = 0;
  for(unsigned int i = 0; i < args->size(); i++) {
    name.clear(); value.clear();
    
    if(args->at(i).find(_argSymbol, 0)  == std::string::npos) {
      if(tmp.empty())
        name = std::to_string(i);
      else
        name = tmp;
      value = args->at(i);
      tmp.clear();
    }
    else {
      pos = args->at(i).find(_delimiter);
      name = args->at(i).substr(_argSymbol.size(), pos - _argSymbol.size());
      value = args->at(i).substr(pos + _delimiter.size(), args->at(i).size() - 1);
      tmp = value;
    }
    
    _args.insert(std::pair <std::string, std::string> (name, value));
  }
	time_t rawtime;
  time (&rawtime);
	struct tm y2k = {0};
  double seconds = difftime(rawtime, mktime(&y2k));
  _run = std::to_string((unsigned int)seconds);
  _time = ctime(&rawtime);
	return true;
}
//------------------------------------------------------------------------------

/* Set functions. */ 
bool DqConfigure::setTypeSymbol(std::string symbol)
{
  if(symbol.size() == 0)
    return 0;
  _typeSymbol = symbol;
  return true;
}
bool DqConfigure::setArgSymbol(std::string symbol)
{
  if(symbol.size() == 0)
    return 0;
  _argSymbol = symbol;
  return true;
}
bool DqConfigure::setDelimiter(std::string symbol)
{
  if(symbol.size() == 0)
    return 0;
  _delimiter = symbol;
  return true;
}
//------------------------------------------------------------------------------

/* Get functions. */ 
std::string DqConfigure::getTypeSymbol() 
{
    return _typeSymbol;
}
std::string DqConfigure::getArgSymbol() 
{
    return _argSymbol;
}
std::string DqConfigure::getDelimiter() 
{
    return _delimiter;
}

std::map<std::string, std::string> DqConfigure::getArgs() 
{
  return _args;
}

std::string DqConfigure::getNumRun() 
{
	return _run;
} 
//------------------------------------------------------------------------------
