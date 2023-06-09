/*
 * L3RelayUnitEx.h
 *
 *  Created on: Mar 31, 2023
 *      Author: wwt
 */

#ifndef NETWORKLAYER_IPV4_L3RELAYUNITEX_H_
#define NETWORKLAYER_IPV4_L3RELAYUNITEX_H_

#include "common/networklayer/ipv4/L3RelayUnit.h"
#include <map>
#include <set>
#include "ReceiveHostAdaptor.h"

const static int CNP_INTERVAL = 50000;//ns

namespace mine{

class INET_API L3RelayUnitEx : public commons::L3RelayUnit
{
    struct IngressPort
    {
        IngressPort(int pn):
            portNum(pn),
            paused(false),
            numPauseFramesSent(0),
            numUnpauseFramesSent(0),
            buffSize(0)
        {}

        void initialize()
        {
            std::string portName = "ingressPort-";
            portName += std::to_string(portNum);

            createWatch((portName + "-numPauseFramesSent").c_str(), numPauseFramesSent);
            createWatch((portName + "-numUnpauseFramesSent").c_str(), numUnpauseFramesSent);
            createWatch((portName + "-ingressQueueSize").c_str(), buffSize); // Virtual ingress queue
        }

        void updateBuffSize(int buffUsage)
        {
            buffSize += buffUsage;
        }

        bool pause(const int& pauseTh) const
        {
            return !paused && (pauseTh <= buffSize);
        }

        bool unpause(const int& unpauseTh) const
        {
            return paused && (buffSize <= unpauseTh);
        }

        // Configuration
        int portNum;

        // State
        bool paused;
        int numPauseFramesSent;
        int numUnpauseFramesSent;
        int buffSize;
    };

  public:
    L3RelayUnitEx();
    virtual ~L3RelayUnitEx();

  protected:
    // Statistics    RoCC
    static simsignal_t rateMsgFanoutSignal;

    virtual void initialize() override;
    virtual void handleMessage(cMessage* msg) override;// dcqcn?????

    virtual void handleMessageIcmp(cMessage* msg) override;
    virtual void handleMessageIf(cMessage* msg) override;
    virtual void handleMessageIfOob(cMessage* msg) override;

    void sendPause(IngressPort* port, bool pause = true);

  private:
    // Configuration
    bool enableLossRecovery;
    bool enablePFC;
    bool destination; // for ECN
    int pauseThreshold; // Bytes
    int unpauseThreshold; // Bytes

    int numAckSent;
    int numNackSent;
    int numDropped;

    ReceiveHostAdaptor hostAdaptor;

    /* for ECN */
    int ecnCount;
    int cnpCount;
    std::set<inet::uint32> cnpSentFlows;

    cMessage* clockEvent;
    void handleClockEvent();
    void setClock();

    void handleEcnMarkedPacket(const cPacket* packet);

    /*---------*/

    std::map<int, IngressPort*> ingressPorts;

    std::stringstream ss;

    virtual void finish() override;
};

}// name space mine



#endif /* NETWORKLAYER_IPV4_L3RELAYUNITEX_H_ */
