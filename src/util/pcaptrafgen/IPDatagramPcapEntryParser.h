//
// Copyright (C) 2011 OpenSim Ltd
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//
// @author Zoltan Bojthe
//

#ifndef __INET_UTIL_IPDATAGRAMPCAPENTRYPARSER_H
#define __INET_UTIL_IPDATAGRAMPCAPENTRYPARSER_H


#include "PcapEntryParserIf.h"

/**
 * This class is a pcap entry parser.
 * It's convert an entry to an INET cPacket.
 * @see PcapEntryParserIf, InetPcapFileReader, PcapTrafficGenerator
 */
class IPDatagramPcapEntryParser : public PcapEntryParserIf
{
  public:
    virtual ~IPDatagramPcapEntryParser() {};
    virtual cPacket* parse(const unsigned char *buf, uint32_t caplen, uint32_t totlen);
};


#endif // __INET_UTIL_IPDATAGRAMPCAPENTRYPARSER_H
