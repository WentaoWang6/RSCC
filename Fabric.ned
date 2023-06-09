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

import inet.common.misc.ThruputMeteringChannel;
import inet.networklayer.configurator.ipv4.IPv4NetworkConfigurator;
import node.Host;
import node.Switch;

network Fabric
{
    parameters:
        int numGenerators;
        @display("bgb=583,409");
    types:
        channel ethChannel extends ThruputMeteringChannel
        {
            thruputDisplayFormat = "u";
        }
    submodules:
        generators[numGenerators]: Host {
            @display("p=100,200,m,4,100");
        }
        sink: Host {
            @display("p=500,100");
        }
        switch: Switch {
            @display("p=350,100");
        }
        configurator: IPv4NetworkConfigurator {
            @display("p=48,31");
        }
    connections:
        for i=0..numGenerators-1 {
            generators[i].ethg++ <--> ethChannel {  datarate = 40Gbps; delay = 1.5us; } <--> switch.ethg++;
        }
        switch.ethg++ <--> ethChannel {  datarate = 40Gbps; delay = 1.5us; } <--> sink.ethg++;
}