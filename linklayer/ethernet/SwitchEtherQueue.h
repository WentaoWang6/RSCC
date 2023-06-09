/*
 * SwitchEtherQueue.h
 *
 *  Created on: Mar 31, 2023
 *      Author: wwt
 */

#ifndef LINKLAYER_ETHERNET_SWITCHETHERQUEUE_H_
#define LINKLAYER_ETHERNET_SWITCHETHERQUEUE_H_

#include "inet/linklayer/ethernet/EtherFrame.h"
#include "inet/networklayer/common/L3Address.h"
#include "common/queue/BaseQueue.h"
#include <set>
#include <list>
#include <map>
#include "../../common/RateMessage_m.h"

#define FLAG_ISBOTTLE 2
#define FLAG_UNDERWATCH 1
#define FLAG_NOTBOTTLE 0

namespace mine{

struct flowItem
{
    flowItem():
        rate(0), updateTime(0), suggestTime(0), sugRate(0), flag(FLAG_ISBOTTLE){}

    int rate; //Mbps
    simtime_t updateTime;
    simtime_t suggestTime;
    int sugRate; //Mbps
    int flag;


    cOutVector estRateVector;
};

class INET_API SwitchEtherQueue : public commons::BaseQueue
{
public:
    SwitchEtherQueue();
    virtual ~SwitchEtherQueue();

protected:
    static simsignal_t queueSizeSignal;
    static simsignal_t rateSignal; // Both switch and source
    static simsignal_t rateMsgBwSignal;
    static simsignal_t markProbabilitySignal;
    static simsignal_t spotProbabilitySignal;

    virtual void initialize() override;
    virtual void handleMessage(cMessage* msg) override;

private:
    bool enablePFC;
    bool congCtrl;
    bool paused;
    int pauseThreshold; // Bytes
    int unpauseThreshold; // Bytes

    cGate* oobL3InGate;
    cGate* oobL3OutGate;

    int queueThreshold; //bytes
    int bandwidth; //Mbps
    //int fairRate; //Mbps

    int numIngress;
    int ingressRate;
    double g;
    double minAlpha;
    int fairRate; //Mbps
    int rateThreshold; //Mbps
    simtime_t timeThreshold;
    simtime_t rtt;

    std::map<inet::uint32, flowItem> mineFlowTable;

    cOutVector fairRateVector;
    cOutVector ingressRateVector;

    cMessage* clockEvent;
    void handleClockEvent();
    void setClock();

    virtual bool isFull() override;
    virtual void processIngressMessage(const cMessage* msg) override;
    virtual void processEgressMessage(const cMessage* msg) override;

    void sendPause();
    void sendUnpause();
    void emitQueueSize();
    void UpdateBufferUsage(const cMessage* msg, bool release = false);

    void updateFlowTable(const cMessage* msg);
    int getFlowRate(inet::uint32 flowId);
    bool handleRateSuggestion(inet::uint32 flowId);
    void suggestRate(inet::uint32 flowId);
    void getFairRate();
    void handleECN(const cMessage* msg);

    virtual void finish() override;



};


} //name space mine



#endif /* LINKLAYER_ETHERNET_SWITCHETHERQUEUE_H_ */
