
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//
network XGPON
{
    parameters:
        int no_onu;
        @display("bgb=1080.54,755.59503,white");
    types:
        channel C1 extends ned.DatarateChannel
        {
            delay = 100 us;
        }
        channel C2 extends ned.DatarateChannel
        {
            delay = 100 us;
        }
    submodules:
        olt: OLT {
            @display("p=96.57,302.76;i=device/router,#808000;is=l");
        }

        Splitter: PowerSplitter {
            @display("p=523.305,302.76");
        }
        //        onu[no_onu]: ONU {
        //            @display("p=156.92308,160.76924,ri,50,50");
        //        }
        //        
        onu: ONU {
            @display("p=473.715,87.435005,ri,50,50");
        }

        RRH: Traffic_Generator {
            @display("p=468.495,30.015001");
        }
        onu1: ONU {
            @display("p=609.435,109.62");
        }
        RRH1: Traffic_Generator {
            @display("p=591.16504,20.880001");
        }
        RRH2: Traffic_Generator {
            @display("p=746.46,45.675");
        }
        RRH3: Traffic_Generator {
            @display("p=887.4,88.74");
        }
        RRH5: Traffic_Generator {
            @display("p=965.7,283.185");
        }
        RRH6: Traffic_Generator {
            @display("p=348.435,45.675");
        }
        RRH7: Traffic_Generator {
            @display("p=253.17,88.74");
        }
        RRH8: Traffic_Generator {
            @display("p=965.7,395.415");
        }
        RRH9: Traffic_Generator {
            @display("p=930.465,510.255");
        }
        RRH10: Traffic_Generator {
            @display("p=887.4,601.605");
        }
        RRH11: Traffic_Generator {
            @display("p=810.405,665.55");
        }
        RRH12: Traffic_Generator {
            @display("p=652.5,685.125");
        }
        RRH13: Traffic_Generator {
            @display("p=531.135,664.245");
        }
        RRH14: Traffic_Generator {
            @display("p=396.72,664.245");
        }
        RRH15: Traffic_Generator {
            @display("p=288.405,600.3");
        }
        onu2: ONU {
            @display("p=703.395,109.62");
        }
        onu3: ONU {
            @display("p=783,135.72");
        }
        onu4: ONU {
            @display("p=844.335,199.66501");
        }
        RRH4: Traffic_Generator {
            @display("p=939.60004,174.87001");
        }
        onu5: ONU {
            @display("p=844.335,281.88");
        }
        onu6: ONU {
            @display("p=424.125,109.62");
        }
        onu7: ONU {
            @display("p=334.08002,153.99");
        }
        onu8: ONU {
            @display("p=844.335,365.4");
        }
        onu9: ONU {
            @display("p=833.895,448.92");
        }
        onu10: ONU {
            @display("p=789.525,529.83");
        }
        onu11: ONU {
            @display("p=715.14,563.76");
        }
        onu12: ONU {
            @display("p=609.435,580.72504");
        }
        onu13: ONU {
            @display("p=498.51,563.76");
        }
        onu14: ONU {
            @display("p=404.55002,529.83");
        }
        onu15: ONU {
            @display("p=324.945,495.9");
        }
        olt1: OLT {
            @display("p=318.42,301.45502;is=n;i=old/switch2");
        }
    //        source[no_onu]: Traffic_Generator {
    //            @display("p=,,ri,110,90;i=block/app2");
    //        }
    connections allowunconnected:

        RRH.out --> onu.in_source;
        onu.out_olt --> Splitter.inOnu++;
        RRH1.out --> onu1.in_source;
        onu1.out_olt --> Splitter.inOnu++;
        Splitter.outOnu++ --> onu.in_olt;
        Splitter.outOnu++ --> onu1.in_olt;
        RRH2.out --> onu2.in_source;
        onu2.out_olt --> Splitter.inOnu++;
        Splitter.outOnu++ --> onu2.in_olt;
        RRH3.out --> onu3.in_source;
        onu3.out_olt --> Splitter.inOnu++;
        Splitter.outOnu++ --> onu3.in_olt;
        onu4.out_olt --> Splitter.inOnu++;
        Splitter.outOnu++ --> onu4.in_olt;
        RRH4.out --> onu4.in_source;
        RRH5.out --> onu5.in_source;
        onu5.out_olt --> Splitter.inOnu++;
        Splitter.outOnu++ --> onu5.in_olt;
        RRH8.out --> onu8.in_source;
        RRH9.out --> onu9.in_source;
        RRH10.out --> onu10.in_source;
        RRH11.out --> onu11.in_source;
        RRH12.out --> onu12.in_source;
        RRH13.out --> onu13.in_source;
        RRH14.out --> onu14.in_source;
        RRH15.out --> onu15.in_source;
        RRH6.out --> onu6.in_source;
        RRH7.out --> onu7.in_source;
        Splitter.outOnu++ --> onu7.in_olt;
        onu7.out_olt --> Splitter.inOnu++;
        onu6.out_olt --> Splitter.inOnu++;
        Splitter.outOnu++ --> onu6.in_olt;
        Splitter.outOnu++ --> onu8.in_olt;
        onu8.out_olt --> Splitter.inOnu++;
        onu9.out_olt --> Splitter.inOnu++;
        Splitter.outOnu++ --> onu9.in_olt;
        onu10.out_olt --> Splitter.inOnu++;
        Splitter.outOnu++ --> onu10.in_olt;
        onu11.out_olt --> Splitter.inOnu++;
        Splitter.outOnu++ --> onu11.in_olt;
        onu12.out_olt --> Splitter.inOnu++;
        Splitter.outOnu++ --> onu12.in_olt;
        onu13.out_olt --> Splitter.inOnu++;
        Splitter.outOnu++ --> onu13.in_olt;
        onu14.out_olt --> Splitter.inOnu++;
        Splitter.outOnu++ --> onu14.in_olt;
        onu15.out_olt --> Splitter.inOnu++;
        Splitter.outOnu++ --> onu15.in_olt;
        Splitter.outOLT --> olt1.in;
        olt1.out --> { @display("ls=blue,2"); } --> Splitter.inOLT;
        olt.out --> olt1.inBBU;
        olt1.outBBU --> { @display("ls=#FF8000,2"); } --> olt.in;
}

