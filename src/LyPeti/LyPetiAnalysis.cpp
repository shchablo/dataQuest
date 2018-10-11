#include "LyPetiAnalysis.hpp"
#include <sstream> 
#include <iomanip>
/* Constructor, destructor. */ 
LyPetiAnalysis::LyPetiAnalysis() 
{
  _hHR = new TH1F(Form("hHR_%d", _hHR), "HR", 300,  1, -1);
  _hLR = new TH1F(Form("hLR_%d", _hLR), "LR", 300,  1, -1);
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
  _deadTime = 10;
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
    thrMin = -300;
   double thrMax = -10;

  if((thrMin < _minTime) && _isMinMaxTime && _isFoundTriger) {
    _isSkipEvent = true;
  //std::cout <<std::setprecision(5)  << thrMin << " " << _minTime << "  HRsize:" << _HR.size() << " LRsize" <<  _LR.size() << std::endl;
  }
  if((thrMax > _maxTime) && _isMinMaxTime && _isFoundTriger) {
    _isSkipEvent = true;
  //std::cout <<std::setprecision(5)  << thrMin << " " << _minTime << "  HRsize:" << _HR.size() << " LRsize" <<  _LR.size() << std::endl;
  }
  //if(_iHR.size() > 20 || _iLR.size() > 20)
  //  _isSkipEvent = true;
  return _isSkipEvent;
}

/* Take data. */ 
std::vector<std::pair<double, double>> LyPetiAnalysis::data(std::string mod)
{
  std::string name = _parser->last(mod);
  std::string param = _parser->first(mod);
  std::vector<std::pair<double, double>> data;
  if(name == "HR" || name == "LR" || 
     name == "AND" || name == "OR" || "nLR" || "nHR")
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
  else if(name == "nLR") {
    result = true;
    *data = _notLR;
  }
  else if(name == "nHR") {
    result = true;
    *data = _notHR;
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
  if(param == "middle") {
    result = true;
    for(auto& it: _CB)
      data->push_back(std::pair<double, double>(it.second.middleStrip, it.second.middleTime));
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
  if(type == "CONFIG EVENT") {
    //---
    double trigTimeHR = 0; double hitTimeHR = 0; 
    for(unsigned int ih = 0; ih < _iHR.size(); ih++) {
      hitTimeHR = (double)_strips->at(_iHR.at(ih).first).HR.at(_iHR.at(ih).second)->time;
      trigTimeHR = (double)_strips->at(_iHR.at(ih).first).trigs.at(0)->time;
      double timeHR = (hitTimeHR - trigTimeHR);
      _hHR->Fill(timeHR);
    }
    double trigTimeLR = 0; double hitTimeLR = 0; 
    for(unsigned int il = 0; il < _iLR.size(); il++) {
      hitTimeLR = (double)_strips->at(_iLR.at(il).first).LR.at(_iLR.at(il).second)->time;
      trigTimeLR = (double)_strips->at(_iLR.at(il).first).trigs.at(0)->time;
      double timeLR = (hitTimeLR - trigTimeLR);
      _hLR->Fill(timeLR);
    }
    //---
    this->clearEvent();
  }
  if(type == "RUN") {
    this->clearEvent();
    this->clearRun();

  }
  if(type == "CONFIG RUN") {
    this->clearEvent();
    this->clearRun();
    //this->newWindows();
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
    _notHR.clear(); _notLR.clear();
    

    _isClusterBasic = false; _isNoiseClusterBasic = false;
    _CB.clear(); _noiseCB.clear();
    
    
    _minTime = 0; _maxTime = 0;

    _isFoundTriger = false;
    return true;
}
bool LyPetiAnalysis::clearRun()
{
    _begWindowHR = _begWindowHR_def; _begWindowLR = _begWindowLR_def;
    _endWindowHR = _endWindowHR_def; _endWindowLR = _endWindowLR_def;  
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
bool LyPetiAnalysis::newWindows()
{
   double pMean  = _hHR->GetMean();
   double pRMS  = _hHR->GetRMS();
   TF1* g1 = new TF1("g", "gaus",  pMean - 6*pRMS, pMean + 6*pRMS);
   _hHR->Fit("g", "S");
   _begWindowHR = g1->GetParameter(1) - 3*g1->GetParameter(2);
   _endWindowHR = g1->GetParameter(1) + 3*g1->GetParameter(2);
   delete g1;
   delete _hHR;
   
   pMean  = _hLR->GetMean();
   pRMS  = _hLR->GetRMS();
   TF1* g2 = new TF1("g2", "gaus",  pMean - 6*pRMS, pMean + 6*pRMS);
   _hLR->Fit("g2", "S");
   _begWindowLR = g2->GetParameter(1) - 3*g2->GetParameter(2);
   _endWindowLR = g2->GetParameter(1) + 3*g2->GetParameter(2);
   delete g2;
   delete _hLR;
  _hHR = new TH1F(Form("hHR_%d", _hHR), "HR", 300,  1, -1);
  _hLR = new TH1F(Form("hLR_%d", _hLR), "LR", 300,  1, -1);
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
    bool isBegWindow = this->findParam("windowLR_beg", &_begWindowLR_def);  
    bool isEndWindow = this->findParam("windowLR_end", &_endWindowLR_def);
    _begWindowLR = _begWindowLR_def;
    _endWindowLR = _endWindowLR_def;
    if(!isBegWindow || !isEndWindow)
      return false;  
  }  
  if(isWindow) {
    bool isBegWindow = this->findParam("windowHR_beg", &_begWindowHR_def);  
    bool isEndWindow = this->findParam("windowHR_end", &_endWindowHR_def);
    _begWindowHR = _begWindowHR_def;
    _endWindowHR = _endWindowHR_def;
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
  bool isDataCB = this->findParam("dataCB_HR(1)_LR(2)_OR(3)_AND(4)_HRLR(5)", &dataCB);
  if(isDataCB)
    _dataCB = (int)dataCB;
  else 
    _dataCB = 100;

  
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
  
  //_filters = "";
  //std::stringstream ss;
  //std::cout.precision(1); 
  //if(_isWindow) {
  //  ss << std::fixed << std::setprecision(2) << "&window_" << _begWindow << "_" << _endWindow << "ns";
  //}
  //_filters += ss.str();
  

  
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

  //if(result == true && _strips->at(js).trigs[0]->BCID <= 2)
  //  result = false;
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
  if((param == "noise") && ((begWindow-_begNoise <= dTime && endWindow-_begNoise >= dTime)))  
 //     || (begWindow-1000 <= dTime && endWindow-1000 >= dTime)  
 //     || (begWindow-800 <= dTime && endWindow-800 >= dTime)  
 //     || (begWindow-500 <= dTime && endWindow-500 >= dTime)))  
  //if((param == "noise") && _begNoise <= dTime && _endNoise >= dTime)  
    return true;
  else if((param == "data") && begWindow <= dTime && endWindow >= dTime)  
    return true;
  else if((param == "deadTime") && ((begWindow  -_begNoise - _deadTime <= dTime && begWindow - _begNoise > dTime) )) {
  //       || (begWindow -1000 - _deadTime <= dTime && begWindow -1000 > dTime)
  //       || (begWindow -800 - _deadTime <= dTime && begWindow -800 > dTime)
  //       || (begWindow -500 - _deadTime <= dTime && begWindow -500 > dTime))) { 
    if(_isDeadTime)
      _isSkipEvent = true;
    return false;
  }
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
  double effNoise = (noise)/noiseTrigers;
  //double effNoise = (noise)/noiseTrigers;
  //double effNoise = 0; // если заработает - дописать
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
  double begWindow = 0;
  double endWindow = 0;
  double noise = 0;
  double trigers = _numEffTrigers - _numSkipedTrigers;
  if(name == "HR") {
      noise = _numNoiseHitsHR;
      begWindow = _begWindowHR;
      endWindow = _endWindowHR;
  }
  else if(name == "LR") {
      noise = _numNoiseHitsLR;
      begWindow = _begWindowLR;
      endWindow = _endWindowLR;
  }
  else if(name == "OR") { 
      noise = _numNoiseHitsOR;
      begWindow = (_begWindowLR+_begWindowHR)/2;
      endWindow = (_endWindowLR+_endWindowHR)/2;
  }
  else if(name == "AND") {
      noise = _numNoiseHitsAND;
      begWindow = (_begWindowLR+_begWindowHR)/2;
      endWindow = (_endWindowLR+_endWindowHR)/2;
  }
  else if(name == "CB") {
      noise = _numNoiseHitsCB;
      begWindow = (_begWindowLR+_begWindowHR)/2;
      endWindow = (_endWindowLR+_endWindowHR)/2;
  }
  else 
    return false;
 
  if(trigers == 0)
    return false;
  //----
  if(trigers != 0) {
    //*n = noise/((std::abs(_endNoise-_begNoise)*trigers-std::abs(noise*10))*_noiseUnit*_chamberArea);
    *n = noise/(((std::abs((endWindow-begWindow)))*trigers)*_noiseUnit*_chamberArea);
    //*n = noise/(((std::abs((_endNoise-_begNoise)))*trigers)*_noiseUnit*_chamberArea);
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

bool LyPetiAnalysis::offSetTime(int refStrip, std::vector<std::pair<double, double>>* data,
                                std::vector<std::pair<double, double>>* dataAND,
                                std::map<int, std::vector<double>>* output, std::string mod) 
{
    double time1 = 0; double time2 =0;
    if(data->size() != 3)
      return true;
    std::string name = _parser->last(mod);
    //std::string param = _parser->first(mod);
    double refTime = 0;
    int count = 0;
    bool isRef = false;
    for(unsigned int i = 0; i < data->size(); i++) {
      if(data->at(i).first == refStrip) {
        int isGoodHit = false;
        if(std::abs(refTime) <  std::abs(data->at(i).second)) {
          for(unsigned int n = 0; n < dataAND->size(); n++) {
            if(data->at(i).first == dataAND->at(n).first) {
              isGoodHit = true;
            }
          }
        }
        if(isGoodHit) {
          refTime = data->at(i).second;
          isRef = true;
          count += 1;
        }
      }
        
    }
    int a= 0;
    //std::cout << count << std::endl;
    if(isRef && count == 1) {
      for(unsigned int i = 0; i < data->size(); i++) {
       a= 0;
       bool isFind = false; int strip = 0;  double time = 10000000;
        // std::cout << std::setprecision(9) << data->at(i).first << " " << data->at(i).second << " " <<  refTime << std::endl; 
        int isGoodHit = false;
        for(unsigned int j = 0; j < data->size(); j++) {
          if((data->at(i).first == data->at(j).first) && std::abs(time) > std::abs(data->at(j).second - refTime)) {
            for(unsigned int n = 0; n < dataAND->size(); n++) {
              if(data->at(i).first == dataAND->at(n).first) {
                a+=1;
                isGoodHit = true;
                break;
              }
            }
            if(isGoodHit) {
              time = data->at(j).second - refTime;
              strip = data->at(j).first;
              if(strip == refStrip - 1) {
                //std::cout << std::setprecision(9) << name << " " << a << " " << strip << " " << data->at(j).second << " " << refTime << " " << time << std::endl;
                time1 = time;
              }
              if(strip == refStrip + 1) {
                //std::cout << "---" << std::endl;
                //std::cout << std::setprecision(9) << name << " " << a << " " << strip << " " << data->at(j).second << " " << refTime << " " << time << std::endl;
                time2 = time;
                if(time1 != 0 && time2 != 0) {
                  //std::cout << std::setprecision(9) << "delay: " << time1 - time2 << std::endl;
                  output->insert(std::make_pair(refStrip - 1, std::vector<double>{time1}));
                  output->insert(std::make_pair(refStrip + 1, std::vector<double>{time2}));
                  output->insert(std::make_pair(refStrip, std::vector<double>{time2-time1}));
                  //auto it = output->find(data->at(i).first);
                  //if(it == output->end()) {
                  //  output->insert(std::make_pair(strip, std::vector<double>{time}));
                  //}
                }
              }
              isFind = true;
            }
          }
        }
        //if(isFind) {
        //  auto it = output->find(data->at(i).first);
        //  if(it == output->end()) {
        //    output->insert(std::make_pair(strip, std::vector<double>{time}));
        //  }
        //  
        //  //output->find(data->at(i).first)->second.push_back(data->at(i).second - refTime);
        //  //std::cout << std::setprecision(9) << strip << " " << time << std::endl;
        //}
      }
    }
    return true;
}
bool LyPetiAnalysis::multiplicity(int* mult, std::string mod) 
{
  std::string name = _parser->last(mod);
  bool result = false;
  if(name == "HR" || name == "LR" || 
     name == "AND" || name == "OR")
    result = this->stripsMultiplicity(mult, mod);
  if(name == "CB" || name == "noiseCB")
    result = this->clusterBasicMultiplicity(mult, name);
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
  if(multiplicity == 0)
    return false;
  return result;
}
bool LyPetiAnalysis::clusterBasicMultiplicity(int* mult, std::string mod) 
{
  double multiplicity = 0;
  if(mod == "CB")
    multiplicity = _CB.size();
  else if(mod == "noiseCB")
    multiplicity = _noiseCB.size();
  *mult = multiplicity;
  if(multiplicity == 0)
    return false;
  return true;
}

bool LyPetiAnalysis::clusterSize(std::vector<int>* sizes, std::string mod)
{
  std::string name = _parser->last(mod);
  if(name == "CB") {
      for(auto& it: _CB)
    sizes->push_back(it.second.size);
    if(sizes->size() == 0)
      return false;
  }
  else if(name == "noiseCB") {
      for(auto& it: _noiseCB)
    sizes->push_back(it.second.size);
    if(sizes->size() == 0)
      return false;
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
      //this->filterXtalk(&_iHR, &_iLR); 
      this->dataFillHR(&_iHR, &_HR);
      this->dataFillLR(&_iLR, &_LR);
      this->dataFillAndOr(&_iHR, &_iLR, &_notHR, &_notLR, &_OR, &_AND);
      _isStripsFill = true;
      result = true;
    }
  }
  if(!_isNoiseFill) {
    if(param == "noise") {
//      this->filterXtalk(&_iNoiseHR, &_iNoiseLR); 
      this->dataFillHR(&_iNoiseHR, &_noiseHR);
      this->dataFillLR(&_iNoiseLR, &_noiseLR);
      std::vector<std::pair<double, double>> notHR; 
      std::vector<std::pair<double, double>> notLR; 
      this->dataFillAndOr( &_iNoiseHR, &_iNoiseLR, &notHR, &notLR, &_noiseOR, &_noiseAND);
      _isNoiseFill = true;
      result = true;
    }
  }
  return result;
}
bool LyPetiAnalysis::filterXtalk(std::vector<std::pair<int, int>>* iHR,
                                 std::vector<std::pair<int, int>>* iLR) 
{
  bool isTrue = false;
  int numStripHR = 0; double trigTimeHR = 0; double hitTimeHR = 0; 
  int numStripLR = 0; double trigTimeLR = 0; double hitTimeLR = 0; 
  for(unsigned int ih = 0; ih < iHR->size(); ih++) {
    numStripHR = 0; trigTimeHR = 0; hitTimeHR = 0; 
    numStripHR = _strips->at(iHR->at(ih).first).number;
    hitTimeHR = (double)_strips->at(iHR->at(ih).first).HR.at(iHR->at(ih).second)->time;
    trigTimeHR = (double)_strips->at(iHR->at(ih).first).trigs.at(0)->time;
    double timeHR = (hitTimeHR - trigTimeHR);
    for(unsigned int il = 0; il < iLR->size(); il++) {
      numStripLR = 0; trigTimeLR = 0; hitTimeLR = 0; 
      numStripLR = _strips->at(iLR->at(il).first).number; 
      hitTimeLR = (double)_strips->at(iLR->at(il).first).LR.at(iLR->at(il).second)->time;
      trigTimeLR = (double)_strips->at(iLR->at(il).first).trigs.at(0)->time;
      double timeLR = (hitTimeLR - trigTimeLR);
      if(numStripHR == numStripLR && ((timeHR - timeLR) > _begBCID) && ((timeHR - timeLR) < _endBCID)) { // rename xtalk
          if(timeHR > timeLR) {
            iLR->erase(iLR->begin()+il);
            isTrue = true;
          }
          else if(timeLR > timeHR) {
            iHR->erase(iHR->begin()+ih); ih = ih - 1;
            isTrue = true;
          }
          else {
            iLR->erase(iLR->begin()+il);
            iHR->erase(iHR->begin()+ih); ih = ih - 1;
            isTrue = true;
          }
        break;
      }
    }
  }
  if(isTrue)
    _badCluster+=1;

  return true;
}
bool LyPetiAnalysis::dataFillHR(std::vector<std::pair<int, int>>* iHR, 
                                std::vector<std::pair<double, double>>* output) 
{
  std::map<int, double>::iterator it;
  for(unsigned int i = 0; i < iHR->size(); i++) {
    it = _offsets.find(_strips->at(iHR->at(i).first).number);
    if(it == _offsets.end()) {
      it = _offsets.begin();
      _offsets.insert(it, std::pair<int,double>(_strips->at(iHR->at(i).first).number, _strips->at(iHR->at(i).first).timeOffSet));
    }
  }
  int numStrip = 0; double trigTime = 0; double hitTime = 0; 
  for(unsigned int ih = 0; ih < iHR->size(); ih++) {
    hitTime = 0;
    numStrip = 0;
    numStrip = _strips->at(iHR->at(ih).first).number;
    hitTime = (double)_strips->at(iHR->at(ih).first).HR.at(iHR->at(ih).second)->time;
    trigTime = (double)_strips->at(iHR->at(ih).first).trigs.at(0)->time;
    double time = (hitTime - trigTime);
    if(_isTimeOffsetHR) {
      if(_strips->at(iHR->at(ih).first).chamber == 1000)
        time = ((time - _strips->at(iHR->at(ih).first).timeOffSetHR)); //- ((0.04337*(numStrip-1000)-580.8) - (0.04337*_strips->at(iHR->at(ih).first).inverse-580.8)));
      if(_strips->at(iHR->at(ih).first).chamber == 0)
        time = ((time - _strips->at(iHR->at(ih).first).timeOffSetHR)); //- ((0.0395*(numStrip)-585) - (0.0395*_strips->at(iHR->at(ih).first).inverse-585)));
    }      
    output->push_back(std::pair<double, double>((double)numStrip, time));
  }
  return true;
}
bool LyPetiAnalysis::dataFillLR(std::vector<std::pair<int, int>>* iLR,
                                std::vector<std::pair<double, double>>* output) 
{
    std::map<int, double>::iterator it;
    for(unsigned int i = 0; i < iLR->size(); i++) {
      it = _offsets.find(_strips->at(iLR->at(i).first).number);
      if(it == _offsets.end()) {
        it = _offsets.begin();
      _offsets.insert(it, std::pair<int,double>(_strips->at(iLR->at(i).first).number, _strips->at(iLR->at(i).first).timeOffSet));
      //  std::cout << _strips->at(iLR->at(i).first).timeOffSet << std::endl;
      }
    }
  int numStrip = 0; double trigTime = 0; double hitTime = 0; 
  for(unsigned int il = 0; il < iLR->size(); il++) {
    numStrip = _strips->at(iLR->at(il).first).number; 
    hitTime = (double)_strips->at(iLR->at(il).first).LR.at(iLR->at(il).second)->time;
    trigTime = (double)_strips->at(iLR->at(il).first).trigs.at(0)->time;
    double time = (hitTime - trigTime);
    if(_isTimeOffsetLR) {
      if(_strips->at(iLR->at(il).first).chamber == 1000)
        time = ((time - _strips->at(iLR->at(il).first).timeOffSetLR)); // ((0.09689*(numStrip-1000)-589.8) - (0.09689*_strips->at(iLR->at(il).first).inverse-589.8))); 
      if(_strips->at(iLR->at(il).first).chamber == 0)
        time = ((time - _strips->at(iLR->at(il).first).timeOffSetLR)); // ((0.0004954*(numStrip)-586.4) - (0.0004954*_strips->at(iLR->at(il).first).inverse-586.4))); 
      //if(_strips->at(iLR->at(il).first).inverse != 0) // rename = mask
    }
      output->push_back(std::pair<double, double>((double)numStrip, time));
  }
  return true;
}
bool LyPetiAnalysis::dataFillAndOr(std::vector<std::pair<int, int>>* iHR,
                                   std::vector<std::pair<int, int>>* iLR,
                                   std::vector<std::pair<double, double>>* notHR, 
                                   std::vector<std::pair<double, double>>* notLR, 
                                   std::vector<std::pair<double, double>>* OR, 
                                   std::vector<std::pair<double, double>>* AND) 
{
    //
    std::vector<std::pair<double, double>> HR;
    std::vector<std::pair<double, double>> LR;
    this->dataFillHR(iHR, &HR);
    this->dataFillLR(iLR, &LR);
    this->sortAndResize(&HR, &LR);
    for(unsigned int i = 0; i < HR.size(); i++) {
      for(unsigned int j = 0; j < LR.size(); j++) {
        if(HR.at(i).first == LR.at(j).first) {
          for(unsigned int l = 0; l < LR.size(); l++) {
            if((LR.at(j).first == LR.at(l).first) && (std::abs(LR.at(j).second - HR.at(i).second) > std::abs(LR.at(l).second - HR.at(i).second)))
              std::swap(LR[j],  LR[l]);
          }
          double time = HR.at(i).second - LR.at(j).second;
          if(_isTimeOffset) {
            time = time + _offsets.find(HR.at(i).first)->second;
 //       std::cout << _offsets.find(HR.at(i).first)->second << std::endl;
          }
          AND->push_back(std::pair<double, double>(HR.at(i).first, time));

          //            std::cout << "time: " << HR.at(i).second - LR.at(j).second << "|" << HR.at(i).second << "|" <<  LR.at(j).second << "|" << (int)HR.at(i).first << " ";
          OR->push_back(std::pair<double, double>(HR.at(i).first, HR.at(i).second));
          LR.erase(LR.begin()+j);
          break;
	  		}
        else if(j == LR.size()-1 && HR.at(i).first != _limit) {
          OR->push_back(std::pair<double, double>(HR.at(i).first, HR.at(i).second));
          _notHR.push_back(std::pair<double, double>(HR.at(i).first, HR.at(i).second));
        }
      }
    }
    for(unsigned int j = 0; j < LR.size(); j++) {
      if(LR.at(j).first != _limit) {
        OR->push_back(std::pair<double, double>(LR.at(j).first, LR.at(j).second));
        _notLR.push_back(std::pair<double, double>(LR.at(j).first, LR.at(j).second));
      }
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
      else if(_dataCB == 5)
        this->clusterBasic(_HR, _LR, &_CB, false);
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
        this->clusterBasic(_noiseOR, &_noiseCB);
      else if(_dataCB == 5)
        this->clusterBasic(_noiseHR, _noiseLR, &_noiseCB, true);
      _isNoiseClusterBasic = true;
      result = true;
    }
  }
  return result;
}
bool LyPetiAnalysis::clusterBasic(std::vector<std::pair<double, double>> dataHR, std::vector<std::pair<double, double>> dataLR,  
                                  std::map<int, lyCB::data>* output, bool isOR) 
{
  //std::cout <<  "..." << std::endl;
  std::map<int, lyCB::data> HR;  
  std::map<int, lyCB::data> LR;  
  this->clusterBasic(dataHR, &HR);
  this->clusterBasic(dataLR, &LR);
  //std::cout << "........." << std::endl;
  //for(auto& it: HR) {
  //  std::cout << std::setprecision(5) << "HR number: " << it.first << std::endl;
  //  for(unsigned int i = 0; i < it.second.hits.size(); i++) {
  //    std::cout << std::setprecision(5) << "strip:" << it.second.hits.at(i).first << " time: " << it.second.hits.at(i).second << std::endl;
  //  }
  //}
  //for(auto& it: LR) {
  //  std::cout << std::setprecision(5) << " LR number: " << it.first << std::endl;
  //  for(unsigned int i = 0; i < it.second.hits.size(); i++) {
  //    std::cout << std::setprecision(5) << "strip:" << it.second.hits.at(i).first << " time: " << it.second.hits.at(i).second << std::endl;
  //  }
  //}
  
  // нахожу наибольште совпадения по целым стрипам для пары кластеров
  std::map<int, lyCB::data>::iterator it;
  std::map<int, lyCB::data> tmpHR = HR;  
  std::map<int, lyCB::data> tmpLR = LR;  
  std::pair<int, int> couple;
  std::vector<std::pair<int, int>> coupls; 
  for(unsigned int c = 0; c < tmpHR.size(); c++) {
    int prevCounter = 0;
    for(auto& hr: tmpHR) {
      for(auto& lr: tmpLR) {
        int counter = 0; 
        for(unsigned int i = 0; i < hr.second.hits.size(); i++) {
          for(unsigned int j = 0; j < lr.second.hits.size(); j++) {
            if(hr.second.hits.at(i).first == lr.second.hits.at(j).first) {
              counter += 1;
              break;
            }
          }    
        }
        if(prevCounter < counter) {
          couple = std::pair<int, int>(hr.first, lr.first);
          prevCounter = counter;
        }
      }
    }
    if(prevCounter > 0) {
      coupls.push_back(couple);
      it = tmpHR.find(coupls.back().first);
      tmpHR.erase(it); c = c - 1;
      it = tmpLR.find(coupls.back().second);
      tmpLR.erase(it);
    }
  }
  //for(unsigned int i = 0; i < coupls.size(); i++)
  //  std::cout << "Couple.. HR: " << coupls.at(i).first << " LR: " << coupls.at(i).second << std::endl;  

  // Заполнение AND
  std::map<int, lyCB::data>::iterator hr;
  std::map<int, lyCB::data>::iterator lr;
  std::vector<std::pair<double, double>> strips;
  for(unsigned int i = 0; i < coupls.size(); i++) {
    lyCB::data cluster;
    hr = HR.find(coupls.at(i).first);
    lr = LR.find(coupls.at(i).second);
    if(hr->second.hits.size() > lr->second.hits.size())
    cluster.size = hr->second.hits.size();
    else
      cluster.size = lr->second.hits.size();
//

    for(unsigned int i = 0; i < hr->second.hits.size(); i++) {
      for(unsigned int j = 0; j < lr->second.hits.size(); j++) {
        if(hr->second.hits.at(i).first == lr->second.hits.at(j).first) {
          cluster.hits.push_back(std::pair<int, double>(hr->second.hits.at(i).first, hr->second.hits.at(i).second - lr->second.hits.at(j).second));
          strips.push_back(std::pair<int, double>(hr->second.hits.at(i).first, hr->second.hits.at(i).second - lr->second.hits.at(j).second));
          hr->second.hits.erase(hr->second.hits.begin() + i); i = i - 1;
          lr->second.hits.erase(lr->second.hits.begin() + j);
          break;
        }
      }
    }
    cluster.centralStrip = (int)strips.at(this->centralStrip(&strips)).first;
    cluster.centralTime = strips.at(this->centralStrip(&strips)).second;
    cluster.middleStrip = this->middleStripCB(&strips);    
    cluster.middleTime = this->middleTimeCB(&strips);
    //if((cluster.middleTime > -0.2 && cluster.middleTime < 0.2) || (cluster.centralTime > -0.2 && cluster.centralTime < 0.2)) {
    //  strips.clear();
    //  continue;
    //}
    if(_isTimeOffset) {
      cluster.middleTime =  this->middleTimeCB(&strips) + _offsets.find(this->middleStripCB(&strips))->second;
      cluster.centralTime = strips.at(this->centralStrip(&strips)).second + _offsets.find(this->centralStrip(&strips))->second;
    }
//    cluster.size = strips.size();
    strips.clear();
    output->insert(std::make_pair(i, cluster));
  }
  // Заполнение если OR
  if(isOR) {
     int iCluster = coupls.size();
     for(auto& it: tmpHR) {
        lyCB::data cluster;
        for(unsigned int i = 0; i < it.second.hits.size(); i++) {
          cluster.hits.push_back(std::pair<int, double>(it.second.hits.at(i).first, -100));
          strips.push_back(std::pair<int, double>(it.second.hits.at(i).first, -100));
        }
        cluster.centralStrip = (int)strips.at(this->centralStrip(&strips)).first;
        cluster.centralTime = -100;
        cluster.middleStrip = this->middleStripCB(&strips);    
        cluster.middleTime = -100;
        cluster.size = strips.size();
        strips.clear();
        output->insert(std::make_pair(iCluster, cluster));
        iCluster += 1;
     }
     for(auto& it: tmpLR) {
        lyCB::data cluster;
        for(unsigned int i = 0; i < it.second.hits.size(); i++) {
          cluster.hits.push_back(std::pair<int, double>(it.second.hits.at(i).first, -100));
          strips.push_back(std::pair<int, double>(it.second.hits.at(i).first, -100));
        }
        cluster.centralStrip = (int)strips.at(this->centralStrip(&strips)).first;
        cluster.centralTime = -100;
        cluster.middleStrip = this->middleStripCB(&strips);    
        cluster.middleTime = -100;
        cluster.size = strips.size();
        strips.clear();
        output->insert(std::make_pair(iCluster, cluster));
        iCluster += 1;
     }
  }
  //for(auto& it: *output) {
  //  std::cout << std::setprecision(5) << " Cluster number: " << it.first << std::endl;
  //  for(unsigned int i = 0; i < it.second.hits.size(); i++) {
  //    std::cout << std::setprecision(5) << "strip:" << it.second.hits.at(i).first << " time: " << it.second.hits.at(i).second << std::endl;
  //  }
  //}
  return true;
}
bool LyPetiAnalysis::clusterBasic(std::vector<std::pair<double, double>> data, std::map<int, lyCB::data>* output) 
{
    bool isDead = false;
    std::vector<int> deadStrips = {36, 1001, 1006, 1011, 1013, 1017, 1037, 1043}; // добавить в параметры
    double timeThr = _timeThrCB; 
    std::sort(data.begin(), data.end(), [](auto &left, auto &right) {
      return std::abs(left.first) < std::abs(right.first);
	  });

    //if(data.size() > 0)
    //std::cout << "data: ";
    //  for(unsigned int i = 0; i < data.size(); i++)
    //    std::cout << std::setprecision(5) << (int)data.at(i).first << "|" << data.at(i).second << " ";
    //if(data.size() > 0)
    //  std::cout << std::endl;
   
    if(data.size() >= 8) {
      _badCluster+=1;
    }
      std::vector<std::pair<double, double>> strips;
      int iCluster = 0;
      while(data.size() > 0) {
        std::vector<int> indexes;
        lyCB::data cluster;
        strips.push_back(std::pair<double, double>(data.at(0).first, data.at(0).second));
        data.erase(data.begin());
        for(unsigned int i = 0; i < data.size(); i++) {
          
          // avoid dead strips
          isDead = false;
          for(unsigned int ds = 0; ds < deadStrips.size(); ds++) {
            if((strips.back().first + 1) == deadStrips.at(ds))
              isDead = true;
          }
          if(isDead && data.at(i).first != (strips.back().first + 1) && i != data.size()-1) {
            int next = 0;
            while(data.size() > i + next) {
              if(data.at(i + next).first == (strips.back().first + 2)) {
                strips.push_back(std::pair<double, double>((strips.back().first + 1), strips.back().second));
                break;
              }
              next = next + 1;
            }
          }
          // clustoring
          if(data.at(i).first == (strips.back().first + 1)) {
            bool isFind = false; int prev = 0; int next = 0; double dTime = 1000000; int plusIndex = 0;
            while(data.size() > i + next) {
              if(data.at(i+next).first == (strips.back().first + 1)) { 
                prev = next;
                std::vector<double> times = this->clustersTimesCB(&strips); 
                for(unsigned int j = 0; j < times.size(); j++) {
                  if(std::abs(data.at(i+next).second - times.at(j)) <= timeThr) {
                    if(dTime > std::abs(data.at(i+next).second -  times.at(j))) {
                      dTime = std::abs(data.at(i+next).second -  times.at(j));
                      isFind = true;
                    }
                  }
                }
                if(!isFind) {
                  for(unsigned int jj = 0; jj < strips.size(); jj++) {
                    if(data.at(i+next).first == strips.at(jj).first) {
                      isFind = false;
                      break;
                    }
                  }
                  if(isFind)
                    plusIndex = prev;
                  // один и тот же стрип
                }
              }
              else 
                break;
              next = next + 1;
            }
            if(isFind) {
              strips.push_back(std::pair<double, double>(data.at(i+plusIndex).first, data.at(i + plusIndex).second));
              indexes.push_back(i + plusIndex);
            }
          }
        }
        cluster.size = strips.size();
       
       // if(strips.size() >= 11) {
       // // clear
       // for(unsigned int i = 0; i < indexes.size(); i++)
       //   data.erase(data.begin() + indexes.at(i) - i);
       // strips.clear();
       // indexes.clear();
       // continue;
       //}
        
        if(cluster.size > 0) {
          //if(cluster.size > 6)
           //std::cout  <<"size:" << strips.size() << " cluster: ";
           //for(unsigned int i = 0; i < strips.size(); i++)
           //  std::cout  << std::setprecision(5) << (int)strips.at(i).first << "|" << strips.at(i).second << " ";
           //std::cout << std::endl;
          cluster.centralStrip = (int)strips.at(this->centralStrip(&strips)).first;
          cluster.centralTime = strips.at(this->centralStrip(&strips)).second;
          cluster.middleStrip = this->middleStripCB(&strips);    
          cluster.middleTime = this->middleTimeCB(&strips);
          for(unsigned int sh = 0; sh < strips.size(); sh++)
            cluster.hits.push_back(std::pair<int, double>((int)strips.at(sh).first, strips.at(sh).second));
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
   //else {
   //  _numEffTrigers -=1;
   //  _badCluster+=1;
   //   }
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
double LyPetiAnalysis::middleTimeCB(std::vector<std::pair<double, double>>* strips) 
{
  double sum = 0;
  double time = 1000000;
    //for(unsigned int i = 0; i < strips->size(); i++) {
    //  if(std::abs(time) > std::abs(strips->at(i).second))
    //    time = strips->at(i).second;
    //}
  for(unsigned int i = 0; i < strips->size(); i++) {
    sum += strips->at(i).second;
  }
  time = sum/strips->size();
  return time;
}
std::vector<double> LyPetiAnalysis::clustersTimesCB(std::vector<std::pair<double, double>>* strips) 
{
  double sum = 0;
  double min = 1000000;
  double max = 0;
  std::vector<double> time;
    //for(unsigned int i = 0; i < strips->size(); i++)
    //  sum += strips->at(i).second;
    for(unsigned int i = 0; i < strips->size(); i++) {
      if(std::abs(min) > std::abs(strips->at(i).second))
        min = strips->at(i).second;
      if(std::abs(max) < std::abs(strips->at(i).second))
        max = strips->at(i).second;
    }
  time.push_back(min);
  time.push_back(max);
  return time;
}
double LyPetiAnalysis::middleStripCB(std::vector<std::pair<double, double>>* strips) 
{
  double sum = 0; 
  for(unsigned int i = 0; i < strips->size(); i++) {
    sum += strips->at(i).first;
  }
  double strip = sum/strips->size();
  return strip;
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
