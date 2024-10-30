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
simple OLT
{
    parameters:
        int window_max_Type2;
        int window_max_Type3;
        int window_max_Type4;
        int No_ONU;
        int service_interval_Type2;
        int service_interval_Type3;
        int service_interval_Type4;
        double up_rate;
        double down_rate;

        @display("i=old/switch1");
    gates:
        input in;
        output out;
        input inBBU;
        output outBBU;
}
