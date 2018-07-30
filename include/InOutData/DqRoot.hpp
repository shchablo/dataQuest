//------------------------------------------------------------------------------
/* INFORMATION: */
//------------------------------------------------------------------------------
#ifndef DQROOT_h
#define DQROOT_h

/* ROOT includes */
#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TDirectory.h>
#include <TObject.h>
#include <TCanvas.h>
#include <TImage.h>
#include <TFrame.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TF1.h>
#include <TString.h>
#include <TGraphErrors.h>
#include <TMultiGraph.h>
#include <TGraph.h>
#include <TMath.h>
//------------------------------------------------------------------------------

/* DQ includes */

#include "DqParser.hpp"
#include "DqTH2D.hpp"
#include "DqTH1D.hpp"
#include "DqTGE.hpp"
//------------------------------------------------------------------------------

/* C++ includes */
#include <string>
#include <iostream>
#include <vector>
#include <math.h>
//------------------------------------------------------------------------------

class DqRoot
{
public:

  /* Constructor, destructor. */ 
  DqRoot();
  virtual ~DqRoot();
  //----------------------------------------------------------------------------

  /* Set functions. */ 
  bool setOutputFile(std::string outputFileName, std::string outputTreeName);
  bool setBranch(double *data, std::string branchName);
  //----------------------------------------------------------------------------

  //----------------------------------------------------------------------------
  bool histParams(std::string name, double value, 
                        std::map<std::string, double>* params, int mod);
  bool fillHist(std::string name, double value, 
                             std::map<std::string, TH1D>* hists,
                             std::map<std::string, double>* histsParams,
                             std::map<std::string, std::string>* params);
  
  bool writeObject(std::string dirName, TObject* object);
	bool writeTH1toCanvasCMS(std::string path, TH1* object, bool isCanvas);
	bool writeDqTGEtoCanvasCMS(std::string path, DqTGE* object, bool isCanvas);
	bool writeTH2toCanvasCMS(std::string path, TH2* object, bool isCanvas);
  bool writeTMGtoCanvasCMS(std::string path, std::vector<TH1>* objects, bool isCanvas);
  //----------------------------------------------------------------------------

  std::map<std::string, DqTH2D>::iterator addDqTH2D(std::string name, std::map<std::string, DqTH2D>* dqTH2Ds);
  std::map<std::string, DqTH1D>::iterator addDqTH1D(std::string name, std::map<std::string, DqTH1D>* dqTH1Ds);
  std::map<std::string, DqTGE*>::iterator  addDqTGE(std::string name, std::map<std::string, DqTGE*>* dqTGEs);
  //----------------------------------------------------------------------------
protected:

  bool isOutFile_;
  TFile *outFile_;
  TTree *outTree_;

//char  *dirName_;
//char  *plotName_;
};
#endif
//------------------------------------------------------------------------------
