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

package networklayer.ipv4;

import common.networklayer.ipv4.L3RelayUnit;

simple L3RelayUnitEx extends L3RelayUnit
{
    parameters:
        @class(mine::L3RelayUnitEx);
        bool enableLossRecovery = default(false);
        bool enablePFC = default(false);
        int pauseThreshold = default(500); // KB
        int unpauseThreshold = default(100); // KB
        
        bool destination = default(false);
        
        int elephantThreshold = default(0); // KB
        int elephantInvldIntvl @unit(ms) = default(2ms);
        
        @signal[rateMsgFanout](type=long);
        @statistic[rateMsgFanout](title="rate msg fanout"; record=max,timeavg,vector,histogram);
}