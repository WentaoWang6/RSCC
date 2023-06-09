/*
 * ETFlowSensor.h
 *
 *  Created on: Mar 31, 2023
 *      Author: wwt
 */

#ifndef NETWORKLAYER_IPV4_ETFLOWSENSOR_H_
#define NETWORKLAYER_IPV4_ETFLOWSENSOR_H_

#include "common/FlowSensor.h"

namespace mine
{

struct ETIngressFlow : commons::IngressFlow
{
    ETIngressFlow(inet::uint32 id):
        commons::IngressFlow(id), aged(false), byteCount(0) {}

    bool aged;
    inet::uint32 byteCount;
};

class ETFlowSensor : public commons::FlowSensor
{
  public:
    ETFlowSensor(int eleThreshold);

    virtual commons::IngressFlow* onIngressData(const cPacket* packet) override;

    void getElephantFlows(std::list<const commons::IngressFlow*>& flows, int egressIfId, int& totFlowCount) const;

    void invalidateElephants();

  private:
    int elephantThreshold;

    std::map<inet::uint32, ETIngressFlow*> activeFlows;

    virtual commons::IngressFlow* createIngressFlow(inet::uint32 flowId, const inet::IPv4Datagram* datagram) override;
};



}// name space mine



#endif /* NETWORKLAYER_IPV4_ETFLOWSENSOR_H_ */
