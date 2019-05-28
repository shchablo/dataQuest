#include "LyPetiEvent.hpp"

/* Constructor, destructor. */ 
LyPetiEvent::LyPetiEvent() {
}
LyPetiEvent::~LyPetiEvent() {
}
//------------------------------------------------------------------------------
/* Get Functions.
*! \fn LyPetiRead::read
* \brief Function for get number of event.
*
* \return int - event number or -1: something wrong.
*/
int LyPetiEvent::getNumOfBuffers() {
  if(_headers.size() == _payloads.size())
    return _headers.size() - 1;
  return -1;
}
//------------------------------------------------------------------------------
/* Functions. */
bool LyPetiEvent::fill(std::vector<zdaq::buffer*>* buffers) {
  
  this->clear();

  lyBuf::header header;
  lyBuf::payload payload;
  std::vector<lyBuf::payload> iPayloads;
  
  unsigned int size = buffers->size();
  _headers.resize(size+1);
  _payloads.resize(size+1);
  for(unsigned int i = 0; i < size; i++) {
	  
    uint32_t detId = buffers->at(i)->detectorId()&0xFF;
    
    if(detId == 120) {
      uint32_t* ibuf = (uint32_t*) buffers->at(i)->payload();
      
      header.number = ibuf[0];
      header.gtc = ibuf[1] & 0xFFFF;
      header.absbcID = ((uint64_t)ibuf[3] << 32) | ibuf[2];
      header.boardID = ibuf[4];
      header.boardIP[0] = ibuf[5] & 0xFF;
      header.boardIP[1] = (ibuf[5] >> 8) & 0xFF;
      header.boardIP[2] = (ibuf[5] >> 16) & 0xFF;
      header.boardIP[3] = (ibuf[5] >> 24) & 0xFF;
      header.numOfCh = ibuf[6];

      _headers.at(i) = header;

	    if(ibuf[6] > 0) {
		    uint8_t* cbuf = ( uint8_t*)&ibuf[7];
		    for(unsigned int j = 0; j < ibuf[6]; j++) {
 
          // head
          payload.head = &_headers.at(i); 

          // data
          payload.ch = cbuf[8*j + 0];
	        payload.BCID = cbuf[8*j + 2]|(((uint16_t)cbuf[8*j + 1])<<8);
	        payload.coarse = ((uint64_t)cbuf[8*j + 6])|((uint64_t)cbuf[8*j + 5]<<8)|((uint64_t)cbuf[8*j + 4]<<16)|((uint64_t)cbuf[8*j + 3]<<24); 
	        payload.fine = cbuf[8*j + 7];

          // derived parameter
	        payload.time = (payload.coarse+payload.fine/256.0)*2.5;
 
          iPayloads.push_back(payload);
	      }
        _payloads.at(i) = iPayloads;
        iPayloads.clear();
	    }
    }
    if(detId == 130) {
      uint32_t* ibuf = (uint32_t*) buffers->at(i)->payload();
      
      header.number = ibuf[0];
      header.gtc = ibuf[1];
      header.absbcID = ((uint64_t)ibuf[3] << 32) | ibuf[2];
      header.boardID = (ibuf[5]>>24) & 0xFF;
      header.boardIP[0] = ibuf[5] & 0xFF;
      header.boardIP[1] = (ibuf[5] >> 8) & 0xFF;
      header.boardIP[2] = (ibuf[5] >> 16) & 0xFF;
      header.boardIP[3] = (ibuf[5] >> 24) & 0xFF;
      header.numOfCh = ibuf[6];

      _headers.at(i) = header;

     // std::cout << "number=" << header.number << " board=" << header.boardID << " numOfCh=" << header.numOfCh << std::endl; 
	    if(ibuf[6] > 0) {
		    uint8_t* cbuf = ( uint8_t*)&ibuf[7];
		    for(unsigned int j = 0; j < ibuf[6]; j++) {
 
          // head
          payload.head = &_headers.at(i); 

          // data
          payload.ch = cbuf[6*j + 0];
	        payload.coarse = ((uint64_t)cbuf[6*j + 4])|((uint64_t)cbuf[6*j + 3]<<8)|((uint64_t)cbuf[6*j + 2]<<16)|((uint64_t)cbuf[6*j + 1]<<24); 
	        payload.fine = cbuf[6*j + 5];
	        payload.BCID = payload.coarse*2.5/200;
          // derived parameter
	        payload.time = (payload.coarse+payload.fine/256.0)*2.5;
          //std::cout << "ch=" <<  (int)payload.ch << " time=" << payload.time << std::endl; 
 
          iPayloads.push_back(payload);
	      }
        _payloads.at(i) = iPayloads;
        iPayloads.clear();
	    }
    }
  }
  // fill strips with map
  lyBuf::strip strip;
  
  strip.number = std::numeric_limits<int>::min();
    for(unsigned int i = 0; i < size; i++) {
      for(auto& it: _map) {
        strip.number = it.first;
        strip.chamber = it.second.at(7);
        bool isStrip = false;
		    for(int j = 0; j < _headers.at(i).numOfCh; j++) {
          // TRIGS
          if(_headers.at(i).boardID == (int)it.second.at(1) && _payloads.at(i).at(j).ch == (int)it.second.at(0)) {
            auto tIt = _trigs.find(it.second.at(7));
            if(tIt == _trigs.end())
              _trigs.insert(std::make_pair(it.second.at(7), std::vector<lyBuf::payload*>{1, &_payloads.at(i).at(j)}));
            bool isNew = true;
            for(unsigned int jt = 0; jt <_trigs.find(it.second.at(7))->second.size(); jt++) {
              if(_trigs.find(it.second.at(7))->second.at(jt) == &_payloads.at(i).at(j)) {
                isNew = false;
                break;
              }
            }
            if(isNew) _trigs.find(it.second.at(7))->second.push_back(&_payloads.at(i).at(j));
          }
          // trigers for strips
          if(_headers.at(i).boardID == (int)it.second.at(1) && _payloads.at(i).at(j).ch == (int)it.second.at(0))
              strip.trigs.push_back(&_payloads.at(i).at(j));
          // counter for strips
          if(_headers.at(i).boardID == (int)it.second.at(1) && _payloads.at(i).at(j).ch == (int)it.second.at(8))
              strip.counter.push_back(&_payloads.at(i).at(j));
          // HR
          if(_headers.at(i).boardID == (int)it.second.at(1) && _payloads.at(i).at(j).ch == it.second.at(2)) {
            strip.HR.push_back(&_payloads.at(i).at(j));
            isStrip = true;
          }
          // LR
          if(_headers.at(i).boardID == (int)it.second.at(1) && _payloads.at(i).at(j).ch == it.second.at(4)) { 
            strip.LR.push_back(&_payloads.at(i).at(j));
            isStrip = true;
          }
        }
        if(isStrip) {
          strip.timeOffSetHR = it.second.at(3); 
          strip.timeOffSetLR = it.second.at(5); 
          strip.timeOffSet = it.second.at(6); 
          auto sIt = _strips.find(it.second.at(7));
          if(sIt == _strips.end())
            _strips.insert(std::make_pair(it.second.at(7), std::vector<lyBuf::strip>{1, strip}));
          else
            _strips.find(it.second.at(7))->second.push_back(strip); 
        }
        strip.number = std::numeric_limits<int>::min();
        strip.HR.clear();
        strip.LR.clear();
        strip.trigs.clear();
      }
    }

    return true;
}
bool LyPetiEvent::clear() {
  _headers.clear();
  for(unsigned int i = 0; i < _payloads.size(); i++)
    _payloads.at(i).clear();
  _payloads.clear();
  for(auto& it: _strips) {
    for(unsigned i = 0; i < it.second.size(); i++) {
      it.second.at(i).HR.clear();
      it.second.at(i).LR.clear();
    }
    it.second.clear();
  }
  _strips.clear();
  for(auto& it: _trigs)
    it.second.clear();
  _trigs.clear();
  
  for(auto& it: _TDCs)
    it.second.clear();
  _TDCs.clear();
  return true;
}

bool LyPetiEvent::mapping(std::vector<int> strips, 
                          std::vector<double> trigCh, 
                          std::vector<double> TDC,
                          std::vector<double> HR, std::vector<double> timeOffSetHR,  
                          std::vector<double> LR, std::vector<double> timeOffSetLR,
                          std::vector<double> timeOffSet,
                          std::vector<double> counter,
                          std::vector<double> chamberNum) {

  std::vector<int> sizes;
  sizes.push_back(strips.size()); 
  sizes.push_back(trigCh.size()); 
  sizes.push_back(TDC.size()); 
  sizes.push_back(HR.size()); sizes.push_back(timeOffSetHR.size()); 
  sizes.push_back(LR.size());sizes.push_back(timeOffSetLR.size()); 
  sizes.push_back(timeOffSet.size()); 
  sizes.push_back(chamberNum.size()); 
  sizes.push_back(counter.size()); 
  if(!(count(sizes.begin(), sizes.end(), sizes[0]) == (int)sizes.size()))
    return false;
  std::vector<double> value;  
  for(unsigned int i = 0; i < strips.size(); i++) {
    value.push_back(trigCh.at(i)); // 0
    value.push_back(TDC.at(i)); // 1
    value.push_back(HR.at(i)); // 2
    value.push_back(timeOffSetHR.at(i)); // 3
    value.push_back(LR.at(i)); // 4
    value.push_back(timeOffSetLR.at(i)); // 5
    value.push_back(timeOffSet.at(i)); // 6
    value.push_back(chamberNum.at(i)); // 7
    value.push_back(counter.at(i)); // 8
    _map.insert(std::make_pair(strips.at(i) + chamberNum.at(i), value));
    value.clear();
  }

  // find number of TDC form map
  for(unsigned int i = 0; i < chamberNum.size(); i++) {
    auto it = _TDCs.find(chamberNum.at(i));
    if(it == _TDCs.end())
      _TDCs.insert(std::make_pair(chamberNum.at(i), std::vector<int>{1, (int)TDC.at(i)}));
    else
      _TDCs.find(chamberNum.at(i))->second.push_back((int)TDC.at(i));
  }
  // number of chambers
  std::sort(chamberNum.begin(), chamberNum.end());
  auto lastChamberNum = std::unique(chamberNum.begin(), chamberNum.end());
  chamberNum.erase(lastChamberNum, chamberNum.end());
  for(unsigned int i = 0; i < chamberNum.size(); i++)
    _chambers.push_back((int)chamberNum.at(i));
  // TDCs for chambers form map
  for(unsigned int i = 0; i < chamberNum.size(); i++) {
    std::sort(_TDCs.find(chamberNum.at(i))->second.begin(), _TDCs.find(chamberNum.at(i))->second.end());
    auto lastTDCs = std::unique(_TDCs.find(chamberNum.at(i))->second.begin(), _TDCs.find(chamberNum.at(i))->second.end());
    _TDCs.find(chamberNum.at(i))->second.erase(lastTDCs, _TDCs.find(chamberNum.at(i))->second.end());
  }

  return true;
}

bool LyPetiEvent::check() {
  return true;
}

lyBuf::header* LyPetiEvent::header(int index) {
  return &_headers.at(index);
}
std::vector<lyBuf::payload>* LyPetiEvent::payloads(int index) {
  return &_payloads.at(index);
}
std::vector<lyBuf::strip>* LyPetiEvent::strips(int chamberNum) {
  auto it = _strips.find(chamberNum);
    if(it == _strips.end()) {
      _strips.insert(std::make_pair(chamberNum, std::vector<lyBuf::strip>{0}));
    }
    return &_strips.find(chamberNum)->second; 
}
std::vector<lyBuf::payload*>* LyPetiEvent::trigs(int chamberNum) {
  auto it = _trigs.find(chamberNum);
    if(it == _trigs.end()) {
      _trigs.insert(std::make_pair(chamberNum, std::vector<lyBuf::payload*>{0}));
      _trigs.find(chamberNum)->second.clear();
    }
  return &_trigs.find(chamberNum)->second; 
}
std::vector<int>* LyPetiEvent::TDCs(int chamberNum) 
{
  auto it = _TDCs.find(chamberNum);
    if(it == _TDCs.end()) {
      _TDCs.insert(std::make_pair(chamberNum, std::vector<int>{0}));
    }
    return &_TDCs.find(chamberNum)->second; 
}
int LyPetiEvent::numberOfStrips(int chamberNum) 
{
  int size = 0;
  for(auto& it: _map) {
    if(it.second.at(7) == chamberNum)
      size += 1;
  }
    return size; 
}

bool LyPetiEvent::setTimeOffSet(std::map<int, double>* offsets) 
{
  auto itOffSet = offsets->find(0);
  for(auto& it: _map) {
    itOffSet = offsets->find(it.first);
      if(itOffSet != offsets->end())
        it.second.at(6) = it.second.at(6) + offsets->find(it.first)->second;
  }
    return true; 
}

bool LyPetiEvent::setTimeOffSetHR(std::map<int, double>* offsets) 
{
  auto itOffSet = offsets->find(0);
  for(auto& it: _map) {
    itOffSet = offsets->find(it.first);
      if(itOffSet != offsets->end())
        it.second.at(3) = it.second.at(3) + offsets->find(it.first)->second;
  }
    return true; 
}

bool LyPetiEvent::setTimeOffSetLR(std::map<int, double>* offsets) 
{
  auto itOffSet = offsets->find(0);
  for(auto& it: _map) {
    itOffSet = offsets->find(it.first);
      if(itOffSet != offsets->end())
        it.second.at(5) = it.second.at(5) + offsets->find(it.first)->second;
  }
    return true; 
}
std::vector<int>* LyPetiEvent::chambers() 
{
  return &_chambers; 
}
//------------------------------------------------------------------------------
