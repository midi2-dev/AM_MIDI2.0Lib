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

#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <array>

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
    umpData() : umpGroup(255), status(0),  form(0) {}
    uint8_t umpGroup;
    uint8_t messageType;
    uint8_t status;
    uint8_t form;
    uint8_t* data;
    uint8_t dataLength;
};

class umpProcessor{
    
  private:
  
	uint32_t umpMess[4]{};
	uint8_t messPos=0;

    // Message type 0x0  callbacks
    void (*utilityMessage)(struct umpGeneric mess) = nullptr;

	// MIDI 1 and 2 CVM  callbacks
    void (*channelVoiceMessage)(struct umpCVM mess) = nullptr;
    
   //System Messages  callbacks
    void (*systemMessage)(struct umpGeneric mess) = nullptr;

    //Sysex
    void (*sendOutSysex)(struct umpData mess) = nullptr;

    // Message Type 0xD  callbacks
    void (*flexTempo)(uint8_t group, uint32_t num10nsPQN) = nullptr;
    void (*flexTimeSig)(uint8_t group, uint8_t numerator, uint8_t denominator, uint8_t num32Notes) = nullptr;
    void (*flexMetronome)(uint8_t group, uint8_t numClkpPriCli, uint8_t bAccP1, uint8_t bAccP2, uint8_t bAccP3,
            uint8_t numSubDivCli1, uint8_t numSubDivCli2) = nullptr;
    void (*flexKeySig)(uint8_t group, uint8_t addrs, uint8_t channel, uint8_t sharpFlats, uint8_t tonic) = nullptr;
    void (*flexChord)(uint8_t group, uint8_t addrs, uint8_t channel, uint8_t chShrpFlt, uint8_t chTonic,
            uint8_t chType, uint8_t chAlt1Type, uint8_t chAlt1Deg, uint8_t chAlt2Type, uint8_t chAlt2Deg,
            uint8_t chAlt3Type, uint8_t chAlt3Deg, uint8_t chAlt4Type, uint8_t chAlt4Deg, uint8_t baShrpFlt, uint8_t baTonic,
            uint8_t baType, uint8_t baAlt1Type, uint8_t baAlt1Deg, uint8_t baAlt2Type, uint8_t baAlt2Deg) = nullptr;
    void (*flexPerformance)(struct umpData mess, uint8_t addrs, uint8_t channel) = nullptr;
    void (*flexLyric)(struct umpData mess, uint8_t addrs, uint8_t channel) = nullptr;

    // Message Type 0xF  callbacks
    void (*midiEndpoint)(uint8_t majVer, uint8_t minVer, uint8_t filter) = nullptr;
    void (*functionBlock)(uint8_t fbIdx, uint8_t filter) = nullptr;
    void (*midiEndpointInfo)(uint8_t majVer, uint8_t minVer, uint8_t numFuncBlocks, bool m2, bool m1, bool rxjr, bool txjr)
            = nullptr;
    void (*midiEndpointDeviceInfo)(std::array<uint8_t, 3> manuId, std::array<uint8_t, 2> familyId,
                             std::array<uint8_t, 2> modelId, std::array<uint8_t, 4> version) = nullptr;
    void (*midiEndpointName)(struct umpData mess) = nullptr;
    void (*midiEndpointProdId)(struct umpData mess) = nullptr;

    void (*midiEndpointJRProtocolReq)(uint8_t protocol, bool jrrx, bool jrtx) = nullptr;
    void (*midiEndpointJRProtocolNotify)(uint8_t protocol, bool jrrx, bool jrtx) = nullptr;

    void (*functionBlockInfo)(uint8_t fbIdx, bool active,
            uint8_t direction, bool sender, bool recv, uint8_t firstGroup, uint8_t groupLength,
            uint8_t midiCIVersion, uint8_t isMIDI1, uint8_t maxS8Streams) = nullptr;
    void (*functionBlockName)(struct umpData mess, uint8_t fbIdx) = nullptr;
    void (*startOfSeq)() = nullptr;
    void (*endOfFile)() = nullptr;
    
  public:

	void clearUMP();
    void processUMP(uint32_t UMP);

		//-----------------------Handlers ---------------------------
    inline void setUtility(void (*fptr)(struct umpGeneric mess)){ utilityMessage = fptr; }
    inline void setCVM(void (*fptr)(struct umpCVM mess)){ channelVoiceMessage = fptr; }
    inline void setSystem(void (*fptr)(struct umpGeneric mess)){ systemMessage = fptr; }
    inline void setSysEx(void (*fptr)(struct umpData mess)){sendOutSysex = fptr; }

    //---------- Flex Data
    inline void setFlexTempo(void (*fptr)(uint8_t group, uint32_t num10nsPQN)){ flexTempo = fptr; }
    inline void setFlexTimeSig(void (*fptr)(uint8_t group, uint8_t numerator, uint8_t denominator, uint8_t num32Notes)){
        flexTimeSig = fptr; }
    inline void setFlexMetronome(void (*fptr)(uint8_t group, uint8_t numClkpPriCli, uint8_t bAccP1, uint8_t bAccP2, uint8_t bAccP3,
                          uint8_t numSubDivCli1, uint8_t numSubDivCli2)){ flexMetronome = fptr; }
    inline void setFlexKeySig(void (*fptr)(uint8_t group, uint8_t addrs, uint8_t channel, uint8_t sharpFlats, uint8_t tonic)){
        flexKeySig = fptr; }
    inline void setFlexChord(void (*fptr)(uint8_t group, uint8_t addrs, uint8_t channel, uint8_t chShrpFlt, uint8_t chTonic,
                      uint8_t chType, uint8_t chAlt1Type, uint8_t chAlt1Deg, uint8_t chAlt2Type, uint8_t chAlt2Deg,
                      uint8_t chAlt3Type, uint8_t chAlt3Deg, uint8_t chAlt4Type, uint8_t chAlt4Deg, uint8_t baShrpFlt, uint8_t baTonic,
                      uint8_t baType, uint8_t baAlt1Type, uint8_t baAlt1Deg, uint8_t baAlt2Type, uint8_t baAlt2Deg)){
        flexChord = fptr; }
    inline void setFlexPerformance(void (*fptr)(struct umpData mess, uint8_t addrs, uint8_t channel)){ flexPerformance = fptr; }
    inline void setFlexLyric(void (*fptr)(struct umpData mess, uint8_t addrs, uint8_t channel)){ flexLyric = fptr; }

    //---------- UMP Stream

	inline void setMidiEndpoint(void (*fptr)(uint8_t majVer, uint8_t minVer, uint8_t filter)){
        midiEndpoint = fptr; }
	inline void setMidiEndpointNameNotify(void (*fptr)(struct umpData mess)){
        midiEndpointName = fptr; }
    inline void setMidiEndpointProdIdNotify(void (*fptr)(struct umpData mess)){
        midiEndpointProdId = fptr; }
	inline void setMidiEndpointInfoNotify(void (*fptr)(uint8_t majVer, uint8_t minVer, uint8_t numOfFuncBlocks, bool m2,
            bool m1, bool rxjr, bool txjr)){
        midiEndpointInfo = fptr; }
    inline void setMidiEndpointDeviceInfoNotify(void (*fptr)(std::array<uint8_t, 3> manuId, std::array<uint8_t, 2> familyId,
            std::array<uint8_t, 2> modelId, std::array<uint8_t, 4> version)){
        midiEndpointDeviceInfo = fptr; }
    inline void setJRProtocolRequest(void (*fptr)(uint8_t protocol, bool jrrx, bool jrtx)){ midiEndpointJRProtocolReq = fptr;}
    inline void setJRProtocolNotify(void (*fptr)(uint8_t protocol, bool jrrx, bool jrtx)){
        midiEndpointJRProtocolNotify = fptr;}

    inline void setFunctionBlock(void (*fptr)(uint8_t filter, uint8_t fbIdx)){ functionBlock = fptr; }
    inline void setFunctionBlockNotify(void (*fptr)(uint8_t fbIdx, bool active,
                            uint8_t direction, bool sender, bool recv, uint8_t firstGroup, uint8_t groupLength,
                            uint8_t midiCIVersion, uint8_t isMIDI1, uint8_t maxS8Streams)){
        functionBlockInfo = fptr; }
    inline void setFunctionBlockNameNotify(void (*fptr)(struct umpData mess, uint8_t fbIdx)){functionBlockName = fptr; }
    inline void setStartOfSeq(void (*fptr)()){startOfSeq = fptr; }
    inline void setEndOfFile(void (*fptr)()){endOfFile = fptr; }



};

#endif //UMP_PROCESSOR_H
