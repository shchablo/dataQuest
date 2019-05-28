/*!
\file 
\brief Body file with definitions of iRPCCluster functions.
\authors Shchablo Konstantin (shchablo@gmail.com)
\version 1.0
\copyright Copyright 2019 Shchablo Konstantin.
\license This file is released under the GNU General Public License v3.0.
\date May 2019
*/
#include "iRPCCluster.hpp"
#include <cmath>
#include <algorithm>
#include <limits>

iRPCCluster::iRPCCluster() 
{
  _bunchx = 0;
  _fstrip = -1; _lstrip = -1;
 		
  _dTime = -1; _nDeltaTime = 0; _sumDeltaTime = 0; _sumDeltaTime2 = 0;
  _highTime = -1; _nHighTime = 0; _sumHighTime = 0; _sumHighTime2 = 0;
  _lowTime = -1; _nLowTime = 0; _sumLowTime = 0; _sumLowTime2 = 0;
 		
  _sumY = 0; _sumY2 = 0; _nY = 0;
  
}
iRPCCluster::iRPCCluster(int bx) : _bunchx(bx) 
{
  _fstrip = -1; _lstrip = -1;
 		
  _dTime = -1; _nDeltaTime = 0; _sumDeltaTime = 0; _sumDeltaTime2 = 0;
  _highTime = -1; _nHighTime = 0; _sumHighTime = 0; _sumHighTime2 = 0;
  _lowTime = -1; _nLowTime = 0; _sumLowTime = 0; _sumLowTime2 = 0;
 		
  _sumY = 0; _sumY2 = 0; _nY = 0;
  
}
iRPCCluster::~iRPCCluster() {}

int iRPCCluster::bx() const { return _bunchx; } 

int iRPCCluster::firstStrip() const { return _fstrip; }
int iRPCCluster::lastStrip() const { return _lstrip; }
int iRPCCluster::clusterSize() const { return _lstrip-_fstrip+1; }

bool iRPCCluster::hasHighTime() const { return _nHighTime > 0; }
float iRPCCluster::highTime() const { return hasHighTime() ? _sumHighTime/_nHighTime : -1; }
float iRPCCluster::highTimeRMS() const { return hasHighTime() ? std::sqrt(std::max(0.0f, _sumHighTime2*_nHighTime - _sumHighTime*_sumHighTime))/_nHighTime : -1; }

bool iRPCCluster::hasLowTime() const { return _nLowTime > 0; }
float iRPCCluster::lowTime() const { return hasLowTime() ? _sumLowTime/_nLowTime : -1; }
float iRPCCluster::lowTimeRMS() const { return hasLowTime() ? std::sqrt(std::max(0.0f, _sumLowTime2*_nLowTime - _sumLowTime*_sumLowTime))/_nLowTime : -1; }

bool iRPCCluster::hasDeltaTime() const { return _nDeltaTime > 0; }
float iRPCCluster::deltaTime() const { return hasDeltaTime() ? _sumDeltaTime/_nDeltaTime : -1; }
float iRPCCluster::deltaTimeRMS() const { return hasDeltaTime() ? std::sqrt(std::max(0.0f, _sumDeltaTime2*_nDeltaTime - _sumDeltaTime*_sumDeltaTime))/_nDeltaTime : -1; }

bool iRPCCluster::hasY() const { return _nY > 0; }
float iRPCCluster::y() const { return hasY() ? _sumY/_nY : 0; }
float iRPCCluster::yRMS() const { return hasY() ? std::sqrt(std::max(0.0f, _sumY2*_nY - _sumY*_sumY))/_nY : -1; }

bool iRPCCluster::hasX() const { return _fstrip != -1 && _lstrip != -1; } 
float iRPCCluster::x() const { return hasX() ? (_lstrip + _fstrip)/2 : -1; }
float iRPCCluster::xD() const { return hasX() ? std::pow((_lstrip-_fstrip),2)/12 : -1; }

iRPCHitContainer* iRPCCluster::hits() { return &_hits; }    
bool iRPCCluster::compute(iRPCCluster &hr, iRPCCluster &lr)
{
  _hits.clear();
  _fstrip =  std::numeric_limits<int>::max();
  _lstrip =  std::numeric_limits<int>::min();
  for(auto hit = hr.hits()->begin(); hit != hr.hits()->end(); ++hit) {
    _hits.push_back(*hit);
    if(hit->isHR()) {
      _nHighTime += 1; _sumHighTime += hit->time(); _sumHighTime2 += hit->time()*hit->time();  
      if(_fstrip > hit->strip()) _fstrip = hit->strip();
      if(_lstrip < hit->strip()) _lstrip = hit->strip();
    }
  }
  for(auto hit = lr.hits()->begin(); hit != lr.hits()->end(); ++hit) {
    _hits.push_back(*hit);
    if(hit->isLR()) {
      _nLowTime += 1; _sumLowTime += hit->time(); _sumLowTime2 += hit->time()*hit->time();  
      if(_fstrip > hit->strip()) _fstrip = hit->strip();
      if(_lstrip < hit->strip()) _lstrip = hit->strip();
    }
  }
  float delta = 0;
  for(auto h = hr.hits()->begin(); h != hr.hits()->end(); ++h) {
    for(auto l = lr.hits()->begin(); l != lr.hits()->end(); ++l) {
      if(h->strip() == l->strip()) {
        delta = h->time() - l->time();
        _nDeltaTime += 1; _sumDeltaTime += delta; _sumDeltaTime2 += delta*delta; 
        //_nY += 1; _sumY +=; _sumHighTime2 +=; 
      }
    }
  }
  
  return true; 
}    
void iRPCCluster::addHit(iRPCHit &hit)
{
    if(_hits.empty()) { _fstrip = hit.strip(); _lstrip = hit.strip(); } 
    
    if(_fstrip > hit.strip()) _fstrip = hit.strip();
    if(_lstrip < hit.strip()) _lstrip = hit.strip();
    
    if(hit.isHR()) {
      _nHighTime += 1; _sumHighTime += hit.time(); _sumHighTime2 += hit.time()*hit.time();  
    }
    if(hit.isLR()) {
      _nLowTime += 1; _sumLowTime += hit.time(); _sumLowTime2 += hit.time()*hit.time();  
    }
    _hits.push_back(hit);
    return;
}    

