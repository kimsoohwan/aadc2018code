﻿/*********************************************************************
Copyright (c) 2018
Audi Autonomous Driving Cup. All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
3.  All advertising materials mentioning features or use of this software must display the following acknowledgement: ?This product includes software developed by the Audi AG and its contributors for Audi Autonomous Driving Cup.?
4.  Neither the name of Audi nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY AUDI AG AND CONTRIBUTORS AS IS AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL AUDI AG OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

**********************************************************************/

#pragma once

#include "stdafx.h"
#include "DriverModuleWidget.h"
#include <a_utils/core/a_utils_core.h>

#define CID_CAR_CONTROLLER  "custom_driver_module.filter.user.aadc.cid"
#define LABEL_CAR_CONTROLLER  "Custom Driver Module"

class DriverModule : public QObject, virtual public cQtUIFilter
{
    Q_OBJECT

        public slots:
    /*! function which transmits the state
    * \param stateID state to be sent; -1: error, 0: Ready, 1: Running
    * \param i16ManeuverEntry current entry to be sent
    */
    tResult OnSendState(aadc::jury::stateCar stateID, tInt16 i16ManeuverEntry);

    /*! this functions loads the maneuver list given in the properties
    * \result Returns a standard result code.
    */
    tResult LoadManeuverList();

signals:
    /*! signal to the gui to show the command "run" from the jury
    * \param entryId current entry to be sent
    */
    void SendRun(int entryId);

    /*! signal to the gui to show the command "stop" from the jury
    * \param entryId current entry to be sent
    */
    void SendStop(int entryId);

    /*! signal to the gui to show the command "request ready" from the jury
    * \param entryId current entry to be sent
    */
    void SendRequestReady(int entryId);

    /*! Trigger to load maneuver list. */
    void TriggerLoadManeuverList();

public:
    ADTF_CLASS_ID_NAME(DriverModule, CID_CAR_CONTROLLER, LABEL_CAR_CONTROLLER);
    ADTF_CLASS_DEPENDENCIES(REQUIRE_INTERFACE(adtf::ui::ant::IQtXSystem),
        REQUIRE_INTERFACE(adtf::services::IReferenceClock));

private:
    /*! The property enable console output */
    property_variable<tBool> m_propEnableConsoleOutput = tFalse;

    /*! The property TCP port */
    property_variable<tInt>    m_propTCPPort = 1234;


    /*! sample writer of The output driver structure */
    cPinReader     m_oInputDriverStruct;
    /*! sample reader The input jury structure */
    cPinWriter     m_oOutputJuryStruct;
    /*! sample reader List of input maneuvers */
    cPinReader     m_oInputManeuverList;

    //Media Descriptions
    struct tJuryStructId
    {
        tSize actionId;
        tSize maneuverEntry;
    } m_ddlJuryStructId;

    /*! The jury structure sample factory */
    cSampleCodecFactory m_juryStructSampleFactory;

    struct tDriverStructId
    {
        tSize stateId;
        tSize maneuverEntry;
    } m_ddlDriverStructId;

    /*! The driver structure sample factory */
    cSampleCodecFactory m_driverStructSampleFactory;

    adtf::base::ant::runnable<> m_oInputDriverStructRunner;

    /*! ui. */
    DisplayWidgetDriver*     m_pWidget;

    /*! The maneuver file string */
    cString     m_strManeuverFileString;

    /*! this is the list with all the loaded sections from the maneuver list*/
    aadc::jury::maneuverList m_sectorList;

    /*! The reference clock */
    object_ptr<adtf::services::IReferenceClock> m_pClock;

    /*! The mutex */
    std::mutex m_oMutex;

    /*! The server socket */
    cServerSocket m_serverSocket;
    /*! The stream socket */
    cStreamSocket m_streamSocket;

    /*! The client connection established */
    tBool m_clientConnectionEstablished;

    tResult StatePassthrough(tTimeStamp tmTimeOfTrigger);
public:
    /*! Default constructor. */
    DriverModule();
    /*! Destructor. */
    virtual ~DriverModule();

protected: // Implement cBaseQtFilter
    QWidget * CreateView() override;
    tVoid    ReleaseView() override;
    tResult OnIdle() override;
    tResult  OnTimer() override;
    tResult  Init(tInitStage eStage) override;
    tResult  Shutdown(cFilterLevelmachine::tInitStage eStage) override;

    /*!
     * Constructor.
     *
     * \param [in,out]  data    The data.
     */
    ::adtf_util::cResult ReceiveTCPData(std::vector<tChar>& data);
};
