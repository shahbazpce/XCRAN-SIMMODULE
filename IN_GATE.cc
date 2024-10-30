/*
 * IN_GATE.cc
 *
 *  Created on: 09-Jan-2021
 *      Author: Subhajit
 */
#include <omnetpp.h>
using namespace omnetpp;
using namespace std;
/* Class Definition for Source Node
 *
 */
class IN_GATE : public cSimpleModule
{
private:
private:

public:
    virtual ~IN_GATE();

protected:
    virtual void handleMessage(cMessage *msg);
};
/* Define Module Links the Class to the node in the NED file
 *
 */
Define_Module(IN_GATE);
/*
 * Destructor exectues when the program ends. In this case we cancel any scheduled events and delete the
 * message that schedules the events
 */
IN_GATE::~IN_GATE()
{

}
/*
 * Initialize the variables declared in class
 */

void IN_GATE::handleMessage(cMessage *msg)
{
    if(strcmp(msg->getName(),"Data")==0)
    {
        long type=msg->par("Type").longValue();
        if(type==2)
        {
            send(msg,"out1");
        }
        if(type==3)
        {
            send(msg,"out2");
        }
        if(type==4)
        {
            send(msg,"out3");
        }
    }
}





