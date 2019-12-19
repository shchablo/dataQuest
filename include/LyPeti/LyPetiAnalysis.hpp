#ifndef LYPETIANALYSIS_h
#define LYPETIANALYSIS_h 

#include <map>
#include <vector>
#include <limits>
#include <algorithm>
#include <TH1D.h>
#include <TH2D.h>

#include "DqParser.hpp"
#include "LyPetiEvent.hpp"
#include "DqRoot.hpp"
#include "iRPCClusterizer.hpp"
#include "iRPCClusterContainer.hpp"
#include "iRPCInfo.hpp"

namespace lyCB {
  struct data
  {
    int size;
    int centralStrip;
    double centralTime;
    double time;
    double middleTime;
    double middleStrip;
    double firstTime;
    double firstStrip;
    double X;
    double Y;
    std::vector<std::pair<double, double>> hits;
    
    std::vector<std::pair<double, double>> HR;
    std::vector<std::pair<double, double>> LR;
  };
}
class LyPetiAnalysis
{

public:
  /* Constructor, destructor. */ 
  LyPetiAnalysis();
  virtual ~LyPetiAnalysis();
  //----------------------------------------------------------------------------
  std::vector<std::pair<double, std::pair<double, double>>> pairData(std::string mod);
  std::vector<std::pair<double, double>> data(std::string mod); 
  
  bool stripsData(std::vector<std::pair<double, double>> *data, std::string mod); 
  bool clusterBasicData(std::vector<std::pair<double, double>> *data, std::string mod); 
  
  std::map<int, lyCB::data> data();
  //----------------------------------------------------------------------------

  bool setParams(std::map<std::string, double> params);
  bool setRuns(std::vector<int> runs, std::vector<double> values);
  bool setParser(DqParser* parser);
  bool setStrips(std::vector<lyBuf::strip>* strips);
	bool setNumBoards(int numBoards);
  
  double getValue(int run);
  int getNumberEvents();
  bool getNumSkipedTrigers(double* num);
  bool getNumTrigers(double* num);
  bool skipEvent(bool is); 
  bool skipEvent(); 
  
  bool setDeadCHsHR(std::vector<int> dead);
  bool setDeadCHsLR(std::vector<int> dead);
  bool configure();
  
  bool clear(std::string type);
  bool clearEvent();
  bool clearRun();
  bool newWindows();
  
  std::string filtersStr();
  std::string modParamsStr(std::string mod);
  //----------------------------------------------------------------------------

  bool efficiency(double* eff, double* eEff, std::string mod);
  
  bool efficiency(std::string mod);
  bool stripsEfficiency(std::string mod); 
  bool clusterBasicEfficiency(std::string mod);
  
  bool noise(double* n, double* eN, int strip, int entries, std::string mod);
  
  bool multiplicity(int* mult, std::string mod); 
  bool stripsMultiplicity(int* mult, std::string mod); 
  bool clusterBasicMultiplicity(int* mult, std::string mod);
  bool clusterSize(std::vector<int>* sizes, std::string mod);
  
  bool minTime(double* time, std::string mod); 
  bool maxTime(double* time, std::string mod); 
  
  bool filter(std::vector<std::pair<int, int>>* iHR,
                   std::vector<std::pair<int, int>>* iLR); 
  //----------------------------------------------------------------------------
  bool algos(std::string mod);
  //----------------------------------------------------------------------------
  bool stripsAlgos();
  bool choiceDataFill(std::string param);
  bool recoveryCHs(std::vector<int> deadCHs,
                                std::vector<std::pair<double, double>>* donor, 
                                std::vector<std::pair<double, double>>* recipient); 
  bool dataFillHR(std::vector<std::pair<int, int>>* iHR, 
                                std::vector<std::pair<double, double>>* output); 
  bool dataFillLR(std::vector<std::pair<int, int>>* iLR,
                                std::vector<std::pair<double, double>>* output); 
  bool dataFillAndOr(std::vector<std::pair<double, double>> HR,
                     std::vector<std::pair<double, double>> LR,
                                   std::vector<std::pair<double, double>>* notHR, 
                                   std::vector<std::pair<double, double>>* notLR, 
                                   std::vector<std::pair<double, double>>* OR, 
                                   std::vector<std::pair<double, double>>* AND); 
  //----------------------------------------------------------------------------
  bool clusterBasicAlgos();
  
  bool choiceDataClusterBasic(std::string param); 
  bool  iCluster(std::vector<std::pair<double, double>> dataHR, std::vector<std::pair<double, double>> dataLR,  
                                  std::map<int, lyCB::data>* output, bool isOR); 
  //----------------------------------------------------------------------------
  
  bool fillEvents(std::string radius, unsigned int js, unsigned int jr); 
  bool fillTrigers(unsigned int numBoards); 
	bool fillTrigers(std::vector<double> trigers); 
  
  bool filters(std::string radius, unsigned int js, unsigned int jr); 
  bool triger(unsigned int js);
  bool window(std::string radius, unsigned int js, unsigned int jr); 
  bool BCID(unsigned int js); 
  //----------------------------------------------------------------------------
  
  //----------------------------------------------------------------------------
  bool findParam(std::string name, double* param);
  bool findParam(std::string name, bool* param);
  template <typename T> bool sortAndResize(std::vector<std::pair<T, T>> *v1, 
													       std::vector<std::pair<T, T>> *v2); 
  //----------------------------------------------------------------------------
  
protected:
 
  std::map<int, double> _offsets;
  std::map<int, double> _offsetsHR;
  std::map<int, double> _offsetsLR;
  bool _isSkipEvent;
  int _numberOfEvents;

  bool _isTimeOffsetHR; bool _isTimeOffsetLR;
  bool _isTimeOffset;
  int _limit; // min strip number shouble more then limit
  
  DqParser* _parser; // helpful class 
  std::vector<lyBuf::strip>* _strips; // data  
  
  unsigned int _numBoards;   
  std::map<std::string, double> _params; // params for analysis   
  std::vector<int> _deadCHsHR; std::vector<int> _deadCHsLR;   
  std::map<int, double> _runs; // values for correspond run number (HV, Current, etc.)
  
  // indexes for filtered data for one event - should be clear with function clear each event
  std::vector<std::pair<int, int>> _iHR; std::vector<std::pair<int, int>> _iNoiseHR;
  std::vector<std::pair<int, int>> _iLR; std::vector<std::pair<int, int>> _iNoiseLR;
  std::vector<std::pair<int, int>> _iHRf; std::vector<std::pair<int, int>> _iLRf;
 
  // data for strips
  bool _isStripsFill;  bool _isNoiseFill;  
  std::vector<std::pair<double, double>> _HR; std::vector<std::pair<double, double>> _noiseHR;
  std::vector<std::pair<double, double>> _LR; std::vector<std::pair<double, double>> _noiseLR;
  std::vector<std::pair<double, double>> _AND; std::vector<std::pair<double, double>> _noiseAND;
  std::vector<std::pair<double, double>> _OR; std::vector<std::pair<double, double>> _noiseOR;
  std::vector<std::pair<double, double>> _notHR; std::vector<std::pair<double, double>> _notLR; //  outside cluster
  std::vector<std::pair<double, double>>  _HRf; std::vector<std::pair<double, double>> _LRf; // filter
  std::vector<std::pair<double, double>>  _ANDf;
  
  std::vector<std::pair<double, std::pair<double, double>>> _pairAND;
  
  int _numNoiseHitsHR; int _numNoiseHitsLR;  int _numNoiseHitsOR; 
  int _numNoiseHitsAND; int _numNoiseHitsCB;
  
  int _numEffTrigers;
  int _numSkipedTrigers;
  int _numDeadTimeEvents;

  int _numEffEventsHR; int _numEffNoiseEventsHR;
  int _numEffEventsLR; int _numEffNoiseEventsLR;
  int _numEffEventsOR; int _numEffNoiseEventsOR;
  int _numEffEventsAND; int _numEffNoiseEventsAND;
  
  // data for basicClustering
  bool _isMinMaxTime; double _minTime;  double _maxTime; 
	double _timeThrCB; 
	double _timeThrHR; double _timeThrLR; double _timeThrA;
  double _thrDeltaMin; double _thrDeltaMax; bool _isOR;
  int _numEffEventsCB; int _numEffNoiseEventsCB;
  bool _isClusterBasic; bool _isNoiseClusterBasic;
  int _dataCB;
  std::map<int, lyCB::data> _CB; std::map<int, lyCB::data> _noiseCB; 
    
  // data for filters
  bool _isAll; bool _isFoundTriger; bool _isTrigers; int _thrTrigers;  
  
  double _preTrigTime;
  int _LRfilter; int _HRfilter;

  TH1F* _hHR; TH1F* _hLR; 
  bool _isWindow; double _rangeHR; double _rangeLR;   
  double _begWindowHR; double _begWindowHR_def; 
  double _endWindowHR; double _endWindowHR_def; 
  double _begWindowLR; double _begWindowLR_def;  
  double _endWindowLR; double _endWindowLR_def;
  
  
  bool _isNoise; double _begNoise; double _endNoise; double _randShift;  
  
  bool _isBCID; double _begBCID; double _endBCID;  

  bool _isDeadTime;  
  double _deadTime;  
  
  double _noiseUnit; 
  double _chamberArea; 
  double _stripArea; 
  
  double _newWindow; 
  // info strings
  std::string _filters;  
};
#endif 
