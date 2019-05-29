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
	\brief In this class defined functions to fulfillment clustering. 
	\author Shchablo (IPNL-France) 
	\version 1.0
	\date May 2019
*/
/* iRPC */
#include "iRPCClusterContainer.hpp"
#include "iRPCCluster.hpp"
#include "iRPCInfo.hpp" // dataQuest
/* std */
#include <vector>
#include <utility>

/* CMSSW */
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
  	
 		/*!
			\brief The function of clustering hits from a single side of the chamber.
			\param[in] thrTime - The threshold for time clustering between two hits (Chain checking of hits).
			\param[out] clusters - Container of output clusters.
			\return Fulfillment status.
		*/
		bool clustering(float thrTime, iRPCHitContainer &hits, iRPCClusterContainer &clusters);
		
 		/*!
			\brief The function of the association of clusters from high radius and low radius radius.
			\param[in] isAND - if true return only associated clusters. if false associated&singleSide clusters. 
			\param[in] thrDeltaMin - Minimum time for difference between high and low radius. Related with the geometry of the chamber. 
			\param[in] thrDeltaMax - Maximum time for difference between high and low radius. Related with the geometry of the chamber. 
			\param[in] hr - Container of clusters correspond high radius of the chamber.
			\param[in] lr - Container of clusters correspond low radius of the chamber.
			\return Container of clusters.
		*/
    iRPCClusterContainer association(bool isAND, float thrDeltaMin,  float thrDeltaMax,
                                     iRPCClusterContainer hr, iRPCClusterContainer lr); 
		
    /* CMSSW */
 		/*!
			\brief The action function.
			\param[in] digiRange - simulated data.
			\return Container of clusters.
		*/
		// iRPCClusterContainer doAction(const RPCDigiCollection::Range& digiRange);
		
		/* dataQuest */
 		/*!
			\brief The action function.
			\param[in] dataHR - Raw data correspond HR.
			\param[in] dataLR - Raw data correspond LR.
			\param[in] info - Parameters for cluster algorithms.
			\return Container of clusters.
		*/
		iRPCClusterContainer doAction(std::vector<std::pair<double, double>> dataHR, 
																	std::vector<std::pair<double, double>> dataLR, 
                                  iRPCInfo info);

};

#endif // RecoLocalMuon_iRPCClusterClusterizer_h 
