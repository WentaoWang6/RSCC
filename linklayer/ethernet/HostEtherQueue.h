/*
 * HostEtherQueue.h
 *
 *  Created on: Mar 31, 2023
 *      Author: wwt
 */

#ifndef LINKLAYER_ETHERNET_HOSTETHERQUEUE_H_
#define LINKLAYER_ETHERNET_HOSTETHERQUEUE_H_

#include "inet/linklayer/ethernet/EtherFrame.h"
#include "inet/networklayer/common/L3Address.h"
#include "inet/applications/udpapp/UDPReliableAppPacket_m.h"
#include "inet/applications/udpapp/UDPReliableAppPacketAck_m.h"
#include "common/queue/BaseQueue.h"
#include "common/ThruputMeter.h"
#include <map>
#include <list>
#include "../../common/AckMessage_m.h"
#include "../../common/RateMessage_m.h"
#include "../../common/CNPMessage_m.h"

namespace mine{

class Channel
{
    struct AckPendingMsg
        {
            enum Status { ACK_PENDING = 0, RESEND };
            inet::uint32 seqNumber;
            cMessage* msg;
            Status status;
        };
    typedef std::map<inet::uint32, AckPendingMsg*> AckPendingMsgs;

    bool systemChannel;
    inet::uint32 flowId;
    bool enableLossRecovery;
    int capacity;
    int rtt;

    int maxRate; //maybe bandwidth
    bool rateLimit;
    int currRate;

    /* -------for mine -------*/
    simtime_t timeThreshold;
    simtime_t acceptSuggestionTime;
    simtime_t lastECNTime;
    int oldAcceptSuggestion;

    int deltRate;

    /*-----------------------*/


    inet::uint32 nextSequence;
    double bitsPerInterval;
    double bitsToSend;

    int depth;
    int numReceived;
    int numDropped;
    int numSent;
    int numResent;
    int numAckReceived;
    int numNackReceived;
    int rateMsgCount;
    int cnpMsgCount;



    inet::uint32 lastAckSeq;
    inet::uint32 prevLastAckSeq; // lastAckSeq at previous timeout
    int timeoutCount;
    int ackTimeoutTickCounter;
    int selfRiseTickCounter;

    cQueue queue;
    cOutVector effectiveRateVector;
    cOutVector rateVector;
    cOutVector ecnVector;
    commons::ThruputMeter thruputMeter;

    AckPendingMsgs ackPendingMsgs;
    inet::UDPReliableAppPacketAck* lastPktAck;
    std::stringstream ss;

    Channel(inet::uint32 flwId, bool enableLossRec, int channelCapacity, int rtt, int maxRate);
    ~Channel();

    void initialize();
    void handleRateMessage(RateMessage* rateMsg); // Received from switch
    void handleAckMessage(AckMessage* ackMsg); // Received from destination
    void handleCnpMessage(CNPMessage* cnpMsg); //Received from destination
    void onAckReceived(AckMessage* ackMsg);

    cMessage* addMessage(cMessage* msg);
    cMessage* getMessage();

    void onRateLimitTimer();
    void onAckClockTimer();
    void onSelfRiseTimer();

    void adjustRate(int rate);

    static inet::uint32 getSequence(cMessage* msg);
    static inet::UDPReliableAppPacket* getAppPkt(omnetpp::cMessage* msg);

    int getPendingResendCount();

    friend class HostEtherQueue;

};

class INET_API HostEtherQueue : public commons::BaseQueue
{
public:
    HostEtherQueue();
    virtual ~HostEtherQueue();

protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage* msg) override;
    virtual void requestPacket() override;

private:
    struct RateMsgHolder
        {
            int propTickCounter;
            RateMessage* rateMsg;
        };
    typedef std::map<inet::uint32, Channel*> Channels; // By flow

    bool enableLossRecovery;
    int channelCapacity;
    int rtt;
    int maxRate; //bandwidth Mbps

    int nextChannel;
    int cnpMsgCount;

    int rateLimitTickCounter;
    int ackClockTickCounter;

    Channels channels;
    std::list<RateMsgHolder*> pendingRateMsgs;

    cGate* oobL3InGate;
    cGate* oobL3OutGate;

    cMessage* clockEvent;

    void handleClockEvent();
    void handleRateMessage(RateMessage* rateMsg); // Received from remote switch
    void handleCnpMessage(CNPMessage* cnpMsg);
    void handleMessageEx(cMessage* msg);
    void sendApplicationPacketAck(cMessage* msg, bool ack);
    inet::UDPReliableAppPacketAck* getLastPktAck(cMessage* msg);

    virtual cMessage* enqueue(cMessage* msg) override;
    virtual cMessage* dequeue() override;

    cMessage* addToChannel(cMessage* msg);

    void setClock();
    void onRateLimitTimer();
    void onAckClockTimer();

    void onSelfRiseTimer();
    void onRateMsgPropTimer();

    void sendPackets(bool stale = false);

    int getQueueSize();
    bool hasNewMsgs();

    virtual bool isEmpty() override;
    virtual bool isFull() override;
    virtual void finish() override;

};



} // name space mine


#endif /* LINKLAYER_ETHERNET_HOSTETHERQUEUE_H_ */
