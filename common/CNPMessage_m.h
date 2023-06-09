//
// Generated file, do not edit! Created by nedtool 5.2 from common/CNPMessage.msg.
//

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif
#ifndef __MINE_CNPMESSAGE_M_H
#define __MINE_CNPMESSAGE_M_H

#include <omnetpp.h>

// nedtool version check
#define MSGC_VERSION 0x0502
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of nedtool: 'make clean' should help.
#endif


namespace mine {

/**
 * Class generated from <tt>common/CNPMessage.msg:18</tt> by nedtool.
 * <pre>
 * packet CNPMessage
 * {
 *     uint32_t flowId;
 *     uint32_t destAddress;
 * }
 * </pre>
 */
class CNPMessage : public ::omnetpp::cPacket
{
  protected:
    uint32_t flowId;
    uint32_t destAddress;

  private:
    void copy(const CNPMessage& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const CNPMessage&);

  public:
    CNPMessage(const char *name=nullptr, short kind=0);
    CNPMessage(const CNPMessage& other);
    virtual ~CNPMessage();
    CNPMessage& operator=(const CNPMessage& other);
    virtual CNPMessage *dup() const override {return new CNPMessage(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    // field getter/setter methods
    virtual uint32_t getFlowId() const;
    virtual void setFlowId(uint32_t flowId);
    virtual uint32_t getDestAddress() const;
    virtual void setDestAddress(uint32_t destAddress);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const CNPMessage& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, CNPMessage& obj) {obj.parsimUnpack(b);}

} // namespace mine

#endif // ifndef __MINE_CNPMESSAGE_M_H

