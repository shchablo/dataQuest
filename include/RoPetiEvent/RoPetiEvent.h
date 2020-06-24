#ifndef ROPETIEVENT_H
#define ROPETIEVENT_H

/* ROOT includes */
#include <TROOT.h>
#include "TObject.h"
#include "TClonesArray.h"
//------------------------------------------------------------------------------
class RoPetiHeader {

private:
   Int_t   fEvtNum;
   Int_t   fLyRun;

public:
   RoPetiHeader() {;};
   virtual ~RoPetiHeader() {;};
   void   Set(Int_t n, Int_t r) {fEvtNum = n; fLyRun = r; }
   Int_t  GetEvtNum() const { return fEvtNum; }
   Int_t  GetLyRun() const { return fLyRun; }

   ClassDef(RoPetiHeader, 1)  //Event Header
};
class RoPetiPayload : public TObject {

private:
  Int_t fBoardID;
  Int_t fGTC;
  
  Int_t fCh; Int_t fStrip;
  
  Double32_t fTime;
  Double32_t fFine;
  Double32_t fCoarse;
 
  Int_t fIsHR;
  Int_t fIsLR;
public:
   RoPetiPayload() {;};
   virtual ~RoPetiPayload() {;};
  

   void Set(Int_t ch, Double32_t time) { fCh = ch; fTime = time; }
   
   void SetBoardID(Int_t boardID) { fBoardID = boardID; }
   void SetGTC(Int_t gtc) { fGTC = gtc; }
   
   void SetCh(Int_t ch) { fCh = ch; }
   void SetStrip(Int_t strip) { fStrip = strip; }
   
   void SetTime(Double32_t time) { fTime = time; }
   void SetFine(Double32_t fine) { fFine = fine; }
   void SetCoarse(Double32_t coarse) { fCoarse = coarse; }
   
   void SetIsHR(Int_t isHR) { fIsHR = isHR; }
   void SetIsLR(Int_t isLR) { fIsLR = isLR; }

   void Clear(Option_t * /*option*/) { TObject::Clear(); }
   ClassDef(RoPetiPayload, 2)  //Event Header

};

class RoPetiEvent : public TObject {

private:
   RoPetiHeader fEvtHdr;
   Int_t fNumTrigs; TClonesArray *fTrigs;
   Int_t fNumHits; TClonesArray *fHits;

   static TClonesArray *fgTrigs;
   static TClonesArray *fgHits;
public :
   RoPetiEvent() {
     if(!fgTrigs) fgTrigs = new TClonesArray("RoPetiPayload", 1000); 
     fTrigs = fgTrigs; fNumTrigs = 0;
     if(!fgHits) fgHits = new TClonesArray("RoPetiPayload", 1000); 
     fHits = fgHits; fNumHits = 0;
   };
   virtual ~RoPetiEvent() {;};
   
  RoPetiHeader *getHeader() { return &fEvtHdr; }
  TClonesArray *getTrigs() const { return fTrigs; }
  TClonesArray *getHits() const { return fHits; }
  
  RoPetiPayload *addTrig(Int_t ch, Float_t time) {fTrigs->Expand(fNumTrigs+1);  RoPetiPayload *trig = (RoPetiPayload*)fTrigs->ConstructedAt(fNumTrigs++); trig->Set(ch, time); return trig; };
  RoPetiPayload *addTrig(Int_t boardID, Int_t gtc, 
                         Int_t ch, 
                         Double32_t time, Double32_t fine, Double32_t coarse) 
    {
      fTrigs->Expand(fNumTrigs+1); RoPetiPayload *trig = (RoPetiPayload*)fTrigs->ConstructedAt(fNumTrigs++); 
     
      trig->SetBoardID(boardID);
      trig->SetGTC(gtc);
      
      trig->SetCh(ch);
      trig->SetStrip(-1);
   
      trig->SetTime(time);
      trig->SetFine(fine);
      trig->SetCoarse(coarse);
   
      trig->SetIsHR(1); trig->SetIsLR(1); 
      
      return trig; 
    };
  
  RoPetiPayload *addHit(Int_t ch, Float_t time) { fHits->Expand(fNumHits+1); RoPetiPayload *hit = (RoPetiPayload*)fHits->ConstructedAt(fNumHits++); hit->Set(ch, time); return hit; };
  RoPetiPayload *addHit(Int_t boardID, Int_t gtc, 
                        Int_t ch, Int_t strip, 
                        Double32_t time, Double32_t fine, Double32_t coarse,
                        Int_t isHR, Int_t isLR) 
    {
      fHits->Expand(fNumHits+1); RoPetiPayload *hit = (RoPetiPayload*)fHits->ConstructedAt(fNumHits++); 
     
      hit->SetBoardID(boardID);
      hit->SetGTC(gtc);
      
      hit->SetCh(ch);
      hit->SetStrip(strip);
   
      hit->SetTime(time);
      hit->SetFine(fine);
      hit->SetCoarse(coarse);
   
      hit->SetIsHR(isHR); hit->SetIsLR(isLR); 
      
      return hit; 
    };
  
  void Clear(Option_t * /*option*/) {
     fTrigs->Clear("C"); fNumTrigs = 0;
     fHits->Clear("C"); fNumHits = 0;
   };
   
   ClassDef(RoPetiEvent, 1)
};

#endif // ROPETIEVENT_H
