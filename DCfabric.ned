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

network DCFabric
{
    parameters:
        int numGenerators;
        int numSinks;
        @display("bgb=910,567");
    types:
        channel ethChannel extends ThruputMeteringChannel
        {
            thruputDisplayFormat = "u";
        }
    submodules:
        configurator: IPv4NetworkConfigurator {
            @display("p=48,31");
        }
        spines[3]: Switch {
            @display("p=150,50,r,300");
        }
        leaves[3]: Switch {
            @display("p=150,150,r,300");
        }
        gens1[numGenerators]: Host {
            @display("p=50,250,m,4,75");
        }
        gens2[numGenerators]: Host {
            @display("p=350,250,m,4,75");
        }
        sinks[numSinks]: Host {
            @display("p=650,250,m,4,75");
        }
    connections:
        for i=0..2 {
            spines[i].ethg++ <--> ethChannel {  datarate = 100Gbps; delay = 1.5us; } <--> leaves[0].ethg++;
            spines[i].ethg++ <--> ethChannel {  datarate = 100Gbps; delay = 1.5us; } <--> leaves[0].ethg++;
            spines[i].ethg++ <--> ethChannel {  datarate = 100Gbps; delay = 1.5us; } <--> leaves[1].ethg++;
            spines[i].ethg++ <--> ethChannel {  datarate = 100Gbps; delay = 1.5us; } <--> leaves[1].ethg++;
            spines[i].ethg++ <--> ethChannel {  datarate = 100Gbps; delay = 1.5us; } <--> leaves[2].ethg++;
            spines[i].ethg++ <--> ethChannel {  datarate = 100Gbps; delay = 1.5us; } <--> leaves[2].ethg++;
        }
        for i=0..numGenerators-1 {
            gens1[i].ethg++ <--> ethChannel {  datarate = 40Gbps; delay = 1.5us; } <--> leaves[0].ethg++;
        }
        for i=0..numGenerators-1 {
            gens2[i].ethg++ <--> ethChannel {  datarate = 40Gbps; delay = 1.5us; } <--> leaves[1].ethg++;
        }
        for i=0..numSinks-1 {
            sinks[i].ethg++ <--> ethChannel {  datarate = 40Gbps; delay = 1.5us; } <--> leaves[2].ethg++;
        }
}