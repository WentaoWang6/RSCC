/*
 * ICMPEx.cc
 *
 *  Created on: Mar 31, 2023
 *      Author: wwt
 */

#include "ICMPEx.h"
#include "inet/networklayer/contract/ipv4/IPv4ControlInfo.h"

using namespace inet;
using namespace mine;

Define_Module(ICMPEx);

ICMPEx::ICMPEx():
        ICMP(),
        numRateMsgsSent(0),
        numRateMsgsReceived(0),
        numAcksSent(0),
        numAcksReceived(0)
{
    WATCH(numRateMsgsSent);
    WATCH(numRateMsgsReceived);
    WATCH(numAcksSent);
    WATCH(numAcksReceived);
}

void ICMPEx::handleMessage(cMessage* msg)
{
    cGate* arrivalGate = msg->getArrivalGate();

    if (!strcmp(arrivalGate->getName(), "l3RelayIn"))
    {
        RateMessage* rateMsg;
        AckMessage* ackMsg;
        CNPMessage* cnpMsg;
        if ((rateMsg = dynamic_cast<RateMessage*>(msg)) != nullptr) { // Switch only
            EV_INFO << "Received " << rateMsg << " message from L3FCN enabled switch ethernet queue.\n";
            sendRateMessage(rateMsg);
        } else if ((ackMsg = dynamic_cast<AckMessage*>(msg)) != nullptr) { // Destination only
            EV_INFO << "Received " << ackMsg << " message from IRN host handler.\n";
            sendAck(ackMsg);
        } else if ((cnpMsg = dynamic_cast<CNPMessage*>(msg)) != nullptr) {
            EV_INFO << "Received " << cnpMsg << " message from relay unit.\n";
            sendCNPMessage(cnpMsg);
        }

    }
    else {
        ICMP::handleMessage(msg);
    }
}

void ICMPEx::sendRateMessage(RateMessage* rateMsg)
{
    IPv4ControlInfo* ipv4Ctrl = new IPv4ControlInfo();
    ipv4Ctrl->setProtocol(IP_PROT_ICMP);
    ipv4Ctrl->setDestinationAddress(L3Address(IPv4Address(rateMsg->getDestAddress())));

    ICMPMessage* icmpMsg = new ICMPMessage("L3FCNRate");
    icmpMsg->setType(ICMP_MINE_RATE);
    icmpMsg->setControlInfo(ipv4Ctrl);
    icmpMsg->encapsulate(rateMsg);

    EV_INFO << "Sent " << icmpMsg << " message to network layer.\n";

    sendToIP(icmpMsg);

    numRateMsgsSent++;
}

void ICMPEx::sendAck(AckMessage* ackMsg)
{
    IPv4ControlInfo* ipv4Ctrl = new IPv4ControlInfo();
    ipv4Ctrl->setProtocol(IP_PROT_ICMP);
    ipv4Ctrl->setDestinationAddress(L3Address(IPv4Address(ackMsg->getDestAddress())));

    ICMPMessage* icmpMsg = new ICMPMessage("L3FCNAck");
    icmpMsg->setType(ICMP_MINE_ACK);
    icmpMsg->setControlInfo(ipv4Ctrl);
    icmpMsg->encapsulate(ackMsg);

    sendToIP(icmpMsg);

    numAcksSent++;

    EV_INFO << "Sent " << icmpMsg << " message to network layer.\n";
}

void ICMPEx::sendCNPMessage(CNPMessage* cnpMsg)
{
    IPv4ControlInfo* ipv4Ctrl = new IPv4ControlInfo();
    ipv4Ctrl->setProtocol(IP_PROT_ICMP);
    ipv4Ctrl->setDestinationAddress(L3Address(IPv4Address(cnpMsg->getDestAddress())));

    ICMPMessage* icmpMsg = new ICMPMessage("CNP");
    icmpMsg->setType(ICMP_MINE_CNP);
    icmpMsg->setControlInfo(ipv4Ctrl);
    icmpMsg->encapsulate(cnpMsg);

    sendToIP(icmpMsg);

    EV_INFO << "Sent " << icmpMsg << " message to network layer.\n";
}

void ICMPEx::processICMPMessage(ICMPMessage* icmpmsg)
{
// Source only
    if (icmpmsg->getType() == ICMP_MINE_RATE) {
        IPv4ControlInfo* ipv4Ctrl = dynamic_cast<IPv4ControlInfo*>(icmpmsg->getControlInfo());
        RateMessage* rateMsg = dynamic_cast<RateMessage*>(icmpmsg->decapsulate());
        if (!rateMsg) {
            throw cRuntimeError("Unexpected ICMP message received!");
        }
        if (ipv4Ctrl->getDestinationAddress().toIPv4().getInt() != rateMsg->getDestAddress()) {
            throw cRuntimeError("Rate message received at wrong destination!");
        }
        rateMsg->setSrcAddress(ipv4Ctrl->getSourceAddress().toIPv4().getInt());
        EV_INFO << "Received " << rateMsg << " message from network.\n";
        delete icmpmsg;
        send(rateMsg, "l3RelayOut");
        numRateMsgsReceived++;
    } else if (icmpmsg->getType() == ICMP_MINE_ACK) {
        IPv4ControlInfo* ipv4Ctrl = dynamic_cast<IPv4ControlInfo*>(icmpmsg->getControlInfo());
        AckMessage* ackMsg = dynamic_cast<AckMessage*>(icmpmsg->decapsulate());
        if (!ackMsg) {
            throw cRuntimeError("Unexpected ICMP message received!");
        }
        if (ipv4Ctrl->getDestinationAddress().toIPv4().getInt() != ackMsg->getDestAddress()) {
            throw cRuntimeError("Ack message received at wrong destination!");
        }
        EV_INFO << "Received " << ackMsg << " message from network.\n";
        delete icmpmsg;
        send(ackMsg, "l3RelayOut");
        numAcksReceived++;
    } else if (icmpmsg->getType() == ICMP_MINE_CNP){
        IPv4ControlInfo* ipv4Ctrl = dynamic_cast<IPv4ControlInfo*>(icmpmsg->getControlInfo());
        CNPMessage* cnpMsg = dynamic_cast<CNPMessage*>(icmpmsg->decapsulate());
        if(!cnpMsg){
            throw cRuntimeError("Unexpected ICMP message received!");
        }
        if(ipv4Ctrl->getDestinationAddress().toIPv4().getInt() != cnpMsg->getDestAddress()){
            throw cRuntimeError("CNP message received at wrong destination!");
        }
        EV_INFO << "Received " << cnpMsg << " message from network.\n";
        delete icmpmsg;
        send(cnpMsg, "l3RelayOut");
    }
    else
    {
        ICMP::processICMPMessage(icmpmsg);
    }
}










