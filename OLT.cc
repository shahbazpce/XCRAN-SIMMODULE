/*
 * OLT.cc
 *
 *  Created on: 29-Dec-2020
 *      Author: Subhajit
 */
#include <omnetpp.h>
#define T 125e-6 //Frame duration
#define To 35e-6 //Response time of ONUs
#define T_G 10e-9 //Guard interval
using namespace omnetpp;
using namespace std;
/* Class Definition for Source Node
 *
 */
class OLT : public cSimpleModule
{
private:
private:
    //Event
    cMessage *generate_BWmap;
    //Parameters & states
    long *buf_size, *req_size, *Grant_size, *Grant_size_ext, *grant_size;
    bool *DBRu, *Flag_grnt, *Flag_ext_grnt, *stat_flag;
    long *w_max;
    int onu_id_tot, *report_count, counter;
    double *RTT, *start_time, last_time;
    double Finish_time, *last_grnt_time;
    double Ru, Rd;
    long RR, onu_id, *Report_ID;
    long FB, *VB, *SI_Timer, SI_2, SI_3, SI_4;
    long Sk; //sum of VBs of TCONTs whose SI_Timer has expired

public:
    virtual ~OLT();

protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    void generate_grant(int TCONT_id);
    long get_grant_size(int id);
    void update_VB(int id);
};
/* Define Module Links the Class to the node in the NED file
 *
 */
Define_Module(OLT);
/*
 * Destructor exectues when the program ends. In this case we cancel any scheduled events and delete the
 * message that schedules the events
 */
OLT::~OLT()
{
    cancelAndDelete(generate_BWmap);
}
/*
 * Initialize the variables declared in class
 */
void OLT::initialize()
{
    Ru=par("up_rate"); // upstream data-rate from ONUs to OLT
    Rd=par("down_rate"); //downstream data-rate from OLT to ONUs
    w_max= new long[3];//Maximum granted window size
    w_max[0]=par("window_max_Type2");
    w_max[1]=par("window_max_Type3");
    w_max[2]=par("window_max_Type4");
    SI_2=par("service_interval_Type2"); //Service interval of TCONTs of ONUs
    SI_3=par("service_interval_Type3");
    SI_4=par("service_interval_Type4");
    onu_id_tot=par("No_ONU"); //Total no of ONUs associated with OLT
    RR=0;// Index representing start ONU during scheduling
    VB=new long[onu_id_tot*10];
    SI_Timer=new long[onu_id_tot*10];
    //Set the remaining service time counter of all TCONTs
    for(int i=0;i<onu_id_tot;i++)
    {
        SI_Timer[i*10+2]=SI_2-1;
        SI_Timer[i*10+3]=SI_3-1;
        SI_Timer[i*10+4]=SI_4-1;
    }
    //Set the remaining available bytes counter of all TCONTs
    for(int i=0;i<onu_id_tot;i++)
    {
        VB[i*10+2]=w_max[0];
        VB[i*10+3]=w_max[1];
        VB[i*10+4]=w_max[2];
    }
    buf_size = new long[onu_id_tot*10];
    req_size=new long[onu_id_tot*10];
    Grant_size=new long[onu_id_tot*10];
    Grant_size_ext=new long[onu_id_tot*10];
    report_count=new int[onu_id_tot*10];
    DBRu=new bool[onu_id_tot*10];
    Flag_grnt=new bool[onu_id_tot*10];
    Flag_ext_grnt=new bool[onu_id_tot*10];
    stat_flag=new bool[onu_id_tot*10];
    grant_size=new long[onu_id_tot*10];
    Report_ID=new long[onu_id_tot*10];
    start_time=new double[onu_id_tot];
    last_grnt_time=new double[onu_id_tot*10];
    for (long i=0;i<onu_id_tot*10;i++)
    {
        buf_size[i]=0;
        req_size[i]=0;
        Grant_size[i]=0;
        Grant_size_ext[i]=0;
        grant_size[i]=0;
        report_count[i]=2;
        last_grnt_time[i]=0;
        Report_ID[i]=0;
        DBRu[i]=false;
        Flag_grnt[i]=false;
        Flag_ext_grnt[i]=false;
        stat_flag[i]=false;
    }
    //set of RTT for all ONUs
    RTT = new double[onu_id_tot];
    for (int i=0;i<onu_id_tot;i++)
    {
        //Maximum polling duration for each ONU
        double time=200e-6;
        RTT[i]=time;
        //Maximum polling duration for each ONU
        cMessage *s=new cMessage("synchronize");
        s->addPar("RTT");
        s->par("RTT").setDoubleValue(RTT[i]);
        s->addPar("ONU_ID");
        s->par("ONU_ID").setLongValue(i);
        send(s,"out");
    }
    generate_BWmap=new cMessage("generate_BW_map");
    scheduleAt(simTime()+T,generate_BWmap);
}

void OLT::handleMessage(cMessage *msg)
{
    if(strcmp(msg->getName(),"Report")==0)
    {
        long id=msg->par("Alloc_ID").longValue();
        EV<<"Report from TCONT "<<id<<" at time: "<<simTime().dbl()<<endl;
        if(msg->hasPar("Window Size"))
        {
            long d=msg->par("Window Size").longValue();
            EV<<"Requested window size= "<<d<<endl;
            long report_id=msg->par("Report_ID").longValue();
            if(report_id==0)
            {
                buf_size[id]=d-Grant_size[id];
                if(buf_size[id]<0)
                    buf_size[id]=0;
                Grant_size[id]=0;
                last_grnt_time[id]=simTime().dbl();
                Flag_grnt[id]=false;
                EV<<"Actual request size= "<<buf_size[id]<<endl;
            }
            if(report_id==1)
            {
                buf_size[id]=d-Grant_size_ext[id];
                if(buf_size[id]<0)
                    buf_size[id]=0;
                Grant_size_ext[id]=0;
                Flag_ext_grnt[id]=false;
                EV<<"Actual extra request size= "<<buf_size[id]<<endl;
            }
        }
        delete msg;
    }
    if(msg==generate_BWmap)
    {
        onu_id=RR;
        FB=38880;
        counter=0;
        for(int i=0;i<onu_id_tot;i++)
        {
            grant_size[i*10+2]=0;
            grant_size[i*10+3]=0;
            grant_size[i*10+4]=0;
            grant_size[i*10+5]=0;
        }
        int k=2;
        generate_grant(k);
        k+=1;
        generate_grant(k);
        k+=1;
        generate_grant(k);
        //Allocation of unallocated remainder
        long remainder=FB/onu_id_tot;
        //Allocation to TCONT Type2
        for(int i=0;i<onu_id_tot;i++)
        {
            grant_size[i*10+5]=remainder;
        }
        //Estimate the start time of all ONUs
        last_time=0;
        onu_id=RR;
        while(1)
        {
            int id_2=onu_id*10+2;
            int id_3=onu_id*10+3;
            int id_4=onu_id*10+4;
            int id_5=onu_id*10+5;
            if(onu_id==RR)
                start_time[onu_id]=0;
            else
                start_time[onu_id]=last_time;
            last_time+=(grant_size[id_2]+grant_size[id_3]+grant_size[id_4]+grant_size[id_5])*8/Ru+T_G;
            onu_id+=1;
            if(onu_id==onu_id_tot)
                onu_id=0;
            if(onu_id==RR)
                break;
        }
        //Generation of BWmap
        for(int i=0;i<onu_id_tot;i++)
        {
            cMessage *BWmap=new cMessage("BWmap");
            BWmap->addPar("ONU_ID");
            BWmap->par("ONU_ID").setLongValue(i);
            BWmap->addPar("start_time");
            BWmap->par("start_time").setDoubleValue(start_time[i]);
            //BW MAP generation for TCONT Type2
            BWmap->addPar("Flag_2");
            BWmap->par("Flag_2").setBoolValue(DBRu[i*10+2]);
            if(DBRu[i*10+2]==true)
            {
                BWmap->addPar("Report_ID_2");
                BWmap->par("Report_ID_2").setLongValue(Report_ID[i*10+2]);
            }
            BWmap->addPar("Alloc_ID_2");
            BWmap->addPar("grant_size_2");
            BWmap->par("Alloc_ID_2").setLongValue(i*10+2);
            BWmap->par("grant_size_2").setLongValue(grant_size[i*10+2]);
            //BW MAP generation for TCONT Type3
            BWmap->addPar("Flag_3");
            BWmap->par("Flag_3").setBoolValue(DBRu[i*10+3]);
            if(DBRu[i*10+3]==true)
            {
                BWmap->addPar("Report_ID_3");
                BWmap->par("Report_ID_3").setLongValue(Report_ID[i*10+3]);
            }
            BWmap->addPar("Alloc_ID_3");
            BWmap->addPar("grant_size_3");
            BWmap->par("Alloc_ID_3").setLongValue(i*10+3);
            BWmap->par("grant_size_3").setLongValue(grant_size[i*10+3]);
            //BW MAP generation for TCONT Type4
            BWmap->addPar("Flag_4");
            BWmap->par("Flag_4").setBoolValue(DBRu[i*10+4]);
            if(DBRu[i*10+4]==true)
            {
                BWmap->addPar("Report_ID_4");
                BWmap->par("Report_ID_4").setLongValue(Report_ID[i*10+4]);
            }
            BWmap->addPar("Alloc_ID_4");
            BWmap->addPar("grant_size_4");
            BWmap->par("Alloc_ID_4").setLongValue(i*10+4);
            BWmap->par("grant_size_4").setLongValue(grant_size[i*10+4]);
            //BW MAP generation for TCONT Type5
            BWmap->addPar("grant_size_5");
            BWmap->par("grant_size_5").setLongValue(grant_size[i*10+5]);
            sendDelayed(BWmap,To,"out");
        }
        //Round Robin scheduling of start ONU
        RR+=1;
        if(RR==onu_id_tot)
            RR=0;
        scheduleAt(simTime()+T,generate_BWmap);
    }
}

void OLT:: generate_grant(int TCONT_id)
{
    onu_id=RR;
    //Grant size generation
    while(1)
    {
        int alloc_id=onu_id*10+TCONT_id;
        if(SI_Timer[alloc_id]==0)
        {
            DBRu[alloc_id]=true;
            Report_ID[alloc_id]=0;
            Flag_grnt[alloc_id]=true;
            last_grnt_time[alloc_id]=simTime().dbl()+To;
            report_count[alloc_id]=1;
        }
        else
            DBRu[alloc_id]=false;
        grant_size[alloc_id]=get_grant_size(alloc_id);
        if(grant_size[alloc_id]>0)
            counter+=1;
        if(Flag_grnt[alloc_id]==true)
        {
            Grant_size[alloc_id]+=grant_size[alloc_id];
        }
        //Condition to grant an extra DBRu field to ONUs/TCONTs to send their report
        if(grant_size[alloc_id]>0 && report_count[alloc_id]<2)
        {
            stat_flag[alloc_id]=true;
            report_count[alloc_id]+=1;
        }
        if(SI_Timer[alloc_id]!=0 && stat_flag[alloc_id]==true)
        {
            DBRu[alloc_id]=true;
            Report_ID[alloc_id]=1;
            Flag_ext_grnt[alloc_id]=true;
            stat_flag[alloc_id]=false;
        }
        if(Flag_ext_grnt[alloc_id]==true)
        {
            Grant_size_ext[alloc_id]+=grant_size[alloc_id];
        }
        onu_id+=1;
        if(onu_id==onu_id_tot)
            onu_id=0;
        if(onu_id==RR)
            break;
    }
    //Update operation of VBs
    onu_id=RR;
    while(1)
    {
        update_VB(onu_id*10+TCONT_id);
        onu_id+=1;
        if(onu_id==onu_id_tot)
            onu_id=0;
        if(onu_id==RR)
            break;
    }
}

long OLT::get_grant_size(int id)
{
    long grant_size=0;
    long type=id%10;
    if(VB[id]>0 && FB>0)
    {
        grant_size=fmin(fmin(w_max[type-2],buf_size[id]),FB);
        VB[id]-=grant_size;
        buf_size[id]-=grant_size;
        FB-=grant_size;
    }
    return grant_size;
}

void OLT:: update_VB(int id)
{
    //Calculation of sum of VBs of TCONTs whose SI_Timer has expired
    Sk=0;
    int type=id%10;
    for(int i=0;i<onu_id_tot;i++)
    {
        if(SI_Timer[i*10+type]==0 && VB[i*10+type]>0)
            Sk+=VB[i*10+type];
    }
    if(VB[id]<0 && Sk>0)
    {
        Sk=Sk+VB[id];
        VB[id]=fmin(0,Sk);
    }
    if(SI_Timer[id]==0)
    {
        int type=id%10;
        if(type==2)
            SI_Timer[id]=SI_2;
        if(type==3)
            SI_Timer[id]=SI_3;
        if(type==4)
            SI_Timer[id]=SI_4;
        VB[id]=fmin(w_max[type-2]+VB[id],w_max[type-2]);
    }
    SI_Timer[id]-=1;
}
