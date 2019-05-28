/*!
\file 
\brief Header file with definitions of iRPCHit class.
\authors Shchablo Konstantin (shchablo@gmail.com)
\version 1.0
\copyright Copyright 2019 Shchablo Konstantin.
\license This file is released under the GNU General Public License v3.0.
\date May 2019
*/

#ifndef RecoLocalMuon_iRPCHit_h
#define RecoLocalMuon_iRPCHit_h

/*!
	\brief iRPC hit. 
	\author Shchablo 
	\version 1.0
	\date May 2019
	This class defines a hit for improved Resistive Plate Chamber (iRPC).
*/
class iRPCHit
{
  public:
    
 		/*
			\brief Constructor.
		*/
		iRPCHit();
		iRPCHit(int channel, int strip, float time); 
		iRPCHit(int channel, int strip, float time, int bunchx); 
 		/*!
			\brief Destructor.
		*/
		~iRPCHit();
    
		/*!
			\brief Return the channel number of hit.
			\return The channal number.
		*/
		int channel();	
 		/*!
			\brief Return the strip of hit.
			\return The strip number.
		*/
		int strip();
 		/*!
			\brief Return the time of hit.
			\return the time..
		*/
		float time();
		
		void setHR(bool is);
		void setLR(bool is);
		bool isHR();
		bool isLR();
				
	private:
	
		bool _isHR;	bool _isLR;	
		int _channel;
		int _strip;
		float _time;		
};
#endif // RecoLocalMuon_iRPCHit_h 
