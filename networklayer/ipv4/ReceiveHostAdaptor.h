/*
 * ReceiveHostAdaptor.h
 *
 *  Created on: Mar 31, 2023
 *      Author: wwt
 */

#ifndef NETWORKLAYER_IPV4_RECEIVEHOSTADAPTOR_H_
#define NETWORKLAYER_IPV4_RECEIVEHOSTADAPTOR_H_

#include "inet/common/Compat.h"
#include "inet/applications/udpapp/UDPReliableAppPacket_m.h"
#include <map>
#include "../../common/AckMessage_m.h"

namespace mine {

class ReceiveHostAdaptor
{
  public:
    static bool appPkt(omnetpp::cMessage* msg);

    AckMessage* receive(omnetpp::cMessage* msg);

    void finish(std::string path);

  private:
    struct Channel
    {
        inet::uint32 nextSequence;
        bool inRecovery;
    };
    typedef std::map<inet::uint32, Channel*> Channels;

    Channels channels; // By flow

    std::stringstream ss;
};

}// name space mine





#endif /* NETWORKLAYER_IPV4_RECEIVEHOSTADAPTOR_H_ */
