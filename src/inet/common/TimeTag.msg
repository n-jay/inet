//
// This program is property of its copyright holder. All rights reserved.
//

import inet.common.INETDefs;
import inet.common.TagBase;

namespace inet;

class CreationTimeTag extends TagBase
{
    simtime_t creationTime = simTime();
}

class ElapsedTimeTag extends TagBase
{
    simtime_t startTime = simTime();
}

class TimeTagBase extends TagBase
{
    simtime_t totalTime;
}

class DelayingTimeTag extends TimeTagBase
{
}

class QueueingTimeTag extends TimeTagBase
{
}

class ProcessingTimeTag extends TimeTagBase
{
}

class TransmissionTimeTag extends TimeTagBase
{
}

class PropagationTimeTag extends TimeTagBase
{
}
