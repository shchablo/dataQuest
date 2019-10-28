/*!
\file
\brief Body file with definitions of iRPCCluster functions.
\authors Shchablo Konstantin (shchablo@gmail.com)
\version 1.0
\copyright Copyright 2019 Shchablo Konstantin.
\license This file is released under the GNU General Public License v3.0.
\date May 2019
*/

/* iRPC */
#include "iRPCCluster.hpp"
/* std */
#include <cmath>
#include <algorithm>
#include <limits>

iRPCCluster::iRPCCluster()
{
    _isCorrect = false;
    _bunchx = 0;
    _fstrip = -1; _lstrip = -1;

    _nDeltaTime = 0; _sumDeltaTime = 0; _sumDeltaTime2 = 0;
    _nHighTime = 0; _sumHighTime = 0; _sumHighTime2 = 0;
    _nLowTime = 0; _sumLowTime = 0; _sumLowTime2 = 0;

    _sumY = 0; _sumY2 = 0; _nY = 0;

}
iRPCCluster::iRPCCluster(int bx) : _bunchx(bx)
{
    _isCorrect = false;
    _fstrip = -1; _lstrip = -1;

    _nDeltaTime = 0; _sumDeltaTime = 0; _sumDeltaTime2 = 0;
    _nHighTime = 0; _sumHighTime = 0; _sumHighTime2 = 0;
    _nLowTime = 0; _sumLowTime = 0; _sumLowTime2 = 0;

    _sumY = 0; _sumY2 = 0; _nY = 0;

}
iRPCCluster::~iRPCCluster() {}

int iRPCCluster::bx() { return _bunchx; }
void iRPCCluster::setBx(int bx) { _bunchx = bx; }

int iRPCCluster::firstStrip() { return _fstrip; }
int iRPCCluster::lastStrip() { return _lstrip; }
int iRPCCluster::clusterSize() { return _lstrip-_fstrip+1; }

bool iRPCCluster::hasHighTime() { return _nHighTime > 0; }
float iRPCCluster::highTime() { return hasHighTime() ? _sumHighTime/_nHighTime : -1; }
float iRPCCluster::highTimeRMS() { return hasHighTime() ? std::sqrt(std::max(0.0f, _sumHighTime2*_nHighTime - _sumHighTime*_sumHighTime))/_nHighTime : -1; }

bool iRPCCluster::hasLowTime() { return _nLowTime > 0; }
float iRPCCluster::lowTime() { return hasLowTime() ? _sumLowTime/_nLowTime : -1; }
float iRPCCluster::lowTimeRMS() { return hasLowTime() ? std::sqrt(std::max(0.0f, _sumLowTime2*_nLowTime - _sumLowTime*_sumLowTime))/_nLowTime : -1; }

bool iRPCCluster::hasDeltaTime() { return _nDeltaTime > 0; }
float iRPCCluster::deltaTime() { return hasDeltaTime() ? _sumDeltaTime/_nDeltaTime : -1; }
float iRPCCluster::deltaTimeRMS() { return hasDeltaTime() ? std::sqrt(std::max(0.0f, _sumDeltaTime2*_nDeltaTime - _sumDeltaTime*_sumDeltaTime))/_nDeltaTime : -1000; }

bool iRPCCluster::hasY() { return _nY > 0; }
float iRPCCluster::y() { return hasY() ? _sumY/_nY : 0; }
float iRPCCluster::yRMS() { return hasY() ? std::sqrt(std::max(0.0f, _sumY2*_nY - _sumY*_sumY))/_nY : -1; }

bool iRPCCluster::hasX() { if(_fstrip == -1 || _lstrip == -1) return false; else return true; }
float iRPCCluster::x() { return hasX() ? (_lstrip + _fstrip)/2 : -1; }
float iRPCCluster::xD() { return hasX() ? std::pow((_lstrip-_fstrip),2)/12 : -1; }

iRPCHitContainer* iRPCCluster::hits() { return &_hits; }

bool iRPCCluster::compute(iRPCInfo &info)
{
    bool result = true;
    _nHighTime = 0; _sumHighTime = 0; _sumHighTime2 = 0;
    _nLowTime = 0; _sumLowTime = 0; _sumLowTime2 = 0;
    _nDeltaTime = 0; _sumDeltaTime = 0; _sumDeltaTime2 = 0;
    if(_hits.size() > 0) _bunchx = _hits.begin()->bx();
    _fstrip =  std::numeric_limits<int>::max();
    _lstrip =  std::numeric_limits<int>::min();
    for(auto hit = _hits.begin(); hit != _hits.end(); ++hit) {
        if(_bunchx != hit->bx()) {
            _bunchx = std::numeric_limits<int>::min();
            result = false;
        }
        if(_fstrip > hit->strip()) _fstrip = hit->strip();
        if(_lstrip < hit->strip()) _lstrip = hit->strip();
        if(hit->isHR()) { _nHighTime += 1; _sumHighTime += hit->time(); _sumHighTime2 += hit->time()*hit->time(); }
        if(hit->isLR()) { _nLowTime += 1; _sumLowTime += hit->time(); _sumLowTime2 += hit->time()*hit->time(); }
    }

    float delta = 0;
    float y = 0; float speed = info.speed();
    for(auto h = _hits.begin(); h != _hits.end(); ++h) {
        for(auto l = _hits.begin(); l != _hits.end(); ++l) {
            if((h->strip() == l->strip()) && h->isHR() && l->isLR()) {
                delta = h->time() - l->time();
                _nDeltaTime += 1; _sumDeltaTime += delta; _sumDeltaTime2 += delta*delta;
                y = delta*speed;
                _nY += 1; _sumY += y; _sumY2 += y*y;
                break;
            }
        }
    }
    _isCorrect = result; return result;
}

void iRPCCluster::initialize(iRPCCluster &hr, iRPCCluster &lr)
{
    _hits.clear();
    for(auto hit = hr.hits()->begin(); hit != hr.hits()->end(); ++hit) _hits.push_back(*hit);
    for(auto hit = lr.hits()->begin(); hit != lr.hits()->end(); ++hit) _hits.push_back(*hit);
}

void iRPCCluster::addHit(iRPCHit &hit) { _hits.push_back(hit); }

bool iRPCCluster::split(iRPCCluster *cluster, int strip)
{
    bool isSplit = false;
    for(auto hit = _hits.begin(); hit != _hits.end();) {
        if(hit->strip() >= strip) {
            cluster->addHit(*hit);
            hit = _hits.erase(hit);
            isSplit = true;
        }
       else
            ++hit;
    }
    return isSplit;
}

bool iRPCCluster::split(iRPCCluster *clusterBot, int stripBot, 
                        iRPCCluster *clusterTop, int stripTop)
{
    bool isSplit = false;
    for(auto hit = _hits.begin(); hit != _hits.end();) {
        if(hit->strip() >= stripTop) {
            clusterTop->addHit(*hit);
            hit = _hits.erase(hit);
            isSplit = true;
        }
        else if(hit->strip() <= stripBot) {
            clusterBot->addHit(*hit);
            hit = _hits.erase(hit);
            isSplit = true;
        }
       else
            ++hit;
    }
    return isSplit;
}
