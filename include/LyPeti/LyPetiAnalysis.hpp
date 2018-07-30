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



namespace lyCB {
  struct data
  {
    int size;
    int centralStrip;
    double centralTime;
    double time;
    double X;
    double Y;
  };
}
class LyPetiAnalysis
{

public:
  /* Constructor, destructor. */ 
  LyPetiAnalysis();
  virtual ~LyPetiAnalysis();
  //----------------------------------------------------------------------------
  std::vector<std::pair<double, double>> data(std::string mod); 
  
  bool stripsData(std::vector<std::pair<double, double>> *data, std::string mod); 
  bool clusterBasicData(std::vector<std::pair<double, double>> *data, std::string mod); 
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
  bool skipEvent(); 
  
  bool configure();
  
  bool clear(std::string type);
  bool clearEvent();
  bool clearRun();
  
  std::string filtersStr();
  std::string modParamsStr(std::string mod);
  //----------------------------------------------------------------------------

  bool efficiency(double* eff, double* eEff, std::string mod);
  
  bool efficiency(std::string mod);
  bool stripsEfficiency(std::string mod); 
  bool clusterBasicEfficiency(std::string mod);
  
  bool noise(double* n, double* eN, std::string mod);
  
  bool offSetTime(std::vector<std::pair<double, double>>* data, 
                              std::map<int, std::vector<double>>* output, std::string mod); 
  bool multiplicity(int* mult, std::string mod); 
  bool stripsMultiplicity(int* mult, std::string mod); 
  bool clusterBasicMultiplicity(int* mult, std::string mod);
  bool clusterSize(std::vector<int>* sizes, std::string mod);
  bool eventFraction(double* fraction, std::string mod);
  //----------------------------------------------------------------------------

  bool isOffSet(std::string mod);
  bool algos(std::string mod);
  //----------------------------------------------------------------------------
  bool stripsAlgos();
  bool choiceDataFill(std::string param);
  bool dataFillHR(std::vector<std::pair<int, int>>* iHR, 
                                std::vector<std::pair<double, double>>* output, bool inverse, bool isOffSet); 
  bool dataFillLR(std::vector<std::pair<int, int>>* iLR,
             std::vector<std::pair<double, double>>* output, bool inverse);
  bool dataFillAndOr(std::vector<std::pair<double, double>>* OR, 
                     std::vector<std::pair<double, double>>* AND);
  //----------------------------------------------------------------------------
  bool clusterBasicAlgos();
  
  bool choiceDataClusterBasic(std::string param); 
  bool clusterBasic(std::vector<std::pair<double, double>> data, std::map<int, lyCB::data>* output); 
  int centralStrip(std::vector<std::pair<double, double>>* strips); 
  //----------------------------------------------------------------------------
  
  bool fillEvents(std::string radius, unsigned int js, unsigned int jr); 
	bool fillTrigers(int numBoards); 
  
  bool filters(std::string radius, unsigned int js, unsigned int jr); 
  bool triger(unsigned int js);
  bool window(std::string radius, unsigned int js, unsigned int jr); 
  bool BCID(std::string radius, unsigned int js, unsigned int jr);
  //----------------------------------------------------------------------------
  
  //----------------------------------------------------------------------------
  bool findParam(std::string name, double* param);
  bool findParam(std::string name, bool* param);
  template <typename T> bool sortAndResize(std::vector<std::pair<T, T>> *v1, 
													       std::vector<std::pair<T, T>> *v2); 
  //----------------------------------------------------------------------------
  
protected:
 
  bool _isSkipEvent;
  int _numberOfEvents;

  bool _isTimeOffsetHR; bool _isTimeOffsetLR;
  bool _isTimeOffset;
  int _limit; // min strip number shouble more then limit
  
  DqParser* _parser; // helpful class 
  std::vector<lyBuf::strip>* _strips; // data  
  
  int _numBoards;   
  std::map<std::string, double> _params; // params for analysis   
  std::map<int, double> _runs; // values for correspond run number (HV, Current, etc.)
  
  // indexes for filtered data for one event - should be clear with function clear each event
  std::vector<std::pair<int, int>> _iHR; std::vector<std::pair<int, int>> _iNoiseHR;
  std::vector<std::pair<int, int>> _iLR; std::vector<std::pair<int, int>> _iNoiseLR;
 
  // data for strips
  bool _isStripsFill;  bool _isNoiseFill;  
  std::vector<std::pair<double, double>> _HR; std::vector<std::pair<double, double>> _noiseHR;
  std::vector<std::pair<double, double>> _LR; std::vector<std::pair<double, double>> _noiseLR;
  std::vector<std::pair<double, double>> _AND; std::vector<std::pair<double, double>> _noiseAND;
  std::vector<std::pair<double, double>> _OR; std::vector<std::pair<double, double>> _noiseOR;
  
  int _numNoiseHitsHR; 
  int _numNoiseHitsLR; 
  int _numNoiseHitsOR; 
  int _numNoiseHitsAND; 
  int _numNoiseHitsCB;
  
  int _numEffTrigers;
  int _numSkipedTrigers;

  int _numEffEventsHR; int _numEffNoiseEventsHR;
  int _numEffEventsLR; int _numEffNoiseEventsLR;
  int _numEffEventsOR; int _numEffNoiseEventsOR;
  int _numEffEventsAND; int _numEffNoiseEventsAND;
  
  // data for basicClustering
	double _timeThrCB; 
  int _numEffEventsCB; int _numEffNoiseEventsCB;
  int _goodCluster;  int _badCluster;
  bool _isClusterBasic; bool _isNoiseClusterBasic;
  int _dataCB;
  std::map<int, lyCB::data> _CB; std::map<int, lyCB::data> _noiseCB; 
    
  // data for filters
  bool _isAll;  
  
  bool _isTrigers;  
  int _thrTrigers;  
  
  bool _isWindow;  
  double _begWindow;  
  double _endWindow;  
  
  bool _isNoise;  
  double _begNoise;  
  double _endNoise;  
  
  bool _isBCID;  
  double _begBCID;  
  double _endBCID;  

  bool _isDeadTime;  
  double _deadTime;  
  
  double _noiseUnit; 
  double _chamberArea; 
  // info strings
  std::string _filters;  
};
#endif 
