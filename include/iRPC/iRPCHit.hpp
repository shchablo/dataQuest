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
    \brief This class defines a hit for improved Resistive Plate Chamber (iRPC).
    \author Shchablo
    \version 1.0
    \date May 2019
*/
class iRPCHit
{
    public:

        /*! \brief Constructor. */
        iRPCHit();
        /*!
            \brief Constructor.
            \param[in] channels - TDC channel.
            \param[in] strip - PCB strip.
            \param[in] time - TDC time.
        */
        iRPCHit(int channel, int strip, float time);
        /*!
            \brief Constructor.
            \param[in] channels - TDC channel.
            \param[in] strip - PCB strip.
            \param[in] time - TDC time.
            \param[in] bunchx - information about bunchx from digi.
        */
        iRPCHit(int channel, int strip, float time, int bunchx);
        /*! \brief Destructor. */
        ~iRPCHit();

        /*!
            \brief Return the bunchx of hit.
            \return bunchX.
        */
        int bx();
        /*!
            \brief Return the channel number of hit.
            \return The channal number.
        */
        int channel();
        /*!
            \brief Return the strip number of hit.
            \return The strip number.
        */
        int strip();
        /*!
            \brief Return the time value of hit.
            \return the time.
        */
        float time();

        /*!
            \brief Sets marker that the hit related to the high radius.
            \param[in] is - marker (true or flase).
        */
        void setHR(bool is);
        /*!
            \brief Sets marker that the hit related to the low radius.
            \param[in] is - marker (true or flase).
        */
        void setLR(bool is);

        /*!
            \brief Return marker that the hit related to the high radius.
            \return _isHR.
        */
        bool isHR();
        /*!
            \brief Return marker that the hit related to the low radius.
            \return _isLR.
        */
        bool isLR();

    private:

        bool _isHR; // !< marker. If true - hit from high radius.
        bool _isLR; // !< marker. If true - hit from low radius.
        int _channel; // !< TDC channel number.
        int _strip; // !< PCB strip number.
        float _time; // !> TDC time.
        float _bunchx; // !> TDC time.
};
#endif // RecoLocalMuon_iRPCHit_h
