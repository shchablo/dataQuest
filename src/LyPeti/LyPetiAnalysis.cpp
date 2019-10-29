#include "LyPetiAnalysis.hpp"
#include <sstream> 
#include <iomanip>
/* Constructor, destructor. */ 
LyPetiAnalysis::LyPetiAnalysis() 
{
  _preTrigTime = 0;
  _HRfilter = 0; _LRfilter = 0;
  _hHR = new TH1F(Form("hHR_%li", (long int)_hHR), "HR", 300,  1, -1);
  _hLR = new TH1F(Form("hLR_%li", (long int)_hLR), "LR", 300,  1, -1);
  
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
  _randShift = 10;
  
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
  _dataCB = 1;
	_timeThrCB = 1000; 
  
  _numEffEventsCB = 0;

  // noise analysis
  _numNoiseHitsHR = 0; _numNoiseHitsLR = 0; _numNoiseHitsOR = 0; _numNoiseHitsAND = 0; _numNoiseHitsCB = 0;
  _noiseUnit = 1;
  _chamberArea = 1;
  _stripArea = 1;
  
  _newWindow = 0;

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
  //return "";
}
std::string LyPetiAnalysis::modParamsStr(std::string mod) 
{
  std::string params = "";
  if(_parser->last(mod) == "CB") {
    std::stringstream ss;
    std::cout.precision(1); 
    ss << std::fixed << std::setprecision(1) << "TimeThr" << _timeThrCB << "ns";
    ss << std::fixed << std::setprecision(1) << "Data" << _dataCB << "mod";
    params += ss.str();
  }
  if(_parser->last(mod) == "LR") {
    std::stringstream ss;
    std::cout.precision(1); 
    ss << std::fixed << std::setprecision(1) << "[" << _begWindowLR << " " << _endWindowLR << "]ns";
    params += ss.str();
  }
  if(_parser->last(mod) == "HR") {
    std::stringstream ss;
    std::cout.precision(1); 
    ss << std::fixed << std::setprecision(1) << "[" << _begWindowHR << " " << _endWindowHR << "]ns";
    params += ss.str();
  }
  return params;
}

bool LyPetiAnalysis::skipEvent(bool is) 
{
  _isSkipEvent = is;
  return _isSkipEvent;
}
bool LyPetiAnalysis::skipEvent() 
{
  //if(_isSkipEvent)
    //this->clearEvent();
  return _isSkipEvent;
}

/* Take data. */ 
std::vector<std::pair<double, double>> LyPetiAnalysis::data(std::string mod)
{
  std::string name = _parser->last(mod);
  std::string param = _parser->first(mod);
  std::vector<std::pair<double, double>> data;
  if(name == "HR" || name == "LR" || 
     name == "AND" || name == "OR" || name == "fLR" || name == "fHR" || name == "fAND")
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
  else if(name == "fHR") {
    result = true;
    *data = _HRf;
  }
  else if(name == "fLR") {
    result = true;
    *data = _LRf;
  }
  else if(name == "fAND") {
    result = true;
    *data = _ANDf;
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
    if(param == "first") {
    result = true;
    for(auto& it: _CB)
      data->push_back(std::pair<double, double>(it.second.firstStrip, it.second.firstTime));
  }
   return result;
}
std::map<int, lyCB::data> LyPetiAnalysis::data()
{
  return _CB;
}
std::vector<std::pair<double, std::pair<double, double>>> LyPetiAnalysis::pairData(std::string mod)
{
  std::string name = _parser->last(mod);
  std::string param = _parser->first(mod);
  std::vector<std::pair<double, std::pair<double, double>>> data;
  if(name == "AND")
    return _pairAND;
  return data;
}
//------------------------------------------------------------------------------

/* Clear data (per RUN and per Event). */ 
bool LyPetiAnalysis::clear(std::string type)
{
  std::string name = _parser->last(type);
  std::string param = _parser->first(type);
  if(name == "ANALYSIS EVENT") {
    this->clearEvent();
  }
  if(name == "CONFIG EVENT") {
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
  if(name == "ANALYSIS RUN") {
    this->clearEvent();
    this->clearRun();

  }
  if(name == "CONFIG RUN") {
    this->clearEvent();
    this->clearRun();
    if(_newWindow == 1) this->newWindows();
  }
  return true;
}
bool LyPetiAnalysis::clearEvent()
{
    if(_isSkipEvent && _isFoundTriger)
      _numSkipedTrigers += 1;
    _isSkipEvent = false;
    
    _preTrigTime = 0;
    _HRfilter = 0; _LRfilter = 0;
    
    
    _numNoiseHitsHR += _HR.size(); 
    _numNoiseHitsLR += _LR.size(); 
    _numNoiseHitsAND += _AND.size();  
    _numNoiseHitsOR += _OR.size();     
    _numNoiseHitsCB += _CB.size();
    
    _numberOfEvents+=1;
    _iHR.clear();_iNoiseHR.clear();
    _iLR.clear();_iNoiseLR.clear();
    
    _isStripsFill = false; _isNoiseFill = false;
    
    _HRf.clear(); _LRf.clear(); _ANDf.clear();
    _HR.clear(); _noiseHR.clear();
    _LR.clear(); _noiseLR.clear();
    _OR.clear(); _noiseOR.clear();
    _AND.clear(); _noiseAND.clear();
    _notHR.clear(); _notLR.clear();
    _pairAND.clear();    

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
   
   _numEffTrigers = 0;
   _numSkipedTrigers = 0;
    return true;
}
bool LyPetiAnalysis::newWindows()
{
   double pMean  = _hHR->GetMean();
   double pRMS  = _hHR->GetRMS();
   TF1* g1 = new TF1("g", "gaus",  pMean - 6*pRMS, pMean + 6*pRMS);
   _hHR->Fit("g", "QS"); 
   // std::cout << " HR:" << std::setprecision(4) << 3*g1->GetParameter(2) << std::endl;
   _begWindowHR = g1->GetParameter(1) - _rangeHR/2;
   _endWindowHR = g1->GetParameter(1) + _rangeHR/2;
   delete g1;
   
   pMean  = _hLR->GetMean();
   pRMS  = _hLR->GetRMS();
   TF1* g2 = new TF1("g2", "gaus",  pMean - 6*pRMS, pMean + 6*pRMS);
   _hLR->Fit("g2", "QS"); 
   // std::cout << " LR:" << std::setprecision(4) << 3*g2->GetParameter(2) << std::endl;
   _begWindowLR = g2->GetParameter(1) - _rangeLR/2;
   _endWindowLR = g2->GetParameter(1) + _rangeLR/2;
   delete g2;
   
   delete _hHR; _hHR = new TH1F(Form("hHR_%li", (long int)_hHR), "HR", 300,  1, -1);
   delete _hLR; _hLR = new TH1F(Form("hLR_%li", (long int)_hLR), "LR", 300,  1, -1);

  _randShift = rand() % 100 + 10;
  
  return true;
}
//------------------------------------------------------------------------------

bool LyPetiAnalysis::configure()
{
  std::stringstream ss;
  std::cout.precision(1); 
 

   delete _hHR; _hHR = new TH1F(Form("hHR_%li", (long int)_hHR), "HR", 300,  1, -1);
   delete _hLR; _hLR = new TH1F(Form("hLR_%li", (long int)_hLR), "LR", 300,  1, -1);

	this->findParam("all&is", &_isAll);  
  
  double numBoards = 0;
  bool isNumBoard = this->findParam("numBoard", &numBoards);
  if(isNumBoard)
    _numBoards = (int)numBoards;
  else 
    _numBoards = 0;
  
  this->findParam("minMaxTime&is", &_isMinMaxTime);  
  
  bool isTrigers = this->findParam("triger&is", &_isTrigers);  
  _filters += "trig";
  if(isTrigers) {
    double thrTrigers = 0;
    bool isTrigers = this->findParam("triger&thr", &thrTrigers);
    if(!isTrigers && thrTrigers >= 0)
      return false;
    _thrTrigers = (int)thrTrigers;
  }
  
  bool isWindow = this->findParam("window&is", &_isWindow);  
  if(isWindow) {
    bool isBegWindow = this->findParam("windowLR&beg", &_begWindowLR_def);  
    bool isEndWindow = this->findParam("windowLR&end", &_endWindowLR_def);
    this->findParam("windowLR&range", &_rangeLR);
    _begWindowLR = _begWindowLR_def;
    _endWindowLR = _endWindowLR_def;
    if(!isBegWindow || !isEndWindow)
      return false;  
  }  
  if(isWindow) {
    bool isBegWindow = this->findParam("windowHR&beg", &_begWindowHR_def);  
    bool isEndWindow = this->findParam("windowHR&end", &_endWindowHR_def);
    this->findParam("windowHR&range", &_rangeHR);
    _begWindowHR = _begWindowHR_def;
    _endWindowHR = _endWindowHR_def;
    if(!isBegWindow || !isEndWindow)
      return false;  
  }  
  bool isNoise = this->findParam("noise&is", &_isNoise);  
  if(isNoise) {
    bool isBegNoise = this->findParam("noise&beg", &_begNoise);  
    bool isEndNoise = this->findParam("noise&end", &_endNoise);
    if(!isBegNoise || !isEndNoise)
      return false;
    else
     _filters += "Noise";
  }  
  
  this->findParam("deadTime&is", &_isDeadTime);  
  if(_isDeadTime) {
    double deadTimeWindow = 0;
    bool isDeadTimeWindow = this->findParam("deadTime&window", &deadTimeWindow);
    if(isDeadTimeWindow) {
      _deadTime = deadTimeWindow;
      ss << std::fixed << std::setprecision(0) << "DeadTime" << deadTimeWindow << "ns";
      _filters += ss.str();
    }
  } 
  
  double timeOffSetHR = 0;
  bool isTimeOffsetHR = this->findParam("offsetHR&isUse", &timeOffSetHR);
  if(isTimeOffsetHR && timeOffSetHR == 1) {
    _isTimeOffsetHR = true;
  }
  
  double timeOffSetLR = 0;
  bool isTimeOffsetLR = this->findParam("offsetLR&isUse", &timeOffSetLR);
  if(isTimeOffsetLR && timeOffSetLR == 1) {
    _isTimeOffsetLR = true;
  }
  if(_isTimeOffsetLR && _isTimeOffsetHR)
     _filters += "OffSet";

  bool isBCID = this->findParam("BCID&is", &_isBCID);  
  if(isBCID) {
    bool isBegBCID = this->findParam("BCID&beg", &_begBCID);  
    bool isEndBCID = this->findParam("BCID&end", &_endBCID);
    if(!isBegBCID || !isEndBCID)
      return false;
    else
     _filters += "BCID";
  } 

  bool isTimeThrCB = this->findParam("CB&timeThr", &_timeThrCB);
  if(!isTimeThrCB)
    _timeThrCB = 100;
  
  bool isTimeThrHR = this->findParam("CB&timeThrHR", &_timeThrHR);
  if(!isTimeThrHR)
    _timeThrHR = 100;
  
  bool isOR = this->findParam("CB&outputOR", &_isOR);
  if(!isOR)
    _isOR = 0;
  
  bool isThrDeltaMin = this->findParam("CB&thrDeltaMin", &_thrDeltaMin);
  if(!isThrDeltaMin)
    _thrDeltaMin = -50;
  
  bool isThrDeltaMax = this->findParam("CB&thrDeltaMax", &_thrDeltaMax);
  if(!isThrDeltaMax)
    _thrDeltaMax = 50;
  
  bool isTimeThrLR = this->findParam("CB&timeThrLR", &_timeThrLR);
  if(!isTimeThrLR)
    _timeThrLR = 100;
  
  bool isTimeThrA = this->findParam("CB&timeThrA", &_timeThrA);
  if(!isTimeThrA)
    _timeThrA = 100;

  double dataCB = 1;
  bool isDataCB = this->findParam("CB&iC", &dataCB);
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
  
  double stripArea = 1;
  bool isStripArea = this->findParam("stripArea", &stripArea);
  if(isStripArea)
    _stripArea = stripArea;
  else 
    _stripArea = 1;
  
  double newWindow = 0;
  bool isNewWindow = this->findParam("newWindow", &newWindow);
  if(isNewWindow) _newWindow = newWindow;
  else _newWindow = 0;
  
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
  if(_isBCID)
    results.push_back(this->BCID(js));

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
  if(_strips->at(js).trigs.size() <= (unsigned int)_thrTrigers && _strips->at(js).trigs.size() > 0) {
    result = true;
  }
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
  
  double beamWindow = std::abs(endWindow-begWindow);

  if((param == "noise") && (((begWindow - _randShift*beamWindow) <= dTime && (endWindow - _randShift*beamWindow) >= dTime))) {
      return true;
  }
  if((param == "data") && begWindow <= dTime && endWindow >= dTime) {  
      return true;
  }
  //if((param == "deadTime") && ((begWindow - _deadTime <= dTime && begWindow > dTime) )) {
  //  if(_isDeadTime)
  //    _isSkipEvent = true;
  //  return false;
  //}
  return false;
}
bool LyPetiAnalysis::BCID(unsigned int js) 
{
  bool result = false;
  bool isTriger =  this->triger(js);
  if(isTriger && _strips->at(js).trigs[0]->BCID >= _begBCID && _strips->at(js).trigs[0]->BCID <= _endBCID)
    result = true;
  return result;
}
//------------------------------------------------------------------------------

/* Analysis and algorithms */ 
bool LyPetiAnalysis::algos(std::string mod)
{
  std::string name = _parser->last(mod);
  if(name == "HR" || name == "LR" || 
     name == "AND" || name == "OR" || name == "fHR" || name == "fLR") {
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
  
  double events = 0; double noise = 0; bool isN = false;
 
  double trigers = _numEffTrigers -_numSkipedTrigers;
  double noiseTrigers = _numEffTrigers - _numSkipedTrigers;
  
  if(name == "HR") { events = _numEffEventsHR; noise = _numEffNoiseEventsHR; } 
  else if(name == "LR") { events = _numEffEventsLR; noise = _numEffNoiseEventsLR; } 
  else if(name == "OR") { events = _numEffEventsOR; noise = _numEffNoiseEventsOR; } 
  else if(name == "AND") { events = _numEffEventsAND; noise = _numEffNoiseEventsAND; } 
  else if(name == "CB") { events = _numEffEventsCB; noise = _numEffNoiseEventsCB; } 
  else if(name == "nHR") { events = _numEffNoiseEventsHR; isN = true; } 
  else if(name == "nLR") { events = _numEffNoiseEventsLR; isN = true; } 
  else if(name == "nAND") { events = _numEffNoiseEventsAND; isN = true; } 
  else if(name == "nOR") { events = _numEffNoiseEventsOR; isN = true; } 
  else if(name == "nCB") { events = _numEffNoiseEventsCB; isN = true; } 
  else return false;
 
  if(trigers == 0) return false;
  //----

  double efficiency = (events)/trigers; 
  double eEfficiency = std::sqrt((efficiency*(1-efficiency)))/std::sqrt(trigers);
  
  double effNoise = (noise)/noiseTrigers;
  double eEffNoise = std::sqrt((effNoise*(1-effNoise)))/std::sqrt(trigers);
  
  *eff = efficiency; *eEff = eEfficiency;
  if(_isNoise && !isN) {
    *eff = (efficiency - effNoise)/(1-effNoise);
    *eEff = eEfficiency + eEffNoise;
  }
   // std::cout << std::setprecision(5) << name << " " << events << " " << trigers << " " << *eff << " " << *eEff << std::endl; 
  if(trigers < 0 || events < 0) { *eff = 0; *eEff = 0; }
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
  if(name == "nHR" ||  name == "nLR" || name == "nAND" || name == "nOR" || name == "nCB")
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
          //noiseLR.erase(noiseLR.begin()+j);
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
          //LR.erase(LR.begin()+j);
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

bool LyPetiAnalysis::noise(double* n, double* eN, int strip, int entrie, std::string mod)
{
  std::string name = _parser->last(mod);
  std::string param = _parser->first(mod);
  
  double noise = 0; double begWindow = 0; double endWindow = 0;
  
  double trigers = _numEffTrigers - _numSkipedTrigers;
  if(trigers <= 0) return false;
  
  if(name == "HR") { noise = _numNoiseHitsHR; 
                     begWindow = _begWindowHR; endWindow = _endWindowHR; }
  else if(name == "LR") { noise = _numNoiseHitsLR; 
                          begWindow = _begWindowLR; endWindow = _endWindowLR; }
  else if(name == "OR") { noise = _numNoiseHitsOR;
                          begWindow = (_begWindowLR+_begWindowHR)/2;
                          endWindow = (_endWindowLR+_endWindowHR)/2; }
  else if(name == "AND") { noise = _numNoiseHitsAND;
                           begWindow = (_begWindowLR+_begWindowHR)/2;
                           endWindow = (_endWindowLR+_endWindowHR)/2; }
  else if(name == "CB") { noise = _numNoiseHitsCB;
                          begWindow = (_begWindowLR+_begWindowHR)/2;
                          endWindow = (_endWindowLR+_endWindowHR)/2; }
  else return false;
  
  if(strip != -1) noise = entrie;

  if(trigers != 0 && strip == -1)
    *n = noise/(((std::abs((endWindow-begWindow)))*trigers)*_noiseUnit*_chamberArea);
  else if(trigers != 0 && strip != -1) 
    *n = noise/(((std::abs((endWindow-begWindow)))*trigers)*_noiseUnit*_stripArea);
  else *n = 0; *eN = 0; 
  
  // std::cout << std::setprecision(5) << name << " RESULT:" << *n << " N:" << noise << " W: " << std::abs(_endNoise-_begNoise) << " U:" << _noiseUnit <<" T: " << trigers << " Surf:" << _chamberArea << std::endl;  
  return true;
}

bool LyPetiAnalysis::getNumSkipedTrigers(double* num)
{
  *num = (double)_numSkipedTrigers; return true;
}
bool LyPetiAnalysis::getNumTrigers(double* num)
{
  *num = (double)_numEffTrigers; return true;
}

bool LyPetiAnalysis::multiplicity(int* mult, std::string mod) 
{
  std::string name = _parser->last(mod);
  bool result = false;
  if(name == "HR" || name == "LR" || 
     name == "AND" || name == "OR")
    result = this->stripsMultiplicity(mult, mod);
  if(name == "CB" || name == "nCB")
    result = this->clusterBasicMultiplicity(mult, name);
  return result;
}

bool LyPetiAnalysis::stripsMultiplicity(int* mult, std::string mod) 
{
  std::string name = _parser->last(mod);
  bool result = false;
  double multiplicity = 0;
  if(name == "HR") { multiplicity = _iHR.size(); result = true; } 
  else if(name == "LR") { multiplicity = _iLR.size(); result = true; } 
  else if(name == "OR") {
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
  } 
  else if(name == "AND") {
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
  } else result = false;
   
  *mult = multiplicity; if(multiplicity == 0) return false;
  return result;
}
bool LyPetiAnalysis::clusterBasicMultiplicity(int* mult, std::string mod) 
{
  double multiplicity = 0;
  
  if(mod == "CB") multiplicity = _CB.size();
  else if(mod == "nCB") multiplicity = _noiseCB.size();
  
  *mult = multiplicity;
  
  if(multiplicity == 0) return false;
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
  else if(name == "nCB") {
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
  else result = false;
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
bool LyPetiAnalysis::fillTrigers(std::vector<double> triggers) 
{
 	bool result = false; 
	if(_numBoards == triggers.size()) {
    if(std::abs(_preTrigTime - triggers.at(0)) > 50) {
      _numEffTrigers += 1;
	    result = true;
      _isFoundTriger = true;
    }
	}
  else
    _isFoundTriger = false;
  
  if(result == true)  _preTrigTime = triggers.at(0);
  return result; 
}
bool LyPetiAnalysis::fillTrigers(unsigned int numBoards) 
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


bool LyPetiAnalysis::filter(std::vector<std::pair<int, int>>* iHR,
                                 std::vector<std::pair<int, int>>* iLR) 
{
  bool isTrue = false;
  for(unsigned int ih = 0; ih < iHR->size(); ih++) {
    for(unsigned int ihh = 0; ihh < iHR->size(); ihh++) {
      if(ih != ihh && _strips->at(iHR->at(ih).first).number == _strips->at(iHR->at(ihh).first).number &&
        (double)_strips->at(iHR->at(ih).first).HR.at(iHR->at(ih).second)->time == 
        (double)_strips->at(iHR->at(ihh).first).HR.at(iHR->at(ihh).second)->time) {
        iHR->erase(iHR->begin()+ih); ih = ih-1;
        _HRfilter += 1;
  			isTrue = true;
        break;
      }
    }
  }

  for(unsigned int il = 0; il < iLR->size(); il++) {
    for(unsigned int ill = 0; ill < iLR->size(); ill++) {
      if(il != ill &&_strips->at(iLR->at(il).first).number == _strips->at(iLR->at(ill).first).number &&
        (double)_strips->at(iLR->at(il).first).LR.at(iLR->at(il).second)->time == 
        (double)_strips->at(iLR->at(ill).first).LR.at(iLR->at(ill).second)->time) {
        iLR->erase(iLR->begin()+il); il = il-1;
        _LRfilter += 1;
  			isTrue = true;
        break;
      }
    }
  }
  return isTrue;
}
bool LyPetiAnalysis::recoveryCHs(std::vector<int> deadCHs,
                                std::vector<std::pair<double, double>>* donor, 
                                std::vector<std::pair<double, double>>* recipient) 
{
  for(unsigned int i = 0; i < donor->size(); i++) {
    bool isDead = false;
    for(unsigned int d = 0; d < deadCHs.size(); d++) {
      if(donor->at(i).first == deadCHs.at(d)) {
        isDead = true; break; 
      }
    }
    if(isDead) {
      bool isTransplantation = false;
      for(unsigned int j = 0; j < recipient->size(); j++) {
        if(recipient->at(j).first + 1 == donor->at(i).first || recipient->at(j).first -1 == donor->at(i).first) {
          recipient->push_back(std::pair<double, double>(donor->at(i).first, recipient->at(j).second));
          isTransplantation = true;
          break;
        }
      }
      if(!isTransplantation) {
          recipient->push_back(std::pair<double, double>(donor->at(i).first, 0));
      }
    }
  }
  return true;
}

bool LyPetiAnalysis::choiceDataFill(std::string param) 
{
  bool result = false;
  if(!_isStripsFill) {
    if(param == "data") {
      this->filter(&_iHR, &_iLR); 
      
      std::vector<int> deadCHsLR = {};
      std::vector<int> deadCHsHR = {};
      
      this->dataFillHR(&_iHR, &_HR, &deadCHsHR);
      this->dataFillLR(&_iLR, &_LR, &deadCHsLR);

      this->recoveryCHs(deadCHsLR, &_HR, &_LR);
      this->recoveryCHs(deadCHsHR, &_LR, &_HR);
      
      this->dataFillAndOr(_HR, _LR, &_notHR, &_notLR, &_OR, &_AND);
      _isStripsFill = true; result = true;
    }
  }
  if(!_isNoiseFill) {
    if(param == "noise") {
      this->filter(&_iNoiseHR, &_iNoiseLR); 
      
      std::vector<int> deadCHsLR = {};
      std::vector<int> deadCHsHR = {};
      
      this->dataFillHR(&_iNoiseHR, &_noiseHR, &deadCHsHR);
      this->dataFillLR(&_iNoiseLR, &_noiseLR, &deadCHsLR);

      std::vector<std::pair<double, double>> notHR; std::vector<std::pair<double, double>> notLR; 
      this->dataFillAndOr(_noiseHR, _noiseLR, &notHR, &notLR, &_noiseOR, &_noiseAND);
      notHR.clear(); notLR.clear();
      _isNoiseFill = true; result = true;
    }
  }
  return result;
}
bool LyPetiAnalysis::dataFillHR(std::vector<std::pair<int, int>>* iHR, 
                                std::vector<std::pair<double, double>>* output,
                                std::vector<int>* deadCHsHR) 
{
  double numStrip = 0; double trigTime = 0; double hitTime = 0; 
  for(unsigned int ih = 0; ih < iHR->size(); ih++) {
      numStrip = _strips->at(iHR->at(ih).first).number;
      hitTime = (double)_strips->at(iHR->at(ih).first).HR.at(iHR->at(ih).second)->time;
      trigTime = (double)_strips->at(iHR->at(ih).first).trigs.at(0)->time;
      
      if(_strips->at(iHR->at(ih).first).deadHR == 1) deadCHsHR->push_back(numStrip);
      
      double time = hitTime - trigTime;
      if(_isTimeOffsetHR) time = ((time - _strips->at(iHR->at(ih).first).timeOffSetHR));
        output->push_back(std::pair<double, double>(numStrip, time));
    }
  return true;
}
bool LyPetiAnalysis::dataFillLR(std::vector<std::pair<int, int>>* iLR,
                                std::vector<std::pair<double, double>>* output, 
                                std::vector<int>* deadCHsLR) 
{
  double numStrip = 0; double trigTime = 0; double hitTime = 0; 
  for(unsigned int il = 0; il < iLR->size(); il++) {
    numStrip = _strips->at(iLR->at(il).first).number; 
    hitTime = (double)_strips->at(iLR->at(il).first).LR.at(iLR->at(il).second)->time;
    trigTime = (double)_strips->at(iLR->at(il).first).trigs.at(0)->time;
    
    if(_strips->at(iLR->at(il).first).deadLR == 1) deadCHsLR->push_back(numStrip);
    
    double time = hitTime - trigTime;
    if(_isTimeOffsetLR) time = ((time - _strips->at(iLR->at(il).first).timeOffSetLR)); 
      output->push_back(std::pair<double, double>(numStrip, time));
  }
  return true;
}
bool LyPetiAnalysis::dataFillAndOr(std::vector<std::pair<double, double>> HR,
                                   std::vector<std::pair<double, double>> LR,
                                   std::vector<std::pair<double, double>>* notHR, 
                                   std::vector<std::pair<double, double>>* notLR, 
                                   std::vector<std::pair<double, double>>* OR, 
                                   std::vector<std::pair<double, double>>* AND) 
{
    this->sortAndResize(&HR, &LR);
    for(unsigned int i = 0; i < HR.size(); i++) {
      for(unsigned int j = 0; j < LR.size(); j++) {
        if(HR.at(i).first == LR.at(j).first) {
          for(unsigned int l = 0; l < LR.size(); l++) {
            if((LR.at(j).first == LR.at(l).first) && (std::abs(LR.at(j).second - HR.at(i).second) > std::abs(LR.at(l).second - HR.at(i).second)))
              std::swap(LR[j],  LR[l]);
          }
          double time = (HR.at(i).second - LR.at(j).second);
          AND->push_back(std::pair<double, double>(HR.at(i).first, time));
          _pairAND.push_back(std::make_pair(HR.at(i).first, std::make_pair(HR.at(i).second, LR.at(j).second)));
          OR->push_back(std::pair<double, double>(HR.at(i).first, HR.at(i).second));
          LR.erase(LR.begin()+j); HR.erase(HR.begin()+i);
          break;
	  		}
        else if(j == LR.size()-1 && HR.at(i).first != _limit) {
          OR->push_back(std::pair<double, double>(HR.at(i).first, HR.at(i).second));
          notHR->push_back(std::pair<double, double>(HR.at(i).first, HR.at(i).second));
        }
      }
    }
    for(unsigned int j = 0; j < LR.size(); j++) {
      if(LR.at(j).first != _limit) {
        OR->push_back(std::pair<double, double>(LR.at(j).first, LR.at(j).second));
        notLR->push_back(std::pair<double, double>(LR.at(j).first, LR.at(j).second));
      }
    }
  return true;
}
/* Basic Cluster Data */ 
bool LyPetiAnalysis::choiceDataClusterBasic(std::string param) 
{
  bool result = false;
  if(!_isClusterBasic) {
    if(param == "data") {
      if(_dataCB == 1) 
        this->iCluster(_HR, _LR, &_CB, _isOR);
      _isClusterBasic = true; result = true;
    }
  }
  if(!_isNoiseClusterBasic) {
    if(param == "noise") {
    if(_dataCB == 1) 
        this->iCluster(_noiseHR, _noiseLR, &_noiseCB, true);
      _isNoiseClusterBasic = true; result = true;
    }
  }
  return result;
}
bool LyPetiAnalysis::iCluster(std::vector<std::pair<double, double>> dataHR, std::vector<std::pair<double, double>> dataLR,  
                                  std::map<int, lyCB::data>* output, bool isOR) 
{
  iRPCClusterContainer clusters;
  iRPCClusterizer clusterizer;
  iRPCInfo info;
  info.setThrTimeHR(_timeThrHR); info.setThrTimeLR(_timeThrLR);
  
  info.setThrDeltaTimeMin(_thrDeltaMin); info.setThrDeltaTimeMax(_thrDeltaMax);
  info.setThrDeltaTimeY(_timeThrA);
  info.isReturnOnlyAND(!isOR);
  
  clusters = clusterizer.doAction(dataHR, dataLR, info);
	for(unsigned int i = 0; i < clusters.size(); i++) {
    lyCB::data cluster;
	  for(unsigned int j = 0; j < clusters.at(i).hits()->size(); j++) {
     if(clusters.at(i).hits()->at(j).isHR()) cluster.HR.push_back(std::pair<int, double>(clusters.at(i).hits()->at(j).channel(), clusters.at(i).hits()->at(j).time()));
     if(clusters.at(i).hits()->at(j).isLR()) cluster.LR.push_back(std::pair<int, double>(clusters.at(i).hits()->at(j).channel(), clusters.at(i).hits()->at(j).time()));
    }
    cluster.middleStrip = clusters.at(i).x();    
    cluster.middleTime = clusters.at(i).deltaTime();
    cluster.size =  clusters.at(i).clusterSize();
    output->insert(std::make_pair(i, cluster));
	}
  //// Print data (test)	
  //std::cout << "\nCouple: " << clusters.size();	
	//for(int i = 0; i < clusters.size(); i++) {
	//	std::cout << "\n\nfirst: " << clusters.at(i).firstStrip() << " last: " << clusters.at(i).lastStrip() << " size: " << clusters.at(i).clusterSize() <<  " time: " << std::setprecision(4) << clusters.at(i).deltaTime();
	//	std::cout << "\nn: " <<  clusters.at(i).hits()->size() << "  ";
	//	for(int j = 0; j < clusters.at(i).hits()->size(); j++)
	//		std::cout << std::setprecision(4) << clusters.at(i).hits()->at(j).channel() << "|" << clusters.at(i).hits()->at(j).time() << " " << clusters.at(i).hits()->at(j).isHR() << "; ";
	//}
	//std::cout << "\n------------------------------------------------------\n";
  
  return false;
}
//------------------------------------------------------------------------------

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
