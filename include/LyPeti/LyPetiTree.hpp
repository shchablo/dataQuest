//------------------------------------------------------------------------------
/* INFORMATION: */
//------------------------------------------------------------------------------
#ifndef LYPETITREE_h
#define LYPETITREE_h

#include <DqRoot.hpp>

// Root Header Files
#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TFrame.h"
#include "TRandom.h"
#include "TSystem.h"
#include "TBrowser.h"
#include "TBenchmark.h"
#include "TObject.h"
#include "TH1.h"

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link C++ nestedclass;
#pragma link C++ nestedtypedef;

#pragma link C++ namespace lyTree;
#pragma link C++ defined_in namespace lyTree;
#pragma link C++ class event::Event+;

#endif


//#ifdef __MAKECINT__
//#pragma link C++ class Event+;
//#endif

namespace lyTree {

class runHeader : public TObject {
  public:
    runHeader() {;};
    ~runHeader() {;};
  private:
    char chamberName[256]; char commnet[1024];
    Int_t webdcs; Int_t lyDaq; 
    Float_t X1; Float_t Y1; Float_t X2; Float_t Y2;
    TH1F *HVapp_top; TH1F *HVeff_top;
    TH1F *HVapp_bot; TH1F *HVeff_bot;
    TH1F *Imon_top; TH1F *Imon_bot;
    Int_t ABS; Float_t ATT;
    Float_t P; Float_t T;
};
class eventHeader: public TObject 
{
  public:
    eventHeader() {;};
    ~eventHeader() {;};
    void setNumber(int input) {number = input;};
    void setNumOfCh(int input) {numOfCh = input;};
  private:
	  Int_t number; Int_t gtc;
	  Int_t absbcID; Int_t boardID;
	  Int_t boardIP[4]; Int_t numOfCh;
  
};
class payload: public TObject 
{
  public:
    payload() {;};
    ~payload() {;};
    void setCh(int input) {ch=input;};
  private:
	  // default
	  uint8_t ch;
	  uint16_t BCID;
	  uint64_t Coarse; 
	  uint8_t fine;
	
	  // derived parameter
	  Float_t time;

};

class raw: public TObject 
{
  public:
    raw() {;};
    ~raw() {;};
    eventHeader *getEventHeader() {return &fEvtHdr;};
    payload *getPayload() {return &fPayload;};
  private:
    eventHeader fEvtHdr;
    payload fPayload;

};
};
class Event : public TObject
{
	public:
		//Event() {;};
  	void setIndex(int input) {index = input;}; 
  private:
    int index;
//ClassDef(Event,1);

};
//ClassImp(Event)
#endif
//------------------------------------------------------------------------------
