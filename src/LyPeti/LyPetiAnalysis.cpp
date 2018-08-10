#include "LyPetiAnalysis.hpp"
#include <iostream>
#include <sstream> 
#include <iomanip>
/* Constructor, destructor. */ 
LyPetiAnalysis::LyPetiAnalysis() 
{
  _isSkipEvent = false;
  _numberOfEvents = 0;
  _isTimeOffsetHR = false; _isTimeOffsetLR = false;
  _isTimeOffset = true; //add
	_limit = +1000000;
  
  _numBoards = 4;   
  // filters
  _isAll = false;  
  
  _isTrigers = false;  
  _thrTrigers = 1;  
  
  _isWindow = false; _isNoise = false;  
  _begWindowHR = 0; _begWindowLR = 0; _begNoise = 0;  
  _endWindowHR = 0; _endWindowLR = 0; _endNoise = 0;  
  
  _isBCID = false;  
  _begBCID = 0;  
  _endBCID = 0;

  _isDeadTime = true;
  _deadTime = 375;
  _numDeadTimeEvents = 0;

   _numEffTrigers = 0;
   _numSkipedTrigers = 0;

  this->clearEvent();
  this->clearRun();
  
  // strips analysis
   _isStripsFill = false;  _isNoiseFill = false;  
   _numEffEventsHR = 0; _numEffNoiseEventsHR = 0;
   _numEffEventsLR = 0; _numEffNoiseEventsLR = 0;
   _numEffEventsOR = 0; _numEffNoiseEventsOR = 0;
   _numEffEventsAND = 0;_numEffNoiseEventsAND = 0;
  
   // clusterBasic
  _isClusterBasic = false; _isNoiseClusterBasic = false;
  _dataCB = 4;
	_timeThrCB = 1000; 
  
  _numEffEventsCB = 0;
  _goodCluster=0; _badCluster=0;

  // noise analysis
  _numNoiseHitsHR = 0; _numNoiseHitsLR = 0; _numNoiseHitsOR = 0; _numNoiseHitsAND = 0; _numNoiseHitsCB = 0;
  _noiseUnit = 1;
  _chamberArea = 1;

  _isMinMaxTime = true;
  _minTime = 0;
  _maxTime = 0;
  _isFoundTriger = false;
}
LyPetiAnalysis::~LyPetiAnalysis() {
}
//------------------------------------------------------------------------------

/* Set Functions. */ 
bool LyPetiAnalysis::setParams(std::map<std::string, double> params) 
{
   for(auto& it: params) {
    _params.insert(std::pair<std::string, double>(it.first, it.second));
  } 
  return true;
}
bool LyPetiAnalysis::setRuns(std::vector<int> runs, std::vector<double> values) 
{
  bool result = false;  
  std::vector<int> sizes;
  sizes.push_back(runs.size()); 
  sizes.push_back(values.size()); 
  if(!(count(sizes.begin(), sizes.end(), sizes[0]) == (int)sizes.size())) {
   result = false;
   for(unsigned int i = 0 ; i < runs.size(); i++)
    _runs.insert(std::pair<int, double>(runs.at(i), runs.at(i))); 
  }
  else {
   result = true;
   for(unsigned int i = 0 ; i < runs.size(); i++)
    _runs.insert(std::pair<int, double>(runs.at(i), values.at(i))); 
  }
  return result;
}
bool LyPetiAnalysis::setParser(DqParser* parser)
{
    _parser = parser;
    return true;
}
bool LyPetiAnalysis::setStrips(std::vector<lyBuf::strip>* strips)
{
    _strips = strips;
    return true;
}
bool LyPetiAnalysis::setNumBoards(int numBoards)
{
    _numBoards = numBoards;
    return true;
}
//------------------------------------------------------------------------------

/* Get Functions. */ 
double LyPetiAnalysis::getValue(int run)
{
	auto it = _runs.find(run);
  if(it == _runs.end())
		return run;
	return it->second;
}
int LyPetiAnalysis::getNumberEvents()
{
	return _numberOfEvents;
}
std::string LyPetiAnalysis::filtersStr() 
{
  return _filters;
}
std::string LyPetiAnalysis::modParamsStr(std::string mod) 
{

  std::string params = "";
  if(_parser->last(mod) == "CB") {
    std::stringstream ss;
    std::cout.precision(1); 
    ss << std::fixed << std::setprecision(3) << "&timeThr_" << _timeThrCB << "ns";
    ss << std::fixed << std::setprecision(3) << "&data_" << _dataCB << "mod";
    params += ss.str();
  }
  return params;
}

bool LyPetiAnalysis::skipEvent() 
{
  double thrMin = 0;
    thrMin = _begNoise;

  if((thrMin < _minTime) && _isMinMaxTime && _isFoundTriger) {
    _isSkipEvent = true;
  //std::cout <<std::setprecision(5)  << thrMin << " " << _minTime << "  HRsize:" << _HR.size() << " LRsize" <<  _LR.size() << std::endl;
  }
  return _isSkipEvent;
}

/* Take data. */ 
std::vector<std::pair<double, double>> LyPetiAnalysis::data(std::string mod)
{
  std::string name = _parser->last(mod);
  std::string param = _parser->first(mod);
  std::vector<std::pair<double, double>> data;
  if(name == "HR" || name == "LR" || 
     name == "AND" || name == "OR")
    this->stripsData(&data, mod);
  if(name == "CB")
    this->clusterBasicData(&data, mod);
  return data;
}
bool LyPetiAnalysis::stripsData(std::vector<std::pair<double, double>> *data, std::string mod) 
{
  std::string name = _parser->last(mod);
  bool result = false;
  if(name == "HR") {
    result = true;
    *data = _HR;
  }
  else if(name == "LR") {
    result = true;
    *data = _LR;
  }
  else if(name == "OR") {
    result = true;
    *data = _OR;
  }
  else if(name == "AND") {
    result = true;
    *data = _AND;
  }
  else
    return false;
   return result;
}
bool LyPetiAnalysis::clusterBasicData(std::vector<std::pair<double, double>> *data, std::string mod) 
{
  std::string param = _parser->first(mod);
  bool result = false;
  if(param == "central") {
    result = true;
    for(auto& it: _CB)
      data->push_back(std::pair<double, double>(it.second.centralStrip, it.second.centralTime));
  }
   return result;
}
//------------------------------------------------------------------------------

/* Clear data (per RUN and per Event). */ 
bool LyPetiAnalysis::clear(std::string type)
{
  if(type == "EVENT") {
    this->clearEvent();
  }
  if(type == "RUN") {
    this->clearEvent();
    this->clearRun();
  }
  return true;
}
bool LyPetiAnalysis::clearEvent()
{
    if(_isSkipEvent)
      _numSkipedTrigers += 1;
    _isSkipEvent = false;
    
    _numNoiseHitsHR += _noiseHR.size(); 
    _numNoiseHitsLR += _noiseLR.size(); 
    _numNoiseHitsAND += _noiseAND.size();  
    _numNoiseHitsOR += _noiseOR.size();     
    _numNoiseHitsCB += _noiseCB.size();
    
    _numberOfEvents+=1;
    _iHR.clear();_iNoiseHR.clear();
    _iLR.clear();_iNoiseLR.clear();
    
    _isStripsFill = false; _isNoiseFill = false;
    _HR.clear(); _noiseHR.clear();
    _LR.clear(); _noiseLR.clear();
    _OR.clear(); _noiseOR.clear();
    _AND.clear(); _noiseAND.clear();
    

    _isClusterBasic = false; _isNoiseClusterBasic = false;
    _CB.clear(); _noiseCB.clear();
    
    
    _minTime = 0; _maxTime = 0;

    _isFoundTriger = false;
    return true;
}
bool LyPetiAnalysis::clearRun()
{
    _numNoiseHitsHR = 0; _numNoiseHitsLR = 0; _numNoiseHitsOR = 0; _numNoiseHitsAND = 0; _numNoiseHitsCB = 0;
   _numberOfEvents = 0;
   _numDeadTimeEvents = 0;
   _numEffEventsHR = 0; _numEffNoiseEventsHR = 0;
   _numEffEventsLR = 0; _numEffNoiseEventsLR = 0;
   _numEffEventsOR = 0; _numEffNoiseEventsOR = 0;
   _numEffEventsAND = 0; _numEffNoiseEventsAND = 0;
   
   _numEffEventsCB = 0; _numEffNoiseEventsCB = 0;
  _goodCluster=0; _badCluster=0;
   
   _numEffTrigers = 0;
   _numSkipedTrigers = 0;
    return true;
}
//------------------------------------------------------------------------------

bool LyPetiAnalysis::configure()
{
  double timeOffSetHR = 0;
  bool isTimeOffsetHR = this->findParam("offsetHR_isUse", &timeOffSetHR);
  if(isTimeOffsetHR && timeOffSetHR == 1)
    _isTimeOffsetHR = true;
  
  double timeOffSetLR = 0;
  bool isTimeOffsetLR = this->findParam("offsetLR_isUse", &timeOffSetLR);
  if(isTimeOffsetLR && timeOffSetLR == 1)
    _isTimeOffsetLR = true;
  
	this->findParam("all_is", &_isAll);  
  
  double numBoards = 0;
  bool isNumBoard = this->findParam("numBoard", &numBoards);
  if(isNumBoard)
    _numBoards = (int)numBoards;
  else 
    _numBoards = 0;
  
  this->findParam("minMaxTime_is", &_isMinMaxTime);  
  bool isTrigers = this->findParam("triger_is", &_isTrigers);  
  if(isTrigers) {
    double thrTrigers = 0;
    bool isTrigers = this->findParam("triger_thr", &thrTrigers);
    if(!isTrigers && thrTrigers >= 0)
      return false;
    _thrTrigers = (int)thrTrigers;
  }
  
  bool isWindow = this->findParam("window_is", &_isWindow);  
  if(isWindow) {
    bool isBegWindow = this->findParam("windowLR_beg", &_begWindowLR);  
    bool isEndWindow = this->findParam("windowLR_end", &_endWindowLR);
    if(!isBegWindow || !isEndWindow)
      return false;  
  }  
  if(isWindow) {
    bool isBegWindow = this->findParam("windowHR_beg", &_begWindowHR);  
    bool isEndWindow = this->findParam("windowHR_end", &_endWindowHR);
    if(!isBegWindow || !isEndWindow)
      return false;  
  }  
  bool isNoise = this->findParam("noise_is", &_isNoise);  
  if(isNoise) {
    bool isBegNoise = this->findParam("noise_beg", &_begNoise);  
    bool isEndNoise = this->findParam("noise_end", &_endNoise);
    if(!isBegNoise || !isEndNoise)
      return false;  
  }  
  
  bool isBCID = this->findParam("BCID_is", &_isBCID);  
  if(isBCID) {
    bool isBegBCID = this->findParam("BCID_beg", &_begBCID);  
    bool isEndBCID = this->findParam("BCID_end", &_endBCID);
    if(!isBegBCID || !isEndBCID)
      return false;  
  } 
  
  bool isDeadTime = this->findParam("deadTime_is", &_isDeadTime);  
  if(isDeadTime) {
    double deadTimeWindow = 0;
    bool isDeadTimeWindow = this->findParam("deadTime_window", &deadTimeWindow);
    if(isDeadTimeWindow)
      _deadTime = deadTimeWindow;
  } 
  bool isTimeThrCB = this->findParam("CB_timeThr", &_timeThrCB);
  if(!isTimeThrCB)
    _timeThrCB = 100;
  double dataCB = 0;
  bool isDataCB = this->findParam("dataCB_HR(1)_LR(2)_OR(3)_AND(4)", &dataCB);
  if(isDataCB)
    _dataCB = (int)dataCB;
  else 
    _dataCB = 5;

  
  double noiseUnit = 1;
  bool isNoiseUnit = this->findParam("noiseUnit", &noiseUnit);
  if(isNoiseUnit)
    _noiseUnit = noiseUnit;
  else 
    _noiseUnit = 1;
  
  double chamberArea = 1;
  bool isChamberArea = this->findParam("chamberArea", &chamberArea);
  if(isChamberArea)
    _chamberArea = chamberArea;
  else 
    _chamberArea = 1;
  // ---
  
  _filters = "";
  std::stringstream ss;
  std::cout.precision(1); 
  //if(_isWindow) {
  //  ss << std::fixed << std::setprecision(2) << "&window_" << _begWindow << "_" << _endWindow << "ns";
  //}
  //if(_isBCID) {
  //  ss << std::fixed << std::setprecision(2) << "&BCID_" << _begBCID << "_" << _endBCID;
  //}
  _filters += ss.str();
  

  
  return true;
}
//------------------------------------------------------------------------------

/* filters */ 
bool LyPetiAnalysis::filters(std::string radius, unsigned int js, unsigned int jr) 
{
  std::vector<bool> results;
  if(_isAll)
    results.push_back(true);
  if(_isTrigers)
    results.push_back(this->triger(js));
  if(_isWindow && _isTrigers)
    results.push_back(this->window(radius, js, jr));
 // if(_isBCID)
 //   results.push_back(this->BCID(radius, js, jr));

  bool result = true;
  if(results.size() == 0)
    result = false; 
  for(unsigned int i = 0; i < results.size(); i++) {
    if(results.at(i) == false)
      result = false;
  } 
  return result; 
}
bool LyPetiAnalysis::triger(unsigned int js)
{
  bool result = false;
  if(_strips->at(js).trigs.size() <= (unsigned int)_thrTrigers && _strips->at(js).trigs.size() > 0)
    result = true;
  return result;
}
bool LyPetiAnalysis::window(std::string radius, unsigned int js, unsigned int jr) 
{
  double begWindow = 0;
  double endWindow = 0;
  std::string name = _parser->last(radius);
  std::string param = _parser->first(radius);
  double dTime = 0; 
  double hitTime = 0; 
  double trigTime = 0;
  bool isTriger =  this->triger(js);
  if(isTriger)
    trigTime = (double)_strips->at(js).trigs.at(0)->time;
  
  if(name == "HR") {
    begWindow = _begWindowHR;
    endWindow = _endWindowHR;
    if(_strips->at(js).HR.size() > jr && _strips->size() > js) {
      hitTime = (double)_strips->at(js).HR.at(jr)->time; 
    }
    else
      return false;    
  }
  else if(name == "LR") {
    begWindow = _begWindowLR;
    endWindow = _endWindowLR;
    if(_strips->at(js).LR.size() > jr && _strips->size() > js) {
      hitTime = (double)_strips->at(js).LR.at(jr)->time;
    }
    else
      return false; 
  }
  else
    return false; 
    
  dTime = hitTime - trigTime;
  if((dTime < _minTime) && _isMinMaxTime)
    _minTime = dTime;
  if((dTime > _maxTime) && _isMinMaxTime)
    _maxTime = dTime;
  if((param == "noise") && _begNoise <= dTime && _endNoise >= dTime)  
    return true;
  else if((param == "data") && begWindow <= dTime && endWindow >= dTime)  
    return true;
  //else if((param == "deadTime") && _begWindow - _deadTime <= dTime && _begWindow > dTime) {  
  //  if(_isDeadTime)
  //    _isSkipEvent = true;
  //  return false;
  //}
  return false;
}
bool LyPetiAnalysis::BCID(std::string radius, unsigned int js, unsigned int jr) 
{
  // не правильно
   std::string name = _parser->last(radius);
  double BCID = 0;
  if(name == "HR") {
    BCID = _strips->at(js).HR.at(jr)->BCID;
  }
  else if(name == "LR") {
    BCID = _strips->at(js).LR.at(jr)->BCID;
  }
  else
    return false;
  if(_begBCID <= BCID && _endBCID >= BCID)  
    return true;
  else
    _isSkipEvent = true;


  return false;
}
//------------------------------------------------------------------------------


/* Analysis and algorithms */ 
bool LyPetiAnalysis::algos(std::string mod)
{
  std::string name = _parser->last(mod);
  if(name == "HR" || name == "LR" || 
     name == "AND" || name == "OR") {
    this->stripsAlgos();
  }
  if(name == "CB")
    this->clusterBasicAlgos();
  return true;
}
bool LyPetiAnalysis::stripsAlgos()
{
  if(_isNoise)
    this->choiceDataFill("noise");
  this->choiceDataFill("data");
  return true;
}
bool LyPetiAnalysis::clusterBasicAlgos()
{
  if(_isNoise)
    this->choiceDataFill("noise");
  this->choiceDataFill("data");
  if(_isNoise)
    this->choiceDataClusterBasic("noise");
  this->choiceDataClusterBasic("data");
    return true;
}

bool LyPetiAnalysis::efficiency(double* eff, double* eEff, std::string mod)
{
  std::string name = _parser->last(mod);
  std::string param = _parser->last(mod);
  double windowFactor = 0;
  double events = 0;
  double noise = 0;
  double trigers = _numEffTrigers -_numSkipedTrigers;
  double noiseTrigers = _numEffTrigers - _numSkipedTrigers;
  if(name == "HR") {
      events = _numEffEventsHR;
      noise = _numEffNoiseEventsHR;
      windowFactor = (std::abs(_endNoise - _begNoise))/std::abs(_endWindowHR - _begWindowHR);
  } else if(name == "LR") {
      events = _numEffEventsLR;
      noise = _numEffNoiseEventsLR;
      windowFactor = (std::abs(_endNoise - _begNoise))/std::abs(_endWindowLR - _begWindowLR);
  } else if(name == "OR") {
      events = _numEffEventsOR;

      windowFactor = (std::abs(_endNoise - _begNoise))/std::abs((_endWindowHR + _endWindowLR)/2 - (_begWindowHR+_begWindowLR)/2);
  } else if(name == "AND") {
      events = _numEffEventsAND;
      noise = _numEffNoiseEventsAND;
      windowFactor = (std::abs(_endNoise - _begNoise))/std::abs((_endWindowHR+_endWindowLR)/2 - (_begWindowHR+_begWindowLR)/2);
  } else if(name == "CB") {
      events = _numEffEventsCB;
      noise = _numEffNoiseEventsCB;
      windowFactor = (std::abs(_endNoise - _begNoise))/std::abs((_endWindowHR+_endWindowLR)/2 - (_begWindowHR+_begWindowLR)/2);
  } 
  else 
    return false;
 
  if(trigers == 0)
    return false;
  //----

  double efficiency = (events)/trigers; 
  double eEfficiency = std::sqrt((efficiency*(1-efficiency)))/std::sqrt(trigers);
  double effNoise = (noise/windowFactor)/noiseTrigers;
  double eEffNoise = std::sqrt((effNoise*(1-effNoise)))/std::sqrt(trigers);
  *eff = efficiency;
  *eEff = eEfficiency;
  if(_isNoise) {
    *eff = (efficiency - effNoise)/(1-effNoise);
    //std::cout << std::setprecision(5) << events << " " << trigers << " " << efficiency << " " << effNoise << std::endl; 
    *eEff = eEfficiency + eEffNoise;
  }
  if(trigers < 0 || events < 0) {
    *eff = 0;
    *eEff = 0;
  }
    

  //----
  return true;
}
bool LyPetiAnalysis::efficiency(std::string mod)
{
  std::string name = _parser->last(mod);
  bool result = false;
  if(name == "HR" ||  name == "LR" || name == "AND" || name == "OR")
    result = this->stripsEfficiency(mod);
  if(name == "CB")
    result = this->clusterBasicEfficiency(mod);
  if(name == "NOISE")
    result = true;
  return result;
}
bool LyPetiAnalysis::stripsEfficiency(std::string mod)
{
  std::string name = _parser->last(mod);
  bool result = false;
  if(name == "HR") {
    result = true;
    if(_iNoiseHR.size() > 0)
      _numEffNoiseEventsHR += 1;
    if(_HR.size() > 0)
      _numEffEventsHR += 1;
  }
  else if(name == "LR") {
    result = true;
    if(_iNoiseLR.size() > 0)
      _numEffNoiseEventsLR += 1;
    if(_LR.size() > 0)
      _numEffEventsLR += 1;
  }
  else if(name == "OR") {
    result = true;
    if(_iNoiseHR.size() > 0 || _iNoiseLR.size() > 0)
      _numEffNoiseEventsOR += 1;
    if(_HR.size() > 0 || _LR.size() > 0)
      _numEffEventsOR += 1;
  }
  else if(name == "AND") {
    result = true;
    // noise 
    bool isNoiseAND = false;
    std::vector<std::pair<int, int>> noiseHR = _iNoiseHR;
    std::vector<std::pair<int, int>> noiseLR = _iNoiseLR;
	  this->sortAndResize(&noiseHR, &noiseLR); 
    for(unsigned int i = 0; i < noiseHR.size(); i++) {
      if(isNoiseAND == true)
          break;
      for(unsigned int j = 0; j < noiseLR.size(); j++) {
        if(noiseHR.at(i).first == noiseLR.at(j).first) {
          isNoiseAND = true;
          noiseLR.erase(noiseLR.begin()+j);
          break;
				}
      }
    }
    if(isNoiseAND)
      _numEffNoiseEventsAND += 1;
    // data
    bool isAND = false;
    std::vector<std::pair<int, int>> HR = _iHR;
    std::vector<std::pair<int, int>> LR = _iLR;
	  this->sortAndResize(&HR, &LR); 
    for(unsigned int i = 0; i < HR.size(); i++) {
      if(isAND == true)
          break;
      for(unsigned int j = 0; j < LR.size(); j++) {
        if(HR.at(i).first == LR.at(j).first) {
          isAND = true;
          LR.erase(LR.begin()+j);
          break;
				}
      }
    }
    if(isAND)
      _numEffEventsAND += 1;
  }
  else
    result = false;
  
  return result;
}
bool LyPetiAnalysis::clusterBasicEfficiency(std::string mod)
{
    if(_noiseCB.size() > 0)
      _numEffNoiseEventsCB += 1;
    if(_CB.size() > 0)
      _numEffEventsCB += 1;
  return true;
}

bool LyPetiAnalysis::noise(double* n, double* eN, std::string mod)
{
  std::string name = _parser->last(mod);
  std::string param = _parser->first(mod);
  double noise = 0;
  double trigers = _numEffTrigers - _numSkipedTrigers;
  if(name == "HR")
      noise = _numNoiseHitsHR;
  else if(name == "LR")
      noise = _numNoiseHitsLR;
  else if(name == "OR") 
      noise = _numNoiseHitsOR;
  else if(name == "AND")
      noise = _numNoiseHitsAND;
  else if(name == "CB")
      noise = _numNoiseHitsCB;
  else 
    return false;
 
  if(trigers == 0)
    return false;
  //----
  if(trigers != 0) {
   // if(noise*_deadTime < std::abs(_endNoise-_begNoise)*trigers) 
   //   *n = noise/(((std::abs(_endNoise-_begNoise))*trigers - noise*_deadTime)*_noiseUnit*_chamberArea);
      *n = noise/(((std::abs(_endNoise-_begNoise))*trigers)*_noiseUnit*_chamberArea);
   // else 
   //   *n = -100;
  }
  else
    *n = 0;
 // std::cout << std::setprecision(5) << name << " RESULT:" << *n << " N:" << noise << " W: " << std::abs(_endNoise-_begNoise) << " U:" << _noiseUnit <<" T: " << trigers << " Surf:" << _chamberArea << std::endl;  
  *eN = 0; 
  //----
  return true;
}

bool LyPetiAnalysis::getNumSkipedTrigers(double* num)
{
  *num = (double)_numSkipedTrigers;
  return true;
}
bool LyPetiAnalysis::getNumTrigers(double* num)
{
  *num = (double)_numEffTrigers;
  return true;
}
bool LyPetiAnalysis::eventFraction(double* fraction, std::string mod) 
{
  bool result = false;

  std::string name = _parser->last(mod);
  if(name == "CB") {
    *fraction = (double)_badCluster/((double)_numEffTrigers); // -(double)_numSkipedTrigers);
    result = true;
  }
  return result;
}

bool LyPetiAnalysis::offSetTime(std::vector<std::pair<double, double>>* data, 
                                std::map<int, std::vector<double>>* output, std::string mod) 
{
  std::string name = _parser->last(mod);
  std::string param = _parser->first(mod);
  if((name == "HR" || name == "LR" || name == "AND") && param == "offset") {
    //std::cout << name << " " << param << std::endl; 
    
    unsigned int refS = 1028;
    unsigned int refS1 = 1020;
    unsigned int refS2 = 1030;
    std::map<int, std::map<int, std::vector<double>>> strips;
    std::map<int, std::vector<double>> strip;
    for(unsigned int s = refS1; s <= refS2; s++) {
      
      double refTime = 0;
      int count = 0;
      for(unsigned int i = 0; i < data->size(); i++) {
        if(data->at(i).first == s) {
          count += 1;
          refTime = data->at(i).second;
        }
      }
      if(count != 1)
        continue;
      
      for(unsigned int i = 0; i < data->size(); i++) {
        auto it = output->find(data->at(i).first);
        if(it == output->end())
          strip.insert(std::make_pair(data->at(i).first, std::vector<double>{data->at(i).second - refTime}));
        strip.find(data->at(i).first)->second.push_back(data->at(i).second - refTime);
       // std::cout << std::setprecision(9) << data->at(i).first << " " << data->at(i).second << " " <<  refTime << std::endl; 
      }
      strips.insert(std::make_pair(s, strip));
    }
     for(auto& s: strips) {
      double sum = 0;
      for(unsigned int sl = s.first; sl < refS; sl++) {
        if(strips.find(sl)->second.find(sl+1)->second.size() > 0) {
          double closest = strips.find(sl)->second.find(sl+1)->second.at(0);
          for(unsigned int t = 0; t < strips.find(sl)->second.find(sl+1)->second.size(); t++) {
            if(closest > strips.find(sl)->second.find(sl+1)->second.at(t))
              closest = strips.find(sl)->second.find(sl+1)->second.at(t);
          }
          sum +=  closest;
        }
      }
      for(unsigned int sh = s.first; sh > refS; sh--) {
        if(strips.find(sh)->second.find(sh-1)->second.size() > 0) {
          double closest = strips.find(sh)->second.find(sh+1)->second.at(0);
          for(unsigned int t = 0; t < strips.find(sh)->second.find(sh+1)->second.size(); t++) {
            if(closest > strips.find(sh)->second.find(sh+1)->second.at(t))
              closest = strips.find(sh)->second.find(sh+1)->second.at(t);
          }
            sum += closest;
        }
      }
      output->insert(std::make_pair(s.first, std::vector<double>{sum}));
    }
    return true;
    //int refStrip = 1020;
    //double refTime = 0;

    //int count = 0;
    //for(unsigned int i = 0; i < data->size(); i++) {
    //  if(data->at(i).first == refStrip) {
    //    count += 1;
    //    refTime = data->at(i).second;
    //  }
    //}
    //if(count != 1)
    //  return false;

    //for(unsigned int i = 0; i < data->size(); i++) {
    //  auto it = output->find(data->at(i).first);
    //  if(it == output->end())
    //    output->insert(std::make_pair(data->at(i).first, std::vector<double>{data->at(i).second - refTime}));
    //  output->find(data->at(i).first)->second.push_back(data->at(i).second - refTime);
    // // std::cout << std::setprecision(9) << data->at(i).first << " " << data->at(i).second << " " <<  refTime << std::endl; 
    //}
    //return true;
  }
  else 
    return false;
}
bool LyPetiAnalysis::multiplicity(int* mult, std::string mod) 
{
  std::string name = _parser->last(mod);
  bool result = false;
  if(name == "HR" || name == "LR" || 
     name == "AND" || name == "OR")
    result = this->stripsMultiplicity(mult, mod);
  if(name == "CB")
    result = this->clusterBasicMultiplicity(mult, mod);
  return result;
}

bool LyPetiAnalysis::stripsMultiplicity(int* mult, std::string mod) 
{
  std::string name = _parser->last(mod);
  bool result = false;
  double multiplicity = 0;
  if(name == "HR") {
      multiplicity = _iHR.size();
      result = true;
  } else if(name == "LR") {
      multiplicity = _iLR.size();
      result = true;
  } else if(name == "OR") {
      result = true;
      std::vector<std::pair<int, int>> HR = _iHR;
      std::vector<std::pair<int, int>> LR = _iLR;
	    this->sortAndResize(&HR, &LR); 
      for(unsigned int i = 0; i < HR.size(); i++) {
        for(unsigned int j = 0; j < LR.size(); j++) {
          if(HR.at(i).first == LR.at(j).first) {
            multiplicity += 1;
            LR.erase(LR.begin()+j);
            break;
				  }
        }
      }
      multiplicity += LR.size();
  } else if(name == "AND") {
      result = true;
      std::vector<std::pair<int, int>> HR = _iHR;
      std::vector<std::pair<int, int>> LR = _iLR;
	    this->sortAndResize(&HR, &LR); 
      for(unsigned int i = 0; i < HR.size(); i++) {
        for(unsigned int j = 0; j < LR.size(); j++) {
          if(HR.at(i).first == LR.at(j).first) {
            multiplicity += 1;
            LR.erase(LR.begin()+j);
            break;
				  }
        }
      }
  } else 
        result = false;
  
  *mult = multiplicity; 
  return result;
}
bool LyPetiAnalysis::clusterBasicMultiplicity(int* mult, std::string mod) 
{
  double multiplicity = 0;
  multiplicity = _CB.size();
  *mult = multiplicity;
  return true;
}

bool LyPetiAnalysis::clusterSize(std::vector<int>* sizes, std::string mod)
{
  std::string name = _parser->last(mod);
  if(name == "CB") {
      for(auto& it: _CB)
    sizes->push_back(it.second.size);
  }
  else 
    return false;
  return true;
}

bool LyPetiAnalysis::minTime(double* time, std::string mod) 
{
  std::string name = _parser->last(mod);
  bool result = false;
  if(name == "HR" || name == "LR") {
    *time = _minTime;
    result = true;
  }
  else
    result = false;
    return result;
}
bool LyPetiAnalysis::maxTime(double* time, std::string mod) 
{
  std::string name = _parser->last(mod);
  bool result = false;
  if(name == "HR" || name == "LR") {
    *time = _maxTime;
    result = true;
  }
  else
    result = false;
    return result;
}
//------------------------------------------------------------------------------

/* Fill data */ 
bool LyPetiAnalysis::fillTrigers(int numBoards) 
{
 	bool result = false; 
	if(_numBoards == numBoards) {
   _numEffTrigers += 1;
	 result = true;
    _isFoundTriger = true;
	}
  else
    _isFoundTriger = false;
  return result; 
}
bool LyPetiAnalysis::fillEvents(std::string radius, unsigned int js, unsigned int jr) 
{
 	bool result = false; 
   std::string name = _parser->last(radius);
   std::string param = _parser->first(radius);
  if(param == "noise" && name == "HR") {
    _iNoiseHR.push_back(std::pair<int, int>((int)js, (int)jr));
  } else if(name == "HR") {
    _iHR.push_back(std::pair<int, int>((int)js, (int)jr));
    result = true;
  }
  if(param == "noise" && name == "LR") {
    _iNoiseLR.push_back(std::pair<int, int>((int)js, (int)jr));
    result = true;
  } else if(name == "LR") {
    _iLR.push_back(std::pair<int, int>((int)js, (int)jr));
    result = true;
  }
  return result; 
}
/* Strips Data */ 
bool LyPetiAnalysis::choiceDataFill(std::string param) 
{
  bool result = false;
  if(!_isStripsFill) {
    if(param == "data") {
      this->dataFillHR(&_iHR, &_HR, false, false);
      this->dataFillLR(&_iLR, &_LR, false);
      this->dataFillAndOr(&_iHR, &_iLR, &_OR, &_AND);
      _isStripsFill = true;
      result = true;
    }
  }
  if(!_isNoiseFill) {
    if(param == "noise") {
      this->dataFillHR(&_iNoiseHR, &_noiseHR, false, false);
      this->dataFillLR(&_iNoiseLR, &_noiseLR, false);
      this->dataFillAndOr( &_iNoiseHR, &_iNoiseLR, &_noiseOR, &_noiseAND);
      _isNoiseFill = true;
      result = true;
    }
  }
  return result;
}
bool LyPetiAnalysis::dataFillHR(std::vector<std::pair<int, int>>* iHR, 
                                std::vector<std::pair<double, double>>* output, bool inverse, bool isOffSet) 
{
  int numStrip = 0; double trigTime = 0; double hitTime = 0; 
  for(unsigned int ih = 0; ih < iHR->size(); ih++) {
    hitTime = 0;
    numStrip = 0;
    numStrip = _strips->at(iHR->at(ih).first).number;
    hitTime = (double)_strips->at(iHR->at(ih).first).HR.at(iHR->at(ih).second)->time;
    if(_isTimeOffsetHR) {
			hitTime = hitTime - _strips->at(iHR->at(ih).first).timeOffSetHR;
      //std::cout << _strips->at(iHR->at(ih).first).timeOffSetHR << std::endl;
    }
    if(isOffSet) {
			hitTime = hitTime - _strips->at(iHR->at(ih).first).timeOffSet;
		  //hitTime -= 5;
    //    std::cout << _strips->at(iHR->at(ih).first).number << " " << _strips->at(iHR->at(ih).first).timeOffSet << " " << std::endl;
    }
    trigTime = (double)_strips->at(iHR->at(ih).first).trigs.at(0)->time;
    double time = (hitTime - trigTime);
    if(inverse)
      time = time*_strips->at(iHR->at(ih).first).inverse;
    else
      time = time;

    if(_strips->at(iHR->at(ih).first).inverse != 0)
      output->push_back(std::pair<double, double>((double)numStrip, time));
  }
  return true;
}
bool LyPetiAnalysis::dataFillLR(std::vector<std::pair<int, int>>* iLR,
                                std::vector<std::pair<double, double>>* output, bool inverse) 
{
  int numStrip = 0; double trigTime = 0; double hitTime = 0; 
  for(unsigned int il = 0; il < iLR->size(); il++) {
    numStrip = _strips->at(iLR->at(il).first).number; 
    hitTime = (double)_strips->at(iLR->at(il).first).LR.at(iLR->at(il).second)->time;
    if(_isTimeOffsetLR)
      hitTime -= _strips->at(iLR->at(il).first).timeOffSetLR;
    trigTime = (double)_strips->at(iLR->at(il).first).trigs.at(0)->time;
    double time = (hitTime - trigTime);
    if(inverse)
      time = time*_strips->at(iLR->at(il).first).inverse;
    else
      time = time;
    if(_strips->at(iLR->at(il).first).inverse != 0)
      output->push_back(std::pair<double, double>((double)numStrip, time));
  }
  return true;
}
bool LyPetiAnalysis::dataFillAndOr(std::vector<std::pair<int, int>>* iHR,
                                   std::vector<std::pair<int, int>>* iLR,
                                   std::vector<std::pair<double, double>>* OR, 
                                   std::vector<std::pair<double, double>>* AND) 
{
    std::vector<std::pair<double, double>> HR;
    std::vector<std::pair<double, double>> LR;
    if(_isTimeOffset) {
      this->dataFillHR(iHR, &HR, true, true);
      this->dataFillLR(iLR, &LR, true);
    }
    else {
      this->dataFillHR(iHR, &HR, true, false);
      this->dataFillLR(iLR, &LR, true);
    }
    this->sortAndResize(&HR, &LR);
    for(unsigned int i = 0; i < HR.size(); i++) {
      for(unsigned int j = 0; j < LR.size(); j++) {
        if(HR.at(i).first == LR.at(j).first ) {
          for(unsigned int l = 0; l < LR.size(); l++) {
            if(std::abs(LR.at(j).second - HR.at(i).second) > std::abs(LR.at(l).second - HR.at(i).second))
              std::swap(LR[j],  LR[l]);
          }
          double time = HR.at(i).second - LR.at(j).second;
        //  if(_isTimeOffset)
         //   time -= _strips->at(LR.at(j).first).timeOffSet;

          AND->push_back(std::pair<double, double>(HR.at(i).first, time));
//            std::cout << "time: " << HR.at(i).second - LR.at(j).second << "|" << HR.at(i).second << "|" <<  LR.at(j).second << "|" << (int)HR.at(i).first << " ";
          OR->push_back(std::pair<double, double>(HR.at(i).first, 0));
          LR.erase(LR.begin()+j);
          break;
	  		}
        else if(j == LR.size()-1 && HR.at(i).first != _limit)
          OR->push_back(std::pair<double, double>(HR.at(i).first, 0));
      }
    }
    for(unsigned int j = 0; j < LR.size(); j++) {
      if(LR.at(j).first != _limit)
        OR->push_back(std::pair<double, double>(LR.at(j).first, 0));
    }
//    std::cout << std::endl;
  return true;
}
/* Basic Cluster Data */ 
bool LyPetiAnalysis::choiceDataClusterBasic(std::string param) 
{
  bool result = false;
  if(!_isClusterBasic) {
    if(param == "data") {
      if(_dataCB == 1)
        this->clusterBasic(_HR, &_CB);
      else if(_dataCB == 2)
        this->clusterBasic(_LR, &_CB);
      else if(_dataCB == 3)
        this->clusterBasic(_OR, &_CB);
      else if(_dataCB == 4)
        this->clusterBasic(_AND, &_CB);
      _isClusterBasic = true;
      result = true;
    }
  }
  if(!_isNoiseClusterBasic) {
    if(param == "noise") {
      if(_dataCB == 1)
        this->clusterBasic(_noiseHR, &_noiseCB);
      else if(_dataCB == 2)
        this->clusterBasic(_noiseLR, &_noiseCB);
      else if(_dataCB == 3)
        this->clusterBasic(_noiseOR, &_noiseCB);
      else if(_dataCB == 4)
        this->clusterBasic(_noiseAND, &_noiseCB);
      _isNoiseClusterBasic = true;
      result = true;
    }
  }
  return result;
}
bool LyPetiAnalysis::clusterBasic(std::vector<std::pair<double, double>> data, std::map<int, lyCB::data>* output) 
{
//  for(unsigned int i = 0; i < _clusterMask.size(); i++)
//    data.push_back(std::pair<double, double>(_clusterMask.at(i), 0));
    bool isDead = false;
    std::vector<int> deadStrips = {36, 1001, 1006, 1011, 1013, 1017, 1037, 1043};
    double timeThr = _timeThrCB; 
    std::sort(data.begin(), data.end(), [](auto &left, auto &right) {
      return std::abs(left.first) < std::abs(right.first);
	  });

    //if(data.size() > 0)
    //std::cout << "data: ";
    //  for(unsigned int i = 0; i < data.size(); i++)
    //    std::cout << (int)data.at(i).first << "|" << data.at(i).second << " ";
    //if(data.size() > 0)
    //  std::cout << std::endl;
   
//    if(data.size() <= 7) {
      std::vector<std::pair<double, double>> strips;
      int iCluster = 0;
      while(data.size() > 0) {
        std::vector<int> indexes;
        lyCB::data cluster;
        strips.push_back(std::pair<double, double>(data.at(0).first, data.at(0).second));
        data.erase(data.begin());
        for(unsigned int i = 0; i < data.size(); i++) {
          isDead = false;
          for(unsigned int ds = 0; ds < deadStrips.size(); ds++) {
            if((strips.back().first + 1) == deadStrips.at(ds))
              isDead = true;
          }
          if(isDead) {
            strips.push_back(std::pair<double, double>((strips.back().first + 1), strips.back().second));
            indexes.push_back(i);
          }
          else {
            if((data.at(i).first == (strips.back().first + 1)) && std::abs(data.at(i).second - strips.back().second) <= timeThr) {
              strips.push_back(std::pair<double, double>(data.at(i).first, data.at(i).second));
              indexes.push_back(i);
            }
          }
        }
        cluster.size = strips.size();
        
        //std::cout  << "cluster: ";
        //for(unsigned int i = 0; i < strips.size(); i++)
        //  std::cout  << (int)strips.at(i).first << "|" << strips.at(i).second << " ";
        //std::cout << std::endl;
        //
        if(cluster.size > 0) {
          cluster.centralStrip = (int)strips.at(this->centralStrip(&strips)).first;
          cluster.centralTime = strips.at(this->centralStrip(&strips)).second;
          output->insert(std::make_pair(iCluster, cluster));
        }
        // clear
        for(unsigned int i = 0; i < indexes.size(); i++)
          data.erase(data.begin() + indexes.at(i) - i);
        iCluster += 1;
        strips.clear();
        indexes.clear();
      }
//   }
//   else {
//       lyCB::data cluster;
//       cluster.centralStrip = -1;
//       cluster.centralTime = 0;
//       output->insert(std::make_pair(1, cluster));
//      _badCluster+=1;  
//      }
  return true;
}
int LyPetiAnalysis::centralStrip(std::vector<std::pair<double, double>>* strips) 
{
  int size = strips->size(); 
  if(size == 0)
    return 0;
  else if(size%2 == 1)
    return (size/2 + 1) - 1;
  else if(size%2 == 0)
    return (size/2) - 1;
  return 0;
}

//------------------------------------------------------------------------------

bool LyPetiAnalysis::isOffSet(std::string mod)
{
  std::string name = _parser->last(mod);
  std::string param = _parser->first(mod);
  if((name == "HR" || name == "LR" || name == "AND") && param == "OFFSET")
    return true;
  else 
    return false;
}

/* helpful functions. */ 
bool LyPetiAnalysis::findParam(std::string name, double* param)
{
  auto it = _params.find(name);
  if(it == _params.end())
    return true;
  else {
    *param = _params.find(name)->second;
  }
  return true;
}
bool LyPetiAnalysis::findParam(std::string name, bool* param)
{
  double check = 0; 
  *param = false;
  auto it = _params.find(name);
  if(it == _params.end())
    return true;
  else {
    check = _params.find(name)->second;
    if(check == 1)
      *param = true;
    else
      *param = false;
  }
  return true;
}
template <typename T> bool LyPetiAnalysis::sortAndResize(std::vector<std::pair<T, T>> *v1, 
													         std::vector<std::pair<T, T>> *v2) 
{
	std::sort(v1->begin(), v1->end(), [](auto &left, auto &right) {
    return std::abs(left.second) < std::abs(right.second);
	});
	std::sort(v2->begin(), v2->end(), [](auto &left, auto &right) {
    return std::abs(left.second) < std::abs(right.second);
	});
	if(v1->size() >= v2->size()) {
		unsigned int size = v1->size() - v2->size(); 
	  for(unsigned int s = 0; s < size; s++)
    	v2->push_back(std::pair<T,T>(_limit, 0));
	}
	else {
		unsigned int size = v2->size() - v1->size(); 
	  for(unsigned int s = 0; s < size; s++)
    	v1->push_back(std::pair<T, T>(_limit, 0));
	}
  return true;
}
//------------------------------------------------------------------------------
