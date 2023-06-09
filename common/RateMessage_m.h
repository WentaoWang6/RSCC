//
// Generated file, do not edit! Created by nedtool 5.2 from common/RateMessage.msg.
//

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif
#ifndef __MINE_RATEMESSAGE_M_H
#define __MINE_RATEMESSAGE_M_H

#include <omnetpp.h>

// nedtool version check
#define MSGC_VERSION 0x0502
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of nedtool: 'make clean' should help.
#endif

// cplusplus {{
#include "inet/common/INETDefs.h"
// }}


namespace mine {

/**
 * Class generated from <tt>common/RateMessage.msg:22</tt> by nedtool.
 * <pre>
 * packet RateMessage
 * {
 *     uint32_t srcAddress;
 *     uint32_t destAddress;
 *     uint32_t flowId;
 *     int rate;
 * }
 * </pre>
 */
class RateMessage : public ::omnetpp::cPacket
{
  protected:
    uint32_t srcAddress;
    uint32_t destAddress;
    uint32_t flowId;
    int rate;

  private:
    void copy(const RateMessage& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const RateMessage&);

  public:
    RateMessage(const char *name=nullptr, short kind=0);
    RateMessage(const RateMessage& other);
    virtual ~RateMessage();
    RateMessage& operator=(const RateMessage& other);
    virtual RateMessage *dup() const override {return new RateMessage(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    // field getter/setter methods
    virtual uint32_t getSrcAddress() const;
    virtual void setSrcAddress(uint32_t srcAddress);
    virtual uint32_t getDestAddress() const;
    virtual void setDestAddress(uint32_t destAddress);
    virtual uint32_t getFlowId() const;
    virtual void setFlowId(uint32_t flowId);
    virtual int getRate() const;
    virtual void setRate(int rate);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const RateMessage& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, RateMessage& obj) {obj.parsimUnpack(b);}

} // namespace mine

#endif // ifndef __MINE_RATEMESSAGE_M_H

