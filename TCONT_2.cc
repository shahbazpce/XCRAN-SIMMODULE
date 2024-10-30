/*
 * TCONT_2.cc
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
class TCONT_2 : public cSimpleModule
{
private:
private:
    //Statistics
    cStdDev pkt_delay;
    cHistogram pd;
    cStdDev pkt_loss_rate;
    cStdDev Queue_Size;
    cStdDev Grant_size;
    cStdDev frame_size;
    //Parameters and states
    cMessage *clear_data;
    cMessage *generate_report;
    cQueue queue, temp_queue;
    int onu_id; //ONU identification
    long alloc_id; //TCONT identification
    long max_len; // Maximum buffer size
    long grnt_sz;
    long size, current_size;// Granted window size from OLT
    long data_size, temp_data_size;// Buffer occupancy of ONU
    long arr_pkts, dscrd_pkts;
    long report_id, current_report_id;
    double rtt, link_rate;
    double wait_time;
    bool flag;
    int counter;

public:
    virtual ~TCONT_2();

protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void finish();
};
/* Define Module Links the Class to the node in the NED file
 *
 */
Define_Module(TCONT_2);
/*
 * Destructor exectues when the program ends. In this case we cancel any scheduled events and delete the
 * message that schedules the events
 */
TCONT_2::~TCONT_2()
{
    cancelAndDelete(clear_data);
    cancelAndDelete(generate_report);
}
/*
 * Initialize the variables declared in class
 */
void TCONT_2::initialize()
{
    max_len=1e6; // Maximum buffer size
    link_rate=par("out_rate"); // upstream link rate
    arr_pkts=0, dscrd_pkts=0;
    size=0, current_size=0, data_size=0, temp_data_size=0;
    current_report_id=0;
    grnt_sz=0;
    counter=0;
    clear_data=new cMessage("Clear_data");
    generate_report=new cMessage("Generate_report");
}

void TCONT_2::handleMessage(cMessage *msg)
{
    if(strcmp(msg->getName(),"Data")==0)
    {
        arr_pkts+=1;
        if(data_size+temp_data_size+msg->par("Size").longValue()<max_len)
        {
            data_size+=msg->par("Size").longValue();
            frame_size.collect(msg->par("Size").longValue());
            msg->setArrivalTime(simTime().dbl());
            queue.insert(msg);
        }
        else
        {
            delete msg;
            dscrd_pkts+=1;
        }
    }

    if(strcmp(msg->getName(),"synchronize")==0)
    {
        onu_id=msg->par("ONU_ID").longValue();
        alloc_id=onu_id*10+2;
        rtt=msg->par("RTT").doubleValue();
        EV<<"Received RTT value of ONU " <<onu_id<<"= "<<rtt<<endl;
        delete msg;
    }

    if(strcmp(msg->getName(),"BWmap")==0)
    {
        long id=msg->par("Alloc_ID").longValue();
        if(id==alloc_id)
        {
            flag=msg->par("Flag").boolValue();
            if(flag==true)
            {
                Grant_size.collect(grnt_sz);
                grnt_sz=0;
            }
            if(msg->hasPar("Report_ID"))
                report_id=msg->par("Report_ID").longValue();
            size=msg->par("grant_size").longValue();
            grnt_sz+=size;
            EV<<"Granted window size from OLT= "<<size<<" for TCONT: "<<id<<endl;
            EV<<"Current queue size of TCONT "<<id<<"= "<<data_size+temp_data_size<<endl;
            wait_time=msg->par("start_time").doubleValue();
            EV<<"Waiting time of TCONT "<<id<<" before transmission: "<<wait_time<<endl;
            if(flag==true)
            {
                counter+=1;
                if(!clear_data->isScheduled())
                {
                    if(!generate_report->isScheduled())
                    {
                        current_report_id=report_id;
                        scheduleAt(simTime().dbl()+wait_time,generate_report);
                    }
                }
            }
            else
            {
                if(!clear_data->isScheduled())
                {
                    if(size>0 && size<=data_size+temp_data_size)
                    {
                        current_size=size;
                        size=0;
                        scheduleAt(simTime().dbl()+wait_time,clear_data);
                    }
                }
            }
        }
        delete msg;
    }

    if(strcmp(msg->getName(),"INT_Report")==0)
    {
        long Q_size=data_size+temp_data_size;
        cMessage *data=new cMessage("INT_Report");
        data->addPar("Alloc_ID");
        data->par("Alloc_ID").setLongValue(alloc_id);
        data->addPar("Window Size");
        data->par("Window Size").setLongValue(Q_size);
        send(data,"out");
        delete msg;
    }

    if(strcmp(msg->getName(),"Generate_report")==0)
    {
        counter-=1;
        //Prepare the Report message containing the amount of
        //additional data waiting in the buffer
        long Q_size=data_size+temp_data_size;
        //Collect the Queue size
        Queue_Size.collect(Q_size);
        EV<<"Current queue size of TCONT "<<alloc_id<<"= "<<Q_size<<endl;
        cMessage *data=new cMessage("Report");
        data->addPar("Alloc_ID");
        data->par("Alloc_ID").setLongValue(alloc_id);
        data->addPar("Report_ID");
        data->par("Report_ID").setLongValue(current_report_id);
        data->addPar("Window Size");
        data->par("Window Size").setLongValue(Q_size);
        send(data,"out");
        if(size>0 && size<=data_size+temp_data_size)
        {
            current_size=size;
            size=0;
            scheduleAt(simTime().dbl(),clear_data);
        }
        else
        {
            if(counter>0)
            {
                current_report_id=report_id;
                scheduleAt(simTime().dbl()+wait_time,generate_report);
            }
        }
    }

    if(strcmp(msg->getName(),"Clear_data")==0)
    {
        if(current_size>0 && temp_data_size>0)
        {
            cMessage *msg=(cMessage* )temp_queue.pop();
            pkt_delay.collect(simTime()-msg->getArrivalTime());
            pd.collect(simTime()-msg->getArrivalTime());
            long sz=msg->par("Size").longValue();
            if(current_size>=sz)
            {
                current_size-=sz;
                temp_data_size-=sz;
                delete msg;
                double wait=sz*8/link_rate;
                scheduleAt(simTime().dbl()+wait,clear_data);
            }
            else
            {
                long remainder=sz-current_size;
                msg->par("Size").setLongValue(remainder);
                temp_queue.insert(msg);
                temp_data_size-=current_size;
                double wait=current_size*8/link_rate;
                current_size=0;
                scheduleAt(simTime().dbl()+wait,clear_data);
            }
        }
        else
        {
            if(current_size>0 && data_size>0)
            {
                cMessage *msg=(cMessage* )queue.pop();
                pkt_delay.collect(simTime()-msg->getArrivalTime());
                pd.collect(simTime()-msg->getArrivalTime());
                long sz=msg->par("Size").longValue();
                if(current_size<sz)
                {
                    long remainder=sz-current_size;
                    msg->par("Size").setLongValue(remainder);
                    temp_queue.insert(msg);
                    temp_data_size+=remainder;
                    data_size-=sz;
                    double wait=current_size*8/link_rate;
                    current_size=0;
                    scheduleAt(simTime().dbl()+wait,clear_data);
                }
                else
                {
                    current_size-=sz;
                    data_size-=sz;
                    double wait=sz*8/link_rate;
                    delete msg;
                    scheduleAt(simTime().dbl()+wait,clear_data);
                }
            }
            else
            {
                if(counter>0)
                {
                    if(!generate_report->isScheduled())
                    {
                        current_report_id=report_id;
                        scheduleAt(simTime().dbl()+wait_time,generate_report);
                    }
                }
                else
                {
                    if(size>0 && size<=data_size+temp_data_size)
                    {
                        current_size=size;
                        size=0;
                        scheduleAt(simTime().dbl()+wait_time,clear_data);
                    }
                }
            }
        }
    }
}

void TCONT_2::finish()
{
    cout<<pkt_delay.getMean()<<endl;
    //cout<<"Queue size= "<<Queue_Size.getMean()<<endl;
    //cout<<"Grant size= "<<Grant_size.getMean()<<endl;
    //cout<<"Average frame size= "<<frame_size.getMean()<<endl;
    pkt_delay.record();
    pd.record();
    //double loss_rate=dscrd_pkts/arr_pkts;
    //cout<<"Packet loss rate of ONU "<<onu_id<<"= "<<loss_rate<<endl;
}




