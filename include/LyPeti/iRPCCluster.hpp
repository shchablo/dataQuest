/*!
\file 
\brief Header file with definitions of iRPCCluster class.
\authors Shchablo Konstantin (shchablo@gmail.com)
\version 1.0
\copyright Copyright 2019 Shchablo Konstantin.
\license This file is released under the GNU General Public License v3.0.
\date May 2019
*/

#ifndef RecoLocalMuon_iRPCCluster_h
#define RecoLocalMuon_iRPCCluster_h

#include "iRPCHit.hpp"
#include "iRPCHitContainer.hpp"

/*!
	\brief iRPC cluster. 
	\author Shchablo 
	\version 1.0
	\date May 2019
	This class defines a cluster for improved Resistive Plate Chamber (iRPC).
*/
class iRPCCluster
{
  public:
    
 		/*!
			\brief Constructor.
		*/
		iRPCCluster();
 		/*!
			\brief Constructor.
			\param[in] bx bunchx
		*/
		iRPCCluster(int bx);
    
 		/*!
			\brief Destructor.
		*/
		~iRPCCluster();
		
 		/*!
			\brief Return bunchx.
			\return bunchx.
		*/
		int bx() const; 
 		/*!
			\brief Return the first strip of cluster.
			\return First strip of the cluster according to the mapping of strips.
		*/
		int firstStrip() const;
 		/*!
			\brief Return the last strip of cluster from the side where were triggered more strips.
			\return Last strip of the cluster according to the mapping of strips.
		*/
 		int lastStrip() const;
 		/*!
			\brief Calculate the cluster size of cluster from the side where were triggered more strips.
			\return The number of strips from the side where were triggered more strips.
		*/
		int clusterSize() const;
		
		/*!
		  \brief Check for the existence of high time information. 
			\return \f$ if(nHighTime <= 0) than false.\f$
		*/
		bool hasHighTime() const;
 		/*!
			\brief Return the average time from high radius of the chamber.
			\return \f$HT = \frac{1}{n}\sum_{i=1}^nht_i = \frac{ht_1 + ht_2 + \cdots + ht_n}{n}.\f$
		*/
		float highTime() const;
 		/*!
			\brief Calculate the root means squere of time from high radius of the chamber.
			\return \f$HT_RMS = \sqrt{\frac {ht_1^2+ ht_2^2+ \ldots+ ht_n^2} {n}}.\f$
		*/
		float highTimeRMS() const;
		
		/*!
		  \brief Check for the existence of low time information. 
			\return \f$ if(nLowTime <= 0) than false.\f$
		*/
		bool hasLowTime() const;
 		/*!
			\brief Return the average time from low radius of the chamber.
			\return \f$LT = \frac{1}{n}\sum_{i=1}^nlt_i = \frac{lt_1 + lt_2 + \cdots + lt_n}{n}.\f$
		*/
		float lowTime() const;
 		/*!
			\brief Calculate the root means squere of time from low radius of the chamber.
			\return \f$LT_RMS = \sqrt{\frac {lt_1^2+ lt_2^2+ \ldots+ lt_n^2} {n}}.\f$
		*/
		float lowTimeRMS() const;
		
		/*!
		  \brief Check for the existence of delta time information. 
			\return \f$ if(nDeltaTime <= 0) than false.\f$
		*/
		bool hasDeltaTime() const;
		/*!
			\brief Return the different averege times between two ends of the chamber.
			\return \f DT = $HT - LT = \frac{1}{n}\sum_{i=1}^nht_i - \frac{1}{n}\sum_{i=1}^nlt_i\f$
		*/
		float deltaTime() const;
 		/*!
		  \brief Calculate the root means square (RMS) of differences times 
			between two ends of the chamber. 
			\return \f$DT_RMS = \sqrt{\frac {dt_1^2+ dt_2^2+ \ldots+ dt_n^2} {n}}.\f$
		*/
		float deltaTimeRMS() const;
 		
		/*!
		  \brief Check for the existence of y information. 
			\return \f$ if(nY <= 0) than false.\f$
		*/
		bool hasY() const;
		/*!
		  \brief Calculate the y position. 
			\return \f\f$
		*/
		float y() const;
		/*!
		  \brief Calculate the root means square (RMS) of y position. 
			\return \f\f$
		*/
		float yRMS() const;
		/*!
		  \brief Check for the existence of x information. 
			\return \f$ if(fstrip == -1 & lstrip == -1 ) than false;.\f$
		*/
		bool hasX() const;
		/*!
		  \brief Calculate the x position. 
			\return \f\f$
		*/
		float x() const;
		/*!
		  \brief Calculate the dispersion of y position. 
			\return \f\f$
		*/
		float xD() const;
	
    iRPCHitContainer* hits();
    bool compute(iRPCCluster &hr, iRPCCluster &lr);    
    void addHit(iRPCHit &hit);    
	
	private:
		
		int _bunchx;
		
		int _fstrip; int _lstrip;
 		
 		float _dTime; unsigned int _nDeltaTime; float _sumDeltaTime; float _sumDeltaTime2;
 		float _highTime; unsigned int _nHighTime; float _sumHighTime; float _sumHighTime2;
 		float _lowTime; unsigned  int _nLowTime; float _sumLowTime; float _sumLowTime2;
 		
 		float _sumY; float _sumY2; unsigned int _nY;

    iRPCHitContainer _hits;

};
#endif // RecoLocalMuon_iRPCCluster_h 
