/*
 * Traffic_Generator.cc
 *
 *  Created on: 30-Dec-2020
 *      Author: Subhajit
 */
#include <omnetpp.h>
using namespace omnetpp;
/* Class Definition for Traffic_Generator Node
 *
 */
class Traffic_Generator : public cSimpleModule
{
private:
    //Input parameters
    double load, burst_time, idle_time, rate, shape_on, shape_off;
    long packet_size;
    //Initial computation
    int burst_len;
    double interval;
    //Pareto distributed random variables
    int next_burst_len;
    double next_idle_time;
    //scale parameter
    double b_on, b_off;
    //Count No of packets in a burst
    int burst_counter;
    //Event
    cMessage *Next_Tx_schedule;
    cMessage *Next_burst_schedule;
    //Storage container
    cQueue queue;

public:
    virtual ~Traffic_Generator();

protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    double get_off_parameter();
};
/* Define Module Links the Class to the node in the NED file
 *
 */
Define_Module(Traffic_Generator);
/*
 * Destructor exectues when the program ends. In this case we cancel any scheduled events and delete the
 * message that schedules the events
 */
Traffic_Generator::~Traffic_Generator()
{
    cancelAndDelete(Next_Tx_schedule);
    cancelAndDelete(Next_burst_schedule);
}
/*
 * Initialize the variables declared in class
 */
void Traffic_Generator::initialize()
{
   //Input parameters
   load=par("load").doubleValue();
   rate=par("Tx_rate");// Transmission rate during ON period
   shape_on=1.4; //Shape parameter for ON period
   shape_off=1.2;//Shape parameter for OFF period
   //Location parameter/minimum value of burst length
   b_on=1.0;
   b_off=get_off_parameter();
   Next_Tx_schedule=new cMessage();
   Next_burst_schedule=new cMessage();
   EV<<" value of interval is "<<interval<<endl;
   scheduleAt(simTime()+interval,Next_burst_schedule);
}

void Traffic_Generator::handleMessage(cMessage *msg)
{
    if(msg==Next_Tx_schedule)
    {
        int type=intuniform(2,4);
        cMessage *pkt=new cMessage("Data");
        pkt->addPar("Type");
        pkt->par("Type").setLongValue(type);
        pkt->addPar("Size");
        pkt->par("Size").setLongValue(packet_size);
        send(pkt,"out");
        if(--burst_counter==0)
        {
            scheduleAt(simTime()+next_idle_time,Next_burst_schedule);
        }
        else
        {
         //   EV<<" value of interval is "<<interval<<endl;
            scheduleAt(simTime()+interval,Next_Tx_schedule);
        }
    }
    if(msg==Next_burst_schedule)
    {
        next_burst_len=pareto_shifted(shape_on,b_on,0)+0.5;
        //The burst contain at least one packet
        if(next_burst_len==0)
            next_burst_len=1;
        burst_counter=next_burst_len;
        //EV<<"Next burst length= "<<next_burst_len<<endl;
        b_off=get_off_parameter();
        next_idle_time=pareto_shifted(shape_off,b_off,0);
        //EV<<"Next idle time= "<<next_idle_time<<endl;
        scheduleAt(simTime()+interval,Next_Tx_schedule);
    }
}

double Traffic_Generator:: get_off_parameter()
{
    //Trimodal distribution of packet size
    int r=intuniform(0,100);
    if(r<60)
        packet_size=64;
    if(r>=60 && r<80)
        packet_size=500;
    if(r>=80)
        packet_size=1500;
    double s=0.0000000002329;
    //Coefficient of ON and OFF period
    double coef_on=(1-pow(s,(1-1/shape_on)))/(1-1/shape_on);
    double coef_off=(1-pow(s,(1-1/shape_off)))/(1-1/shape_off);
    //Estimation of interval between transmission of two consecutive packets
    interval=packet_size*8/rate;
    //Error coefficient for ON and OFF period
    double c_on=pow((1.19*shape_on-1.166),(-0.027));
    double c_off=pow((1.19*shape_off-1.166),(-0.027));
    //Location Parameter/minimum value of idle period
    double off=interval*(coef_on/coef_off)*(c_on/c_off)*b_on*(1/load-1);
    return off;
}








