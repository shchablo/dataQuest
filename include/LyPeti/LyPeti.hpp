//------------------------------------------------------------------------------
/* INFORMATION: */
//------------------------------------------------------------------------------
#ifndef LYPETI_h
#define LYPETI_h

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <map>
#include <vector>
#include <limits>
#include <algorithm>

#include "DqAnalysis.hpp"
#include "DqCSV.hpp"
#include "DqRoot.hpp"

#include "LyPetiEvent.hpp"
#include "LyPetiRead.hpp"
#include "LyPetiAnalysis.hpp"

#include "DqProcessBar.hpp"
#include "DqParser.hpp"

class LyPeti : public DqAnalysis
{
public:

  /* Constructor, destructor. */ 
  //----------------------------------------------------------------------------
  LyPeti(std::string sysMods, 
               std::map<std::string, std::string> params,
               std::vector<std::string> inputs,
               std::vector<std::string> outputs);
  virtual ~LyPeti();
  //----------------------------------------------------------------------------
  
  /* Get functions. */ 
  virtual std::string getLog();
  //----------------------------------------------------------------------------
  
  /* analysis params and input files */
  std::vector<int> parsNumRuns(std::vector<std::string> files); 
  int numRun(std::string file);
  bool saveHists(int run, std::map<int, LyPetiAnalysis> *analysis, DqRoot *dataRoot,
                       std::map<std::string, DqTH1D> *TH1s, std::map<std::string, DqTH2D> *TH2s);
  bool saveGraphs(std::map<int, LyPetiAnalysis> *analysis, DqRoot *dataRoot,
                        std::map<std::string, DqTGE*> *TGEs);
  bool saveMGraphs(std::map<int, LyPetiAnalysis> *analysis, DqRoot *dataRoot,
                        std::map<std::string, DqMTGE*> *MTGEs);
  //----------------------------------------------------------------------------
  bool petiroc(std::pair<int, int> run, std::vector<std::string> *mods,  
                     LyPetiEvent *event, std::map<int, LyPetiAnalysis> *analysis, 
                     DqRoot *dataRoot, 
                     std::map<std::string, DqTGE*> *TGEs, 
                     std::map<std::string, DqMTGE*> *MTGEs, 
                     std::map<std::string, DqTH1D> *TH1s, 
                     std::map<std::string, DqTH2D> *TH2s);
  /* analysis */
  bool stripStudy(std::string mod, std::vector<std::string> *mods, 
                  int run, LyPetiEvent *event, std::map<int, LyPetiAnalysis> *analysis,
                  DqRoot *dataRoot,
                  std::map<std::string, DqTGE*> *TGEs, 
                  std::map<std::string, DqTH1D> *TH1s, 
                  std::map<std::string, DqTH2D> *TH2s);
  bool raw(std::string mod, LyPetiEvent *event, std::map<int, LyPetiAnalysis> *analysis, 
                 DqRoot *dataRoot, std::map<int, TTree*> *treesHR, std::map<int, TTree*> *treesLR,
                 int run, 
                 Int_t *number_HR, Float_t *time_HR, Float_t *triger_HR, Float_t *ch_HR, Int_t *size_HR, Float_t *dtime_HR, uint16_t *BCID_HR, uint64_t *coarse_HR, uint8_t *fine_HR,   
                 Int_t *number_LR, Float_t *time_LR, Float_t *triger_LR, Float_t *ch_LR, Int_t *size_LR, Float_t *dtime_LR, uint16_t *BCID_LR, uint64_t *coarse_LR, uint8_t *fine_LR); 
  bool offset(std::string mod, std::vector<std::string> *mods,  
              int run, int chamber, LyPetiEvent *event, std::map<int, LyPetiAnalysis> *analysis,
              DqRoot *dataRoot,
              std::map<std::string, DqTGE*> *TGEs, 
              std::map<std::string, DqTH1D> *TH1s, 
              std::map<std::string, DqTH2D> *TH2s);
  bool core(std::string mod, std::vector<std::string> *mods, 
                  int run, LyPetiEvent *event, std::map<int, LyPetiAnalysis> *analysis,
                  DqRoot *dataRoot,
                  std::map<std::string, DqTGE*> *TGEs, 
                  std::map<std::string, DqTH1D> *TH1s, 
                  std::map<std::string, DqTH2D> *TH2s);
  bool ATime(std::string mod, std::vector<std::string> *mods, 
                  int run, LyPetiEvent *event, std::map<int, LyPetiAnalysis> *analysis,
                  DqRoot *dataRoot,
                  std::map<std::string, DqTGE*> *TGEs, 
                  std::map<std::string, DqTH1D> *TH1s, 
                  std::map<std::string, DqTH2D> *TH2s);
  bool ATimeSingleChamber(std::string mod, std::vector<std::string> *mods, 
                  int run, LyPetiEvent *event, std::map<int, LyPetiAnalysis> *analysis,
                  DqRoot *dataRoot,
                  std::map<std::string, DqTGE*> *TGEs, 
                  std::map<std::string, DqTH1D> *TH1s, 
                  std::map<std::string, DqTH2D> *TH2s);
  bool ClusterAlgoTest(std::string mod, std::vector<std::string> *mods, 
                  int run, LyPetiEvent *event, std::map<int, LyPetiAnalysis> *analysis,
                  DqRoot *dataRoot,
                  std::map<std::string, DqTGE*> *TGEs, 
                  std::map<std::string, DqMTGE*> *MTGEs, 
                  std::map<std::string, DqTH1D> *TH1s, 
                  std::map<std::string, DqTH2D> *TH2s);
  /* Loop function */ 
  //----------------------------------------------------------------------------
  bool initEvent(LyPetiEvent *event);
  std::vector<int> initRuns();
  std::vector<std::pair<int, int>> initOffSetRuns(std::vector<int> *runs,  LyPetiEvent *event);
  bool initAnalys(std::map<int, LyPetiAnalysis> *analysis, LyPetiEvent *event, std::vector<int> runs);
  bool analysis();
  bool binToROOT();
  //----------------------------------------------------------------------------
  
  virtual bool run();
protected:
	
  std::string _rootFilePath;
  DqParser _parser; std::string _strDelim;  
  std::string  _logRunNumber;  std::string _comment;
  std::string _mod;
  std::map<std::string, std::string> _params;
  std::vector<std::string> _inputs;
  std::vector<std::string> _outputs;
};

#endif
//------------------------------------------------------------------------------
