#ifndef LYPETIEVENT_h
#define LYPETIEVENT_h 

#include <map>
#include <vector>
#include <algorithm>

#include <fcntl.h>
#include <zlib.h>

#include "zmBuffer.hpp"

namespace lyBuf {

struct header
{
	int number;
	int gtc;
	int absbcID;
	int boardID;
	int boardIP[4];
	int numOfCh;
};      

struct payload
{

  header* head;
//  payload* trig;

	// default
	uint8_t ch;
	uint16_t BCID;
	uint64_t coarse; 
	uint8_t fine;
	
	// derived parameter
	double time;
 
};      
struct strip 
{
	int chamber;
  int number; /* -10000 - something wrong with number of strip;
                 -[value] - trig strip (same like boardID) */
  std::vector<payload*> HR;
  std::vector<payload*> LR;
  std::vector<payload*> trigs;
  
  double deadHR;
  double deadLR;
  
  double timeOffSet;
  double timeOffSetHR;
  double timeOffSetLR;
};
}
class LyPetiEvent
{
public:
  /* Constructor, destructor. */ 
  LyPetiEvent();
  virtual ~LyPetiEvent();
  //----------------------------------------------------------------------------
  
  /* Get Functions. */
  int getNumOfBuffers();
  //----------------------------------------------------------------------------

  bool setTimeOffSetHR(std::map<int, double>* offsets); 
  bool setTimeOffSetLR(std::map<int, double>* offsets); 
  bool setTimeOffSet(std::map<int, double>* offsets); 
  std::map<int, double> getTimeOffSetHR(); 
  std::map<int, double> getTimeOffSetLR();
  std::vector<int> getDeadCHsHR(); 
  std::vector<int> getDeadCHsLR(); 
  /* Functions. */
  bool fill(std::vector<zdaq::buffer*>* buffers);
  bool clear();
  bool check();
  bool mapping(std::vector<int> strips, 
               std::vector<double> trigCh, 
               std::vector<double> TDC,
               std::vector<double> HR, std::vector<double> timeOffSetHR,  
               std::vector<double> LR, std::vector<double> timeOffSetLR,
               std::vector<double> timeOffSet,
               std::vector<double> deadHR,
               std::vector<double> deadLR,
               std::vector<double> chamberNum);

  lyBuf::header* header(int index);
  std::vector<lyBuf::payload>* payloads(int index);
  std::vector<lyBuf::strip>* strips(int chamberNum);
  std::vector<lyBuf::payload*>* trigs(int chamberNum);
  std::vector<int>* TDCs(int chamberNum);
  std::vector<int>* chambers();
  int numberOfStrips(int chamberNum); 

  //----------------------------------------------------------------------------
  
protected:

  std::vector<int> _chambers; // payloads of boards (playload)  
  std::map<int, std::vector<int>> _TDCs; // payloads of boards (playload)  
  std::vector<lyBuf::header> _headers;  // headers of boards
  std::vector<std::vector<lyBuf::payload>> _payloads; // payloads of boards (playload)  
  std::map<int, std::vector<lyBuf::strip>> _strips; // payloads of boards (playload)  
  std::map<int, std::vector<lyBuf::payload*>> _trigs; // payloads of boards (playload)  
  int _trigCh; // payloads of boards (playload)  

  std::map<int, std::vector<double>> _map; // key - number of strip
                              // int[0] - trigCh;
                              // int[1] - TDC;
                              // int[2] - ch for HR;
                              // int[3] - HR time offset;
                              // int[4] - ch for LR;
                              // int[5] - LR time offset;
                              // int[6] - timeOffset;
                              // int[7] - chamber;
                              // int[8] - deadHR;
                              // int[9] - deadLR;
};
#endif 
