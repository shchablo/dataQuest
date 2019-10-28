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

/* iRPC */
#include "iRPCInfo.hpp"
#include "iRPCHit.hpp"
#include "iRPCHitContainer.hpp"

/*!
    \brief This class defines a cluster for improved Resistive Plate Chamber (iRPC).
    \author Shchablo
    \version 1.0
    \date May 2019
*/
class iRPCCluster
{
    public:

        /*! \brief Constructor. */
        iRPCCluster();
        /*!
            \brief Constructor.
            \param[in] bx - bunchx.
        */
        iRPCCluster(int bx);

        /*! \brief Destructor. */
        ~iRPCCluster();

        /*!
            \brief Return bunchx.
            \return bunchx.
        */
        int bx();
        /*!
            \brief Set bunchx information.
            \param[in] bx - bunchx.
        */
        void setBx(int bx);

        /*!
            \brief Return the first (order to the map) strip of the cluster which was fired at least from one side.
            \return First strip of the cluster according to the mapping of strips.
        */
        int firstStrip();
        /*!
            \brief The last (order to the map) strip of the cluster which was fired at least from one side.
            \return Last strip of the cluster according to the mapping of strips.
        */
        int lastStrip();
        /*!
            \brief Calculate the cluster size of cluster.
            \return The number of strips which were fired at least from one side.
        */
        int clusterSize();

        /*!
            \brief Check of the existing information of high time.
            \return \f$if(nHighTime<=0) \ than \ false\f$
        */
        bool hasHighTime();
        /*!
            \brief Return the average time from high radius of the chamber.
            \return \f$HT=\frac{1}{n}\sum_{i=1}^nht_i=\frac{ht_1+ht_2+\cdots+ht_n}{n}\f$
        */
        float highTime();
        /*!
            \brief Calculate the root means squere of time from high radius of the chamber.
            \return \f$HT_{RMS}=\sqrt{\frac{ht_1^2+ht_2^2+\ldots+ht_n^2}{n}}\f$
        */
        float highTimeRMS();

        /*!
            \brief Check of the existing information of low time.
            \return \f$if(nLowTime<=0) \ than \ false\f$
        */
        bool hasLowTime();
        /*!
            \brief Return the average time from low radius of the chamber.
            \return \f$LT=\frac{1}{n}\sum_{i=1}^nlt_i=\frac{lt_1+lt_2+\cdots+lt_n}{n}\f$
        */
        float lowTime();
        /*!
            \brief Calculate the root means squere of time from low radius of the chamber.
            \return \f$LT_{RMS}=\sqrt{\frac{lt_1^2+lt_2^2+\ldots+lt_n^2}{n}}\f$
        */
        float lowTimeRMS();

        /*!
            \brief Check of the existing information of delta time.
            \return \f$if(nDeltaTime<=0) \ than \ false\f$
        */
        bool hasDeltaTime();
        /*!
            \brief Return the different averege times between two ends of the chamber.
            \return \f$DT=HT-LT=\frac{1}{n}\sum_{i=1}^nht_i-\frac{1}{n}\sum_{i=1}^nlt_i\f$
        */
        float deltaTime();
        /*!
            \brief Calculate the root means square (RMS) of differences times between two ends of the chamber.
            \return \f$DT_{RMS}=\sqrt{\frac{dt_1^2+dt_2^2+\ldots+dt_n^2}{n}}\f$
        */
        float deltaTimeRMS();

        /*!
            \brief Check of the existing information of position along-strip.
            \return \f$if(nY<=0) \ than \ false\f$
        */
        bool hasY();
        /*!
            \brief Calculate the y position.
            \return Position along-strip (mm).
        */
        float y();
        /*!
            \brief Calculate the root means square (RMS) of y position.
            \return \f$Y_{RMS}=\sqrt{\frac{y_1^2+ y_2^2+\ldots+y_n^2}{n}}\f$
        */
        float yRMS();

        /*!
            \brief Check of the existing information of strip position.
            \return \f$if(fstrip==-1 \ || \ lstrip==-1) \ than \ false\f$
        */
        bool hasX();
        /*!
            \brief Calculate the strip position (x).
            \return \f$(lstrip+fstrip)/2\f$
        */
        float x();
        /*!
            \brief Calculate the dispersion of x position.
            \return \f${xD}\left[X\right]=\frac{(lstrip-fstrip)^2}{12}\f$
        */
        float xD();

        /*!
            \brief Returns a pointer to the hit container.
            \return The container of hits which include in the cluster.
        */
        iRPCHitContainer* hits();

        /*!
            \brief Compute all parametors of cluster.
            \param[in] info -  All parameters for compute cluster.
            \return true if computed. false if something wrong.
        */
        bool compute(iRPCInfo &info);
        /*!
            \brief Fills the cluster and calculates important parameters for cluster algorithms.
            \param[in] hr - Cluster from high radius of chamber.
            \param[in] lr - Cluster from low radius of chamber.
        */
        void  initialize(iRPCCluster &hr, iRPCCluster &lr);

        /*!
            \brief Add hit to the container of the cluster and calculates important parameters for cluster algorithms.
            \param[in] hit - signle hit (iRPCHit) from raw data.
        */
        void addHit(iRPCHit &hit);
        
        /*!
            \brief Split existing cluster by two along the border of the strip.
            \param[in] strip - strip number for splitting. 
            \param[out] cluster - new cluster. 
            \return true if splitted. false if something wrong.
        */
        bool split(iRPCCluster *cluster, int strip);
        /*!
            \brief Split existing cluster by two along the border of the strip.
            \param[in] stripTop - strip number for splitting. 
            \param[out] clusterTop - new cluster. 
            \param[in] stripBot - strip number for splitting. 
            \param[out] clusterBot - new cluster. 
            \return true if splitted. false if something wrong.
        */
        bool split(iRPCCluster *clusterBot, int stripBot, 
                   iRPCCluster *clusterTop, int stripTop);

    private:

        bool _isCorrect; // !< true - true - if cluster computed correctly.
        int _bunchx; // !< digi bunch information.

        int _fstrip; // !< The first (order to the map) strip of the cluster which was fired at least from one side.
        int _lstrip; // !< The last (order to the map) strip of the cluster which was fired at least from one side.

        unsigned int _nDeltaTime; // !< The number of strips which have time information from both ends.
        float _sumDeltaTime; // !< The sum of all delta times of cluster.
        float _sumDeltaTime2; // !< The sum of squares of all delta times of cluster.

        unsigned int _nHighTime; // !< The number of strips which have time information from high radius.
        float _sumHighTime; // !< The sum of all times from high radius of cluster.
        float _sumHighTime2; // !< The sum of squares of all times from high radius of cluster.

        unsigned int _nLowTime; // !< The number of strips which have time information from low radius.
        float _sumLowTime; // !< The sum of all times from low radius of cluster.
        float _sumLowTime2; // !< The sum of squares of all times from low radius of cluster.

        unsigned int _nY; // !< The number of strips which have time information from both ends.
        float _sumY; // !< The sum of all reconstructed positions (need the speed of light from iRPCInfo).
        float _sumY2;  // !< The sum of squares of all reconstructed positions (need the speed of light from iRPCInfo).

        iRPCHitContainer _hits; // !< The container of hits which include in the cluster.
};
#endif // RecoLocalMuon_iRPCCluster_h
