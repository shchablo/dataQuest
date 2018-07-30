#include "DqTH1D.hpp"

DqTH1D::~DqTH1D()
{
}

bool DqTH1D::config(std::string name, std::map<std::string, std::string>* params)
{
  if(!_isConfig) {
    DqParser parser;
    double param = 0;
    std::string strParam = "";
    parser.strToDouble(&_chamber, parser.chamberName(name));
    bool isMod = parser.findObjectParam((name + _modStr), &strParam, params);
    if(isMod)
      _mod = strParam;
    bool isSize = parser.findObjectParam((name + _sizeStr), &strParam, params);
    if(isSize) {
      parser.strToDouble(&param, strParam);
      _size = (int)param;
      _isSize = false;
    }
    bool isBeg = parser.findObjectParam((name + _begStr), &strParam, params);
    if(isBeg) {
      parser.strToDouble(&param, strParam);
      _beg = param;
      _isBeg = false;
    }
    bool isEnd = parser.findObjectParam((name + _endStr), &strParam, params);
    if(isEnd) {
      parser.strToDouble(&param, strParam);
      _end = param;
      _isEnd = false;
    }
    std::string titleX  = "X";
    bool isTitleX = parser.findObjectParam((name + _titleXstr), &strParam, params);
    if(isTitleX) titleX = strParam;
    this->GetXaxis()->SetTitle(titleX.c_str());
    
    std::string titleY  = "Y";
    bool isTitleY = parser.findObjectParam((name + _titleYstr), &strParam, params);
    if(isTitleY) titleY = strParam;
    this->GetYaxis()->SetTitle(titleY.c_str());
    _isConfig = true;
    return true;
  }
  return false;
}

bool DqTH1D::setting(double value)
{
 
  // Size of bins for strips --------------------------------------------------- 
  if(_mod == "default") {
    if(value <= _beg && _isBeg)
      _beg = value;
    if(value > _end && _isEnd)
      _end = value;
  }
  //----------------------------------------------------------------------------
  // Size of bins for strips --------------------------------------------------- 
  if(_mod == "strips") {
    if(value <= _beg && _isBeg)
      _beg = value;
    if(value > _end && _isEnd)
      _end = value;
  }
  //----------------------------------------------------------------------------
  
  // Rice Rule FILLING ----------------------------------------------------- 
  else if(_mod == "rice") {
    if(value <= _beg && _isBeg)
      _beg = value;
    if(value > _end && _isEnd)
      _end = value;
    if(_isSize)
      _size += 1;
  }
  //----------------------------------------------------------------------------
  else
    return false;
return true;
}

bool DqTH1D::setting()
{
 
  // Rice Rule CALCULATION ------------------------------------------------- 
  if(_mod == "default") {
    if((1000 > (int)std::abs(_end - _beg)) && _isSize) {
      _size = std::abs(_end - _beg);
    }
    else if(_isSize)
      _size = 1000;
    _beg = _beg - std::abs(_end-_beg)/_size/2;
    _end = _end + std::abs(_end-_beg)/_size/2;
  }
  if(_mod == "rice") { 
    if(_isSize)
      _size = (int)(2*pow(_size, 0.333));
      _beg = _beg - std::abs(_end-_beg)/_size/2;
      _end = _end + std::abs(_end-_beg)/_size/2;
  }
  if(_mod == "strips") {
      _size = std::abs(_end - _beg) + 1;
      _beg = _beg - 0.5;
      _end = _end + 0.5;
  }
  //----------------------------------------------------------------------------
  
  this->SetBins(_size, _beg, _end);
  return true;
}
