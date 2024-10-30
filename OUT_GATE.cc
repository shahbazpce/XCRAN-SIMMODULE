/*
 * OUT_GATE.cc
 *
 *  Created on: 09-Jan-2021
 *      Author: Subhajit
 */
#include <omnetpp.h>
#define T_G 5e-9
using namespace omnetpp;
using namespace std;
/* Class Definition for Source Node
 *
 */
class OUT_GATE : public cSimpleModule
{
private:
private:
    long onu_id;
    long alloc_id2, alloc_id3, alloc_id4;
    bool flag2, flag3, flag4;
    bool stat2, stat3, stat4;
    long grant2, grant3, grant4, grant5;
    double Ru, start, start2, start3, start4;
    long buf2, buf3, buf4;
    long report2, report3, report4;

public:
    virtual ~OUT_GATE();

protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    void generate_BWmap();
};
/* Define Module Links the Class to the node in the NED file
 *
 */
Define_Module(OUT_GATE);
/*
 * Destructor exectues when the program ends. In this case we cancel any scheduled events and delete the
 * message that schedules the events
 */
OUT_GATE::~OUT_GATE()
{

}
/*
 * Initialize the variables declared in class
 */
void OUT_GATE::initialize()
{
    Ru=par("up_rate");
}

void OUT_GATE::handleMessage(cMessage *msg)
{

    if(strcmp(msg->getName(),"synchronize")==0)
    {
        onu_id=msg->par("ONU_ID").longValue();
        cMessage *msg2=msg->dup();
        send(msg2,"o1");
        cMessage *msg3=msg->dup();
        send(msg3,"o2");
        cMessage *msg4=msg->dup();
        send(msg4,"o3");
        delete msg;
    }

    if(strcmp(msg->getName(),"BWmap")==0)
    {
        long id=msg->par("ONU_ID").longValue();
        start=msg->par("start_time").doubleValue();
        if(id==onu_id)
        {
            alloc_id2=msg->par("Alloc_ID_2").longValue();
            alloc_id3=msg->par("Alloc_ID_3").longValue();
            alloc_id4=msg->par("Alloc_ID_4").longValue();
            flag2=msg->par("Flag_2").boolValue();
            flag3=msg->par("Flag_3").boolValue();
            flag4=msg->par("Flag_4").boolValue();
            if(flag2==true)
            {
                report2=msg->par("Report_ID_2").longValue();
            }
            if(flag3==true)
            {
                report3=msg->par("Report_ID_3").longValue();
            }
            if(flag4==true)
            {
                report4=msg->par("Report_ID_4").longValue();
            }
            grant2=msg->par("grant_size_2").longValue();
            grant3=msg->par("grant_size_3").longValue();
            grant4=msg->par("grant_size_4").longValue();
            grant5=msg->par("grant_size_5").longValue();
            stat2=false;
            stat3=false;
            stat4=false;
            cMessage *msg2=new cMessage("INT_Report");
            send(msg2,"o1");
            cMessage *msg3=msg2->dup();
            sendDelayed(msg3,T_G,"o2");
            cMessage *msg4=msg2->dup();
            sendDelayed(msg4,T_G*2,"o3");
        }
        delete msg;

    }

    if(strcmp(msg->getName(),"INT_Report")==0)
    {
        int alloc_id=msg->par("Alloc_ID").longValue();
        int type=alloc_id%10;
        if(type==2)
        {
            stat2=true;
            buf2=msg->par("Window Size").longValue();
        }
        if(type==3)
        {
            stat3=true;
            buf3=msg->par("Window Size").longValue();
        }
        if(type==4)
        {
            stat4=true;
            buf4=msg->par("Window Size").longValue();
        }
        if(stat2 && stat3 && stat4)
        {
            if(buf2>grant2 && grant5>0)
            {
                if(buf2-grant2>grant5)
                {
                    grant2+=grant5;
                    grant5=0;
                }
                else
                {
                    grant5-=(buf2-grant2);
                    grant2=buf2;
                }
            }
            if(buf3>grant3 && grant5>0)
            {
                if(buf3-grant3>grant5)
                {
                    grant3+=grant5;
                    grant5=0;
                }
                else
                {
                    grant5-=(buf3-grant3);
                    grant3=buf3;
                }
            }
            if(buf4>grant4 && grant5>0)
            {
                if(buf4-grant4>grant5)
                {
                    grant4+=grant5;
                    grant5=0;
                }
                else
                {
                    grant5-=(buf4-grant4);
                    grant4=buf4;
                }
            }
            generate_BWmap();
        }
        delete msg;
    }
    if(strcmp(msg->getName(),"Report")==0)
    {
        send(msg,"o4");
    }
}

void OUT_GATE:: generate_BWmap()
{
    cMessage *msg2=new cMessage("BWmap");
    msg2->addPar("Flag");
    msg2->addPar("Alloc_ID");
    msg2->addPar("start_time");
    msg2->addPar("grant_size");
    msg2->par("Alloc_ID").setLongValue(alloc_id2);
    msg2->par("Flag").setBoolValue(flag2);
    start2=start;
    msg2->par("start_time").setDoubleValue(start2);
    msg2->par("grant_size").setLongValue(grant2);
    if(flag2==true)
    {
        msg2->addPar("Report_ID");
        msg2->par("Report_ID").setLongValue(report2);
    }
    send(msg2,"o1");
    cMessage *msg3=new cMessage("BWmap");
    msg3->addPar("Flag");
    msg3->addPar("Alloc_ID");
    msg3->addPar("start_time");
    msg3->addPar("grant_size");
    msg3->par("Alloc_ID").setLongValue(alloc_id3);
    msg3->par("Flag").setBoolValue(flag3);
    start3=start+grant2*8/Ru;
    msg3->par("start_time").setDoubleValue(start3);
    msg3->par("grant_size").setLongValue(grant3);
    if(flag3==true)
    {
        msg3->addPar("Report_ID");
        msg3->par("Report_ID").setLongValue(report3);
    }
    send(msg3,"o2");
    cMessage *msg4=new cMessage("BWmap");
    msg4->addPar("Flag");
    msg4->addPar("Alloc_ID");
    msg4->addPar("start_time");
    msg4->addPar("grant_size");
    msg4->par("Alloc_ID").setLongValue(alloc_id4);
    msg4->par("Flag").setBoolValue(flag4);
    start4=start+(grant2+grant3)*8/Ru;
    msg4->par("start_time").setDoubleValue(start4);
    msg4->par("grant_size").setLongValue(grant4);
    if(flag4==true)
    {
        msg4->addPar("Report_ID");
        msg4->par("Report_ID").setLongValue(report4);
    }
    send(msg4,"o3");
}




