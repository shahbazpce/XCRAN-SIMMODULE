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

module ONU
{
    @display("i=device/smallrouter,#169EA9;bgb=,,white");
    gates:
        //    input in_user;
        input in_olt;
        output out_olt;
        input in_source;
     //   output out_source;
    submodules:
        in_gate: IN_GATE {
            @display("p=320,118;i=block/rx");
        }
        out_gate: OUT_GATE {
            @display("p=59,99;i=block/classifier");
        }
        TCONT_2: TCONT_2 {
            @display("p=196,137;i=old/queue");
        }
        TCONT_3: TCONT_3 {
            @display("p=193,41;i=old/queue");
        }
        TCONT_4: TCONT_4 {
            @display("p=194,220;i=old/queue");
        }
    //        }
    connections:
        //      in_gate.in <-- in_user;
        in_gate.out1 --> TCONT_2.in1;
        in_gate.out2 --> TCONT_3.in1;
        in_gate.out3 --> TCONT_4.in1;
        TCONT_2.out --> out_gate.i1;
        TCONT_2.in2 <-- out_gate.o1;
        TCONT_3.out --> out_gate.i2;
        TCONT_3.in2 <-- out_gate.o2;
        TCONT_4.out --> out_gate.i3;
        TCONT_4.in2 <-- out_gate.o3;
        out_gate.i4 <-- in_olt;
        out_gate.o4 --> out_olt;
        in_source --> in_gate.in;
}
