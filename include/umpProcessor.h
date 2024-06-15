/**********************************************************
 * MIDI 2.0 Library 
 * Author: Andrew Mee
 * 
 * MIT License
 * Copyright 2021 Andrew Mee
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 * ********************************************************/
#ifndef UMP_PROCESSOR_H
#define UMP_PROCESSOR_H

#include <cstdint>
#include <array>
#include <functional>

#include "utils.h"

struct umpCVM{
    umpCVM() : umpGroup(255), messageType(255), status(0),channel(255),note(255), value(0),  index(0), bank(0),
         flag1(false), flag2(false) {}
    uint8_t umpGroup;
    uint8_t messageType;
    uint8_t status;
    uint8_t channel;
    uint8_t note;
    uint32_t value;
    uint16_t index;
    uint8_t bank;
    bool flag1;
    bool flag2;
};

struct umpGeneric{
    umpGeneric() : umpGroup(255), status(0),  value(0) {}
    uint8_t umpGroup;
    uint8_t messageType;
    uint8_t status;
    uint16_t value;
};

struct umpData{
    umpData() : umpGroup(255), streamId(0), status(0),  form(0) {}
    uint8_t umpGroup;
    uint8_t messageType;
    uint8_t streamId;
    uint8_t status;
    uint8_t form;
    uint8_t* data;
    uint8_t dataLength;
};

class umpProcessor{
public:
    using utilityMessageFn = std::function<void(umpGeneric mess)>;
    using channelVoiceMessageFn = std::function<void(umpCVM mess)>;
    using systemMessageFn = std::function<void(umpGeneric mess)>;
    using sendOutSysexFn = std::function<void(umpData mess)>;

    using flexTempoFn = std::function<void(uint8_t group, uint32_t num10nsPQN)>;
    using flexTimeSigFn = std::function<void(uint8_t group, uint8_t numerator, uint8_t denominator,
                                             uint8_t num32Notes)>;
    using flexMetronomeFn =
        std::function<void(uint8_t group, uint8_t numClkpPriCli, uint8_t bAccP1, uint8_t bAccP2,
                           uint8_t bAccP3, uint8_t numSubDivCli1, uint8_t numSubDivCli2)>;
    using flexKeySigFn = std::function<void(uint8_t group, uint8_t addrs, uint8_t channel,
                                            uint8_t sharpFlats, uint8_t tonic)>;
    using flexChordFn = std::function<void(
        uint8_t group, uint8_t addrs, uint8_t channel, uint8_t chShrpFlt, uint8_t chTonic,
        uint8_t chType, uint8_t chAlt1Type, uint8_t chAlt1Deg, uint8_t chAlt2Type,
        uint8_t chAlt2Deg, uint8_t chAlt3Type, uint8_t chAlt3Deg, uint8_t chAlt4Type,
        uint8_t chAlt4Deg, uint8_t baShrpFlt, uint8_t baTonic, uint8_t baType, uint8_t baAlt1Type,
        uint8_t baAlt1Deg, uint8_t baAlt2Type, uint8_t baAlt2Deg)>;
    using flexPerformanceFn = std::function<void(umpData mess, uint8_t addrs, uint8_t channel)>;
    using flexLyricFn = std::function<void(umpData mess, uint8_t addrs, uint8_t channel)>;

    using midiEndpointFn = std::function<void(uint8_t majVer, uint8_t minVer, uint8_t filter)>;
    using midiEndpointNameFn = std::function<void(umpData mess)>;
    using midiEndpointProcIdFn = std::function<void(umpData mess)>;
    using midiEndpointJRProtocolReqFn = std::function<void(uint8_t protocol, bool jrrx, bool jrtx)>;
    using midiEndpointInfoFn =
        std::function<void(uint8_t majVer, uint8_t minVer, uint8_t numOfFuncBlocks, bool m2,
                           bool m1, bool rxjr, bool txjr)>;
    using midiEndpointDeviceInfoFn =
        std::function<void(std::array<uint8_t, 3> manuId, std::array<uint8_t, 2> familyId,
                           std::array<uint8_t, 2> modelId, std::array<uint8_t, 4> version)>;
    using midiEndpointJRProtocolNotifyFn =
        std::function<void(uint8_t protocol, bool jrrx, bool jrtx)>;

    using functionBlockFn = std::function<void(uint8_t fbIdx, uint8_t filter)>;
    using functionBlockInfoFn = std::function<void(
        uint8_t fbIdx, bool active, uint8_t direction, bool sender, bool recv, uint8_t firstGroup,
        uint8_t groupLength, uint8_t midiCIVersion, uint8_t isMIDI1, uint8_t maxS8Streams)>;
    using functionBlockNameFn = std::function<void(umpData mess, uint8_t fbIdx)>;

    using startOfSeqFn = std::function<void()>;
    using endOfFileFn = std::function<void()>;

    using unknownUMPMessageFn = std::function<void(uint32_t* ump, uint8_t length)>;

    void clearUMP();
    void processUMP(uint32_t UMP);

    //-----------------------Handlers ---------------------------
    void setUtility(utilityMessageFn fptr) { utilityMessage = fptr; }
    void setCVM(channelVoiceMessageFn fptr) { channelVoiceMessage = fptr; }
    void setSystem(systemMessageFn fptr) { systemMessage = fptr; }
    void setSysEx(sendOutSysexFn fptr) { sendOutSysex = fptr; }

    //---------- Flex Data
    void setFlexTempo(flexTempoFn fptr) { flexTempo = fptr; }
    void setFlexTimeSig(flexTimeSigFn fptr) { flexTimeSig = fptr; }
    void setFlexMetronome(flexMetronomeFn fptr) { flexMetronome = fptr; }
    void setFlexKeySig(flexKeySigFn fptr) { flexKeySig = fptr; }
    void setFlexChord(flexChordFn fptr) { flexChord = fptr; }
    void setFlexPerformance(flexPerformanceFn fptr) { flexPerformance = fptr; }
    void setFlexLyric(flexLyricFn fptr) { flexLyric = fptr; }

    //---------- UMP Stream
    void setMidiEndpoint(midiEndpointFn fptr) { midiEndpoint = fptr; }
    void setMidiEndpointNameNotify(midiEndpointNameFn fptr) { midiEndpointName = fptr; }
    void setMidiEndpointProdIdNotify(midiEndpointProcIdFn fptr) { midiEndpointProdId = fptr; }
    void setMidiEndpointInfoNotify(midiEndpointInfoFn fptr) { midiEndpointInfo = fptr; }
    void setMidiEndpointDeviceInfoNotify(midiEndpointDeviceInfoFn fptr) {
        midiEndpointDeviceInfo = fptr;
    }

    void setJRProtocolRequest(midiEndpointJRProtocolReqFn fptr) {
        midiEndpointJRProtocolReq = fptr;
    }
    void setJRProtocolNotify(midiEndpointJRProtocolNotifyFn fptr) {
        midiEndpointJRProtocolNotify = fptr;
    }

    void setFunctionBlock(functionBlockFn fptr) { functionBlock = fptr; }
    void setFunctionBlockNotify(functionBlockInfoFn fptr) { functionBlockInfo = fptr; }
    void setFunctionBlockNameNotify(functionBlockNameFn fptr) { functionBlockName = fptr; }

    void setStartOfSeq(startOfSeqFn fptr) { startOfSeq = fptr; }
    void setEndOfFile(endOfFileFn fptr) { endOfFile = fptr; }

    // Unknown UMP
    void setUnknownUMP(unknownUMPMessageFn fptr) { unknownUMPMessage = fptr; }

private:
    uint32_t umpMess[4]{};
    uint8_t messPos = 0;

    utilityMessageFn utilityMessage;            // Message type 0x0 callbacks
    channelVoiceMessageFn channelVoiceMessage;  // MIDI 1 and 2 CVM callback
    systemMessageFn systemMessage;              // System Messages callbacks
    sendOutSysexFn sendOutSysex;                // Sysex

    // Message Type 0xD callbacks
    flexTempoFn flexTempo;
    flexTimeSigFn flexTimeSig;
    flexMetronomeFn flexMetronome;
    flexKeySigFn flexKeySig;
    flexChordFn flexChord;
    flexPerformanceFn flexPerformance;
    flexLyricFn flexLyric;

    // Message Type 0xF callbacks
    midiEndpointFn midiEndpoint;
    midiEndpointInfoFn midiEndpointInfo;
    midiEndpointDeviceInfoFn midiEndpointDeviceInfo;
    midiEndpointNameFn midiEndpointName;
    midiEndpointProcIdFn midiEndpointProdId;
    midiEndpointJRProtocolReqFn midiEndpointJRProtocolReq;
    midiEndpointJRProtocolNotifyFn midiEndpointJRProtocolNotify;

    functionBlockFn functionBlock;
    functionBlockInfoFn functionBlockInfo;
    functionBlockNameFn functionBlockName;
    startOfSeqFn startOfSeq;
    endOfFileFn endOfFile;

    // Handle new Messages
    unknownUMPMessageFn unknownUMPMessage;
};

#endif //UMP_PROCESSOR_H
