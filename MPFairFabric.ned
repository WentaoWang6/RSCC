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

network MPFairFabric
{
	types:
        channel ethChannel extends ThruputMeteringChannel
        {
            thruputDisplayFormat = "u";
        }
    
    submodules:
        host1: Host {
           @display("p=100,400");
        } 
        host2: Host {
           @display("p=100,200");
        } 
        host3: Host {
           @display("p=400,200");
        } 
        sink1: Host {
           @display("p=400,400");
        } 
        sink2: Host {
           @display("p=600,300");
        } 
        sw1: Switch {
            @display("p=200,300");
        }
        sw2: Switch {
            @display("p=400,300");
        }
        configurator: IPv4NetworkConfigurator {
            @display("p=48,31");
        }
        
	connections:
	    sw1.ethg++ <--> ethChannel {  datarate = 40Gbps; delay = 1us; } <--> sw2.ethg++;
	    host1.ethg++ <--> ethChannel { datarate = 40Gbps; delay = 1us; } <--> sw1.ethg++;
	    host2.ethg++ <--> ethChannel { datarate = 40Gbps; delay = 1us; } <--> sw1.ethg++;
	    host3.ethg++ <--> ethChannel { datarate = 40Gbps; delay = 1us; } <--> sw2.ethg++;
	    sw2.ethg++ <--> ethChannel { datarate = 40Gbps; delay = 1us; } <--> sink1.ethg++;
	    sw2.ethg++ <--> ethChannel{ datarate = 40Gbps; delay = 1us; } <--> sink2.ethg++;
	    

}