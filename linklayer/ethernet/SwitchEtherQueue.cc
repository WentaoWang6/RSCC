/*
 * SwitchEtherQueue.cc
 *
 *  Created on: 2023年4月9日
 *      Author: Acer
 */

#include "inet/linklayer/ethernet/EtherFrame.h"
#include "inet/networklayer/ipv4/IPv4Datagram.h"
#include "common/messages/PauseMessage_m.h"
#include "inet/networklayer/common/L3Address.h"
#include "inet/applications/udpapp/UDPReliableAppPacket_m.h"
#include "inet/applications/udpapp/UDPReliableAppPacketAck_m.h"
#include "common/FlowSensor.h"
#include <cmath>
#include <omnetpp.h>
#include "SwitchEtherQueue.h"
#include "../../common/BufferUsage_m.h"

using namespace std;
using namespace inet;
using namespace commons;
using namespace mine;

Define_Module(SwitchEtherQueue);

const static int ECN_CE = 3;

simsignal_t SwitchEtherQueue::queueSizeSignal = registerSignal("queueSize");
simsignal_t SwitchEtherQueue::rateSignal = registerSignal("rate");
simsignal_t SwitchEtherQueue::rateMsgBwSignal = registerSignal("rateMsgBw");
simsignal_t SwitchEtherQueue::markProbabilitySignal = registerSignal("markProbability");
simsignal_t SwitchEtherQueue::spotProbabilitySignal = registerSignal("spotProbability");

SwitchEtherQueue::SwitchEtherQueue():
        enablePFC(false),
        congCtrl(true),
        paused(false),
        pauseThreshold(0),
        unpauseThreshold(0),
        oobL3InGate(nullptr),
        oobL3OutGate(nullptr),
        queueThreshold(40000),
        //minth(5000),
        bandwidth(0),
        //fairRate(0),
        numIngress(0),
        ingressRate(0),
        g(0.05),
        minAlpha(0.9),
        rateThreshold(50), //Mbps
        timeThreshold(1000),
        clockEvent(nullptr)

{}

SwitchEtherQueue::~SwitchEtherQueue()
{
    cancelAndDelete(clockEvent);
}

void SwitchEtherQueue::initialize()
{
    BaseQueue::initialize();

    enablePFC = par("enablePFC");
    pauseThreshold = par("pauseThreshold").longValue() * 1000;
    unpauseThreshold = par("unpauseThreshold").longValue() * 1000;
    congCtrl = par("congCtrl");
    bandwidth = par("linkBandwidth").longValue() * 1000; // Mbps
    fairRate = bandwidth;

    oobL3InGate = gate("oobL3In");
    oobL3OutGate = gate("oobL3Out");

    queueThreshold = par("queueThreshold").longValue() * 1000; //bytes
    rateThreshold = par("rateThreshold").longValue(); //Mbps
    timeThreshold = par("timeThreshold").longValue() * 1e-6;// RTT
    rtt = par("rtt").longValue() * 1e-6;
    minAlpha = par("minAlpha");

    fairRateVector.setName("fairRate");
    ingressRateVector.setName("ingressRateVector");

    //WATCH(rateMessageCount);
    //WATCH(pauseCount);
    WATCH(paused);
    setClock();
}

void SwitchEtherQueue::handleMessage(cMessage* msg)
{
    if(!msg->isSelfMessage()){
        PassiveQueueBase::handleMessage(msg);
    }else{
        handleClockEvent();
    }
}

void SwitchEtherQueue::handleClockEvent()
{
    double T = timeThreshold.dbl() * 1e6; // in us
    T = T / 10;
    ingressRate = numIngress / T; // in Mbps
    double alpha  = 0.005;
    ingressRate = (1-alpha)*ingressRate + alpha * numIngress / T;

    ingressRateVector.recordWithTimestamp(simTime(), ingressRate);
    numIngress = 0;
    //getFairRate();
    setClock();

}

void SwitchEtherQueue::setClock()
{
    if (!clockEvent) {
        clockEvent = new cMessage("ClockEvent");
    }
    scheduleAt(simTime() + timeThreshold/10, clockEvent);
}
bool SwitchEtherQueue::isFull()
{
    return queueCapacity <= queueSize;
}

void SwitchEtherQueue::processIngressMessage(const cMessage* msg)
{
    BaseQueue::processIngressMessage(msg);

    emitQueueSize();
    UpdateBufferUsage(msg);

    if(enablePFC && !paused && (pauseThreshold <= queueSize)){
        sendPause();
    }

    const EtherFrame* frame = dynamic_cast<const EtherFrame*>(msg);
    if (!frame) {
        return;
    }
    cPacket* udpPacket = frame->getEncapsulatedPacket()->getEncapsulatedPacket();
    if (!udpPacket) {
        return;
    }
    UDPReliableAppPacket* appPacket = dynamic_cast<UDPReliableAppPacket*>(udpPacket->getEncapsulatedPacket());
    if (!appPacket) {
        return;
    }

    numIngress += udpPacket->getBitLength();

    uint32 flowId = FlowSensor::getFlowId((const EtherFrame *)msg);
    updateFlowTable(msg);
    if(!handleRateSuggestion(flowId))
    {
        handleECN(msg);
    }
   // queueManage(msg);
}

void SwitchEtherQueue::processEgressMessage(const cMessage* msg)
{
    BaseQueue::processEgressMessage(msg);

    emitQueueSize();
    UpdateBufferUsage(msg, true);

    if (enablePFC && paused && (queueSize <= unpauseThreshold)) {
        sendUnpause();
    }
}

void SwitchEtherQueue::updateFlowTable(const cMessage* msg)
{
    uint32 flowId = FlowSensor::getFlowId((const EtherFrame *)msg);

    //update rate flow table
    auto item = mineFlowTable.find(flowId);
    if(item == mineFlowTable.end()){
        mineFlowTable[flowId].rate = 0;
        mineFlowTable[flowId].suggestTime = 0;
        mineFlowTable[flowId].flag = FLAG_ISBOTTLE;
        mineFlowTable[flowId].sugRate = 0;
        mineFlowTable[flowId].estRateVector.setName(("estRate-" + to_string(flowId)).c_str());
    }else{
        simtime_t deltTime = simTime() - item->second.updateTime;
        const cPacket* packet = dynamic_cast<const cPacket*>(msg);
        if(deltTime.dbl() != 0)
        {
            item->second.rate = (1 -g)*item->second.rate + g * packet->getBitLength() / (deltTime.dbl() * 1e6);
            item->second.estRateVector.recordWithTimestamp(simTime(), item->second.rate);
        }
    }

    //update time flow table
    mineFlowTable[flowId].updateTime = simTime();

}

int SwitchEtherQueue::getFlowRate(uint32 flowId)
{
    auto rateItem = mineFlowTable.find(flowId);
    if(rateItem == mineFlowTable.end())
        return -1;
    return rateItem->second.rate;
}

bool SwitchEtherQueue::handleRateSuggestion(inet::uint32 flowId)
{
    int flowRate = getFlowRate(flowId);
    if(flowRate < 0)
        return false;

    getFairRate();

    if(mineFlowTable[flowId].suggestTime == 0)
    {
        suggestRate(flowId);
        mineFlowTable[flowId].flag = FLAG_UNDERWATCH;

        return true;
    }

    if(simTime() - mineFlowTable[flowId].suggestTime >= timeThreshold)
    {
        if(mineFlowTable[flowId].flag == FLAG_UNDERWATCH){
            if(abs(mineFlowTable[flowId].sugRate - flowRate) > rateThreshold){
                mineFlowTable[flowId].flag = FLAG_NOTBOTTLE;
            }
            else{
                mineFlowTable[flowId].flag = FLAG_ISBOTTLE;
            }
        }

        if(mineFlowTable[flowId].flag == FLAG_ISBOTTLE){
            if(abs(fairRate - flowRate) > rateThreshold){
                suggestRate(flowId);
                mineFlowTable[flowId].flag = FLAG_UNDERWATCH; //flag-->11
                return true;
            }
            else return false;
        }else if(mineFlowTable[flowId].flag == FLAG_NOTBOTTLE){
            if(flowRate - fairRate > rateThreshold ){
                suggestRate(flowId);
                mineFlowTable[flowId].flag = FLAG_ISBOTTLE; //flag-->10
            }
             return true;
        }
    }
    return false;
}

void SwitchEtherQueue::suggestRate(inet::uint32 flowId)
{
   // getFairRate();

    mineFlowTable[flowId].sugRate = fairRate;
    mineFlowTable[flowId].suggestTime = simTime();

    RateMessage* rateMsg = new RateMessage("RateMessage");
    rateMsg->setRate(fairRate);
    rateMsg->setFlowId(flowId);

    EV_INFO << "Sent " << rateMsg << " message from L3FCN engine.\n";
    send(rateMsg, oobL3OutGate);
}
void SwitchEtherQueue::getFairRate()
{
    int numActive = 0;
    int numBottle = 0;
    int totRate = 0;

    double L;
    int lam;

    for(auto item = mineFlowTable.begin(); item != mineFlowTable.end(); item++)
    {
        if(simTime() - item->second.updateTime <= rtt)
        {
            numActive++;
            if(item->second.flag == FLAG_ISBOTTLE || item->second.flag == FLAG_UNDERWATCH){
                numBottle++;
            }
            else{
                totRate += item->second.rate;
            }
        }
    }
    double alpha;
    if(queueSize >= queueThreshold)
        alpha = minAlpha;
    else alpha = 1;

    //L = alpha * (double)bandwidth - ingressRate;
    //lam = L / rateThreshold ;

    if(numBottle){
        fairRate = (alpha * (double)bandwidth - totRate) / numBottle ;
    }


    //fairRate = alpha * (double)bandwidth / ingressRate * fairRate;
    if(numBottle ){
        if(fairRate > alpha * (double)bandwidth / numBottle)
        {
            fairRate = alpha * (double)bandwidth / numBottle;
        }
        //fairRate +=  0.1*(alpha * (double)bandwidth - ingressRate)/numBottle;
    }

    if(numActive){
        if(fairRate <  alpha* (double)bandwidth / numActive){
            fairRate = alpha * (double)bandwidth / numActive;
        }
    }

    if(fairRate > bandwidth){
        fairRate = bandwidth;
    }

    fairRateVector.recordWithTimestamp(simTime(), fairRate);
}

void SwitchEtherQueue::handleECN(const cMessage* msg)
{
    if(!congCtrl)
        return;

    const cPacket* pkt = PK((cMessage*)msg);
    if(!pkt)
        return;

    if (queueSize < queueThreshold)
        return;
    else
    {
        IPv4Datagram* datagram = dynamic_cast<IPv4Datagram*>(pkt->getEncapsulatedPacket());
        datagram->setExplicitCongestionNotification(ECN_CE);
    }
}

void SwitchEtherQueue::emitQueueSize()
{
    emit(queueSizeSignal, queueSize/1000);
}

void SwitchEtherQueue::UpdateBufferUsage(const cMessage* msg, bool release)
{
    const cPacket* frame = dynamic_cast<const cPacket*>(msg);
    cPacket* datagram = frame->getEncapsulatedPacket();
    if (datagram && datagram->hasPar("IngressPort")) {
        BufferUsage* bufferUsage = new BufferUsage();
        bufferUsage->setPortNum(datagram->par("IngressPort").longValue());
        bufferUsage->setUsage(frame->getByteLength() * (release ? -1 : 1));
        send(bufferUsage, oobL3OutGate);
        if (release) {
            delete datagram->getParList().remove("IngressPort");
        }
    }
}

void SwitchEtherQueue::sendPause()
{
    // Send pause message
    PauseMessage* pauseMsg = new PauseMessage();
    pauseMsg->setPauseUnits(65535);

    send(pauseMsg, oobL3OutGate);

    paused = true;
    //pauseCount++;
}

void SwitchEtherQueue::sendUnpause()
{
    // Send pause message
    PauseMessage* pauseMsg = new PauseMessage();
    pauseMsg->setPauseUnits(0);

    send(pauseMsg, oobL3OutGate);

    paused = false;
}

void SwitchEtherQueue::finish()
{
    PassiveQueueBase::finish();
}



