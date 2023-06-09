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

package linklayer.ethernet;

import common.queue.BaseQueue;

simple HostEtherQueue extends BaseQueue
{
    
    parameters:
        @class(mine::HostEtherQueue);
        bool enableLossRecovery = default(false);
        int channelCapacity = default(75);  // KB
        int rtt @unit(us) = default(0); // us
        
        int maxRate = default(40000); //Mbps
        //int networkDelay = default(0); // us one way delay  in dcqcn
        //int linkBandwidth = default(40); // Gbps in dcqcn
        
    gates:
        input oobL3In;
        output oobL3Out;
            
}






