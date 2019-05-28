/*!
\file 
\brief File with definitions iRPCClusterizer class.
\authors Shchablo Konstantin (IPNL-France) (shchablo@gmail.com)
\version 1.0
\copyright Copyright 2019 Shchablo Konstantin.
\license This file is released under the GNU General Public License v3.0.
\date May 2019
*/

#ifndef RecoLocalMuon_iRPCClusterizer_h
#define RecoLocalMuon_iRPCClusterizer_h

/*!
	\brief Class to fulfillment clustering. 
	\author Shchablo (IPNL-France) 
	\version 1.0
	\date May 2019
	in this class defined functions to fulfillment clustering.
*/
#include "iRPCClusterContainer.hpp"
#include "iRPCCluster.hpp"
#include "iRPCInfo.hpp"
#include <vector>
#include <utility>

// CMSSW
//#include "DataFormats/RPCDigi/interface/RPCDigiCollection.h"

class iRPCClusterizer {
 
	public:

 		/*
			\brief Constructor.
		*/
  	iRPCClusterizer();
 		/*!
			\brief Destructor.
		*/
  	~iRPCClusterizer();
  	
		// CMSSW
		// iRPCClusterContainer doAction(const RPCDigiCollection::Range& digiRange);
		
		// dataQuest
 		/*!
			\brief  Initialization function.
			\param[in] dataHR Raw data correspond HR.
			\param[in] dataLR Raw data correspond LR.
			\return Container of clusters.
		*/
		iRPCClusterContainer doAction(std::vector<std::pair<double, double>> dataHR, 
																	std::vector<std::pair<double, double>> dataLR, 
                                  iRPCInfo info);
		
		bool clustering(float thrTime, iRPCHitContainer &hits, iRPCClusterContainer &clusters);
		iRPCClusterContainer association(iRPCInfo &info, iRPCClusterContainer hr, iRPCClusterContainer lr); 

};

#endif // RecoLocalMuon_iRPCClusterClusterizer_h 
