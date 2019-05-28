/*!
\file 
\brief Header file with definitions of iRPCMap class.
\authors Shchablo Konstantin (shchablo@gmail.com)
\version 1.0
\copyright Copyright 2019 Shchablo Konstantin.
\license This file is released under the GNU General Public License v3.0.
\date May 2019
*/

#ifndef RecoLocalMuon_iRPCInfo_h
#define RecoLocalMuon_iRPCInfo_h

#include <map>

/*!
	\brief IRPCInfo. 
	\author Shchablo 
	\version 1.0
	\date May 2019
	This class defines a association map for improved Resistive Plate Chamber (iRPC).
*/
class iRPCInfo
{
  public:
    
 		/*
			\brief Constructor.
		*/
		iRPCInfo();
 		/*!
			\brief Destructor.
		*/
		~iRPCInfo();
    
    bool isLR(int channel, int &strip);
    bool isHR(int channel, int &strip);
    
    void setMapHR(std::map<int, int> &map);
    void setMapLR(std::map<int, int> &map);
    
    std::map<int, int> getMapHR();
    std::map<int, int> getMapLR();

    float thrTimeHR();
    float thrTimeLR();
    
    void setThrTimeHR(float thrTime);
    void setThrTimeLR(float thrTime);
    
    float thrDeltaTimeMin();
    float thrDeltaTimeMax();
    
    void setThrDeltaTimeMin(float thrTime);
    void setThrDeltaTimeMax(float thrTime);

    float speed();
    void setSpeed(float _speed);
    
    bool isAND();
    void isAND(bool is);

	private:
    std::map<int, int> _HR;
    std::map<int, int> _LR;

    float _thrTimeHR;
    float _thrTimeLR;
    
    float _thrDeltaTimeMin;
    float _thrDeltaTimeMax;
    
    float _speed;
    
    bool _isAND;
};
#endif // RecoLocalMuon_iRPCInfo_h 
