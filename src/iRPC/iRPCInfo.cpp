/*!
\file
\brief Body file with definitions of iRPCInfo functions.
\authors Shchablo Konstantin (shchablo@gmail.com)
\version 1.0
\copyright Copyright 2019 Shchablo Konstantin.
\license This file is released under the GNU General Public License v3.0.
\date May 2019
*/
#include "iRPCInfo.hpp"

/* std */
#include <limits>

iRPCInfo::iRPCInfo()
{
    _isAND = true;
    _isUse = true;
    _speed = 19.786302;
    _thrTimeHR = std::numeric_limits<float>::max();
    _thrTimeLR = std::numeric_limits<float>::max();
    _thrDeltaTimeMin = std::numeric_limits<float>::min();
    _thrDeltaTimeMax = std::numeric_limits<float>::max();
    _count = 0;
}

iRPCInfo::~iRPCInfo()
{
}

bool iRPCInfo::isHR(int channel, int &strip)
{
    auto it = _HR.find(channel);
    if(it == _HR.end()) return false;
    strip = it->second;
    return true;
}
bool iRPCInfo::isLR(int channel, int &strip)
{
    auto it = _LR.find(channel);
    if(it == _LR.end()) return false;
    strip = it->second;
    return true;
}

void iRPCInfo::setMapHR(std::map<int, int> &map) { _HR = map; }
void iRPCInfo::setMapLR(std::map<int, int> &map) { _LR = map; }

std::map<int, int> iRPCInfo::getMapHR() { return _HR; }
std::map<int, int> iRPCInfo::getMapLR() {return _LR; }

float iRPCInfo::thrTimeHR() { return _thrTimeHR; }
float iRPCInfo::thrTimeLR() { return _thrTimeLR; }

void iRPCInfo::setThrTimeHR(float thrTime) { _thrTimeHR = thrTime; }
void iRPCInfo::setThrTimeLR(float thrTime) { _thrTimeLR = thrTime; }

void iRPCInfo::setThrDeltaTimeMin(float thrTime) { _thrDeltaTimeMin = thrTime; }
void iRPCInfo::setThrDeltaTimeMax(float thrTime) { _thrDeltaTimeMax = thrTime; }
void iRPCInfo::setThrDeltaTimeY(float thrTime) { _thrDeltaTimeY = thrTime; }

float iRPCInfo::thrDeltaTimeMin() { return _thrDeltaTimeMin; }
float iRPCInfo::thrDeltaTimeMax() { return _thrDeltaTimeMax; }
float iRPCInfo::thrDeltaTimeY() { return _thrDeltaTimeY; }

float iRPCInfo::speed() { return _speed; }
void iRPCInfo::setSpeed(float speed) { _speed = speed; }

bool iRPCInfo::isReturnOnlyAND() { return _isAND; }
void iRPCInfo::isReturnOnlyAND(bool is) { _isAND = is; };

bool iRPCInfo::isUseIRPCAlgorithm() { return _isUse; }
void iRPCInfo::isUseIRPCAlgorithm(bool is) { _isUse = is; };

int iRPCInfo::counter() { _count = _count + 1; return _count; };
