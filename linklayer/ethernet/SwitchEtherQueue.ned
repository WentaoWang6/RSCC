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

simple SwitchEtherQueue extends BaseQueue
{
    parameters:
        @class(mine::SwitchEtherQueue);
        bool enablePFC = default(false); // about PFC
        int pauseThreshold = default(500); // KB
        int unpauseThreshold = default(100); // KB
        
        // about rate 
        bool congCtrl = default(false); // Enable L3FCN flow control
        int linkBandwidth = default(40); // Gbps : deprecated
        int queueThreshold = default(100); //KB       
        int rateThreshold = default(100); // Mbps
        int timeThreshold = default(20); // RTT in us
        int rtt = default(20); // in us
        double minAlpha = default(0.9);
        
        @signal[queueSize](type=long);
        @statistic[queueSize](title="queue size"; record=max,timeavg,vector,histogram);
        
        //rate
        @signal[rate](type=long);
        @statistic[rate](title="rate"; record=max,timeavg,vector,histogram);
        @signal[rateMsgBw](type=long);
        @statistic[rateMsgBw](title="rate msg bandwidth"; record=max,timeavg,vector,histogram);
        
        //ecn
        @signal[markProbability](type=double);
        @statistic[markProbability](title="mark proability"; record=max,timeavg,vector,histogram);
        @signal[spotProbability](type=double);
        @statistic[spotProbability](title="spot proability"; record=max,timeavg,vector,histogram);
        
    gates:
        input oobL3In;
        output oobL3Out;

}