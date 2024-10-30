/*
 * PowerSplitter.cc
 *
 *  Created on: 22-Oct-2022
 *      Author: User
 */

#ifndef POWERSPLITTER_CC_
#define POWERSPLITTER_CC_


#include <omnetpp.h>
#include<string.h>

using namespace omnetpp;


class PowerSplitter : public cSimpleModule
{
private:
    int i;
protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    void finish();
public:
    PowerSplitter();
    virtual ~PowerSplitter();

};

Define_Module(PowerSplitter);



PowerSplitter::PowerSplitter()
{

}


PowerSplitter::~PowerSplitter()
{

}

void PowerSplitter :: initialize()
{
    i=0;
}

void PowerSplitter :: handleMessage(cMessage *msg)
{
    const char *getName=msg->getName();

    if(strcmp(getName,"BWmap")==0)
        {
  //          for(i=0;i<=15;i++)
  //          {
             cMessage *msgCopy=msg->dup();
                send(msgCopy,"outOnu",i);
                if(i==15)
                {
                    i=0;
                }
                else
                i++;
//            }
            delete(msg);
        }
        else
        {
            send(msg,"outOLT");
        }

}


#endif /* POWERSPLITTER_CC_ */






void PowerSplitter::finish()
{

}
