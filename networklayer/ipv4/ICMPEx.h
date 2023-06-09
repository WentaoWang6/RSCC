/*
 * ICMPEx.h
 *
 *  Created on: Mar 31, 2023
 *      Author: wwt
 */

#ifndef NETWORKLAYER_IPV4_ICMPEX_H_
#define NETWORKLAYER_IPV4_ICMPEX_H_

#include "inet/networklayer/ipv4/ICMP.h"
#include "../../common/AckMessage_m.h"
#include "../../common/RateMessage_m.h"
#include "../../common/CNPMessage_m.h"

namespace mine {

enum MINETCMPType {
    ICMP_MINE_CNP = 30,
    ICMP_MINE_ACK = 31,
    ICMP_MINE_RATE = 32
};

class INET_API ICMPEx : public inet::ICMP
{
public:
    ICMPEx();

protected:
    virtual void handleMessage(cMessage* msg) override;
    virtual void processICMPMessage(inet::ICMPMessage* icmpmsg) override;

    void sendRateMessage(RateMessage* rateMsg);
    void sendAck(AckMessage* ackMsg);
    void sendCNPMessage(CNPMessage* cnpMsg);

private:
    int numRateMsgsSent;
    int numRateMsgsReceived;
    int numAcksSent;
    int numAcksReceived;


};



}//namespace mine



#endif /* NETWORKLAYER_IPV4_ICMPEX_H_ */
