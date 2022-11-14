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

class umpProcessor{
    
  private:
  
	uint32_t umpMess[4]{};
	uint8_t messPos=0;

    // Message type 0x0  callbacks
    void (*recvNOOP)() = nullptr;
    void (*recvJRClock)(uint16_t timing) = nullptr;
    void (*recvJRTimeStamp)(uint16_t timestamp) = nullptr;
    void (*recvDCTTickPQN)(uint16_t timestamp) = nullptr;
    void (*recvDCTickLastEv)(uint16_t timestamp) = nullptr;


    // MIDI 1 and 2 CVM  callbacks
    void (*midiNoteOff)(uint8_t group, uint8_t mt, uint8_t channel, uint8_t noteNumber, uint16_t velocity, uint8_t attributeType,
            uint16_t attributeData) = nullptr;
    void (*midiNoteOn)(uint8_t group, uint8_t mt, uint8_t channel, uint8_t noteNumber, uint16_t velocity, uint8_t attributeType,
            uint16_t attributeData) = nullptr;
    void (*controlChange)(uint8_t group, uint8_t mt, uint8_t channel, uint8_t index, uint32_t value) = nullptr;
    void (*rpn)(uint8_t group, uint8_t channel, uint8_t bank, uint8_t index, uint32_t value) = nullptr;
    void (*nrpn)(uint8_t group, uint8_t channel, uint8_t bank, uint8_t index, uint32_t value) = nullptr;
    void (*rnrpn)(uint8_t group, uint8_t channel, uint8_t bank, uint8_t index, int32_t value) = nullptr;
    void (*rrpn)(uint8_t group, uint8_t channel, uint8_t bank, uint8_t index, int32_t value) = nullptr;
    void (*polyPressure)(uint8_t group, uint8_t mt, uint8_t channel, uint8_t noteNumber, uint32_t pressure) = nullptr;
    void (*perNotePB)(uint8_t group, uint8_t channel, uint8_t noteNumber, uint32_t pitch) = nullptr;
    void (*nrpnPerNote)(uint8_t group, uint8_t channel, uint8_t noteNumber, uint8_t index, uint32_t value) = nullptr;
    void (*rpnPerNote)(uint8_t group, uint8_t channel, uint8_t noteNumber, uint8_t index, uint32_t value) = nullptr;
    void (*perNoteManage)(uint8_t group, uint8_t channel, uint8_t noteNumber, bool detach, bool reset) = nullptr;
    void (*channelPressure)(uint8_t group, uint8_t mt, uint8_t channel, uint32_t pressure) = nullptr;
    void (*pitchBend)(uint8_t group, uint8_t mt, uint8_t channel, uint32_t value) = nullptr;
    void (*programChange)(uint8_t group, uint8_t mt, uint8_t channel, uint8_t program, bool bankValid, uint8_t bank,
            uint8_t index) = nullptr;

    //System Messages  callbacks
    void (*timingCode)(uint8_t group, uint8_t timeCode) = nullptr;
    void (*songSelect)(uint8_t group, uint8_t song) = nullptr;
    void (*songPositionPointer)(uint8_t group, uint16_t position) = nullptr;
    void (*tuneRequest)(uint8_t group) = nullptr;
    void (*timingClock)(uint8_t group) = nullptr;
    void (*seqStart)(uint8_t group) = nullptr;
    void (*seqCont)(uint8_t group) = nullptr;
    void (*seqStop)(uint8_t group) = nullptr;
    void (*activeSense)(uint8_t group) = nullptr;
    void (*systemReset)(uint8_t group) = nullptr;

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
    void (*flexPerformance)(uint8_t group, uint8_t form, uint8_t addrs, uint8_t channel, uint8_t status, uint8_t* text,
            uint8_t textLength) = nullptr;
    void (*flexLyric)(uint8_t group, uint8_t form, uint8_t addrs, uint8_t channel, uint8_t status, uint8_t* text,
                            uint8_t textLength) = nullptr;

    // Message Type 0xF  callbacks
    void (*midiEndpoint)(uint8_t majVer, uint8_t minVer, uint8_t filter) = nullptr;
    void (*functionBlock)(uint8_t fbIdx, uint8_t filter) = nullptr;
    void (*midiEndpointInfo)(uint8_t majVer, uint8_t minVer, uint8_t numFuncBlocks, bool m2, bool m1, bool rxjr, bool txjr)
            = nullptr;
    void (*midiEndpointDeviceInfo)(std::array<uint8_t, 3> manuId, std::array<uint8_t, 2> familyId,
                             std::array<uint8_t, 2> modelId, std::array<uint8_t, 4> version) = nullptr;
    void (*midiEndpointName)(uint8_t form, uint8_t nameLength, uint8_t* name) = nullptr;
    void (*midiEndpointProdId)(uint8_t form, uint8_t prodIdLength, uint8_t* prodId) = nullptr;
    void (*midiEndpointJRProtocolReq)(uint8_t protocol, bool jrrx, bool jrtx) = nullptr;
    void (*midiEndpointJRProtocolNotify)(uint8_t protocol, bool jrrx, bool jrtx) = nullptr;
    void (*functionBlockInfo)(uint8_t fbIdx, bool active,
            uint8_t direction, uint8_t firstGroup, uint8_t groupLength,
            bool midiCIValid, uint8_t midiCIVersion, uint8_t isMIDI1, uint8_t maxS8Streams) = nullptr;
    void (*functionBlockName)(uint8_t fbIdx, uint8_t form, uint8_t nameLength, uint8_t* name) = nullptr;
    void (*startOfSeq)() = nullptr;
    void (*endOfFile)() = nullptr;

    void (*sendOutSysex)(uint8_t group, uint8_t *sysex ,uint8_t length, uint8_t state) = nullptr;
    
  public:

	void clearUMP();
    void processUMP(uint32_t UMP);

	//-----------------------Handlers ---------------------------
    inline void setJRClock(void (*fptr)(uint16_t timing)){ recvJRClock = fptr;}
    inline void setJRTimeStamp(void (*fptr)(uint16_t timestamp)){ recvJRTimeStamp = fptr;}
    inline void setDCTickPQN(void (*fptr)( uint16_t timing)){ recvDCTTickPQN = fptr;}
    inline void setJDCSinceLastEvent(void (*fptr)(uint16_t timestamp)){ recvDCTickLastEv = fptr;}

	inline void setNoteOff(void (*fptr)(uint8_t group, uint8_t mt ,uint8_t channel, uint8_t noteNumber, uint16_t velocity,
            uint8_t attributeType, uint16_t attributeData)){ midiNoteOff = fptr; }
	inline void setNoteOn(void (*fptr)(uint8_t group, uint8_t mt ,uint8_t channel, uint8_t noteNumber, uint16_t velocity,
            uint8_t attributeType, uint16_t attributeData)){ midiNoteOn = fptr; }
	inline void setControlChange(void (*fptr)(uint8_t group, uint8_t mt, uint8_t channel, uint8_t index, uint32_t value)){
        controlChange = fptr; }
	inline void setRPN(void (*fptr)(uint8_t group, uint8_t channel,uint8_t bank,  uint8_t index, uint32_t value)){
        rpn = fptr; }
	inline void setNRPN(void (*fptr)(uint8_t group, uint8_t channel,uint8_t bank,  uint8_t index, uint32_t value)){
        nrpn = fptr; }
	inline void setRelativeNRPN(void (*fptr)(uint8_t group, uint8_t channel,uint8_t bank,  uint8_t index,
            int32_t value/*twoscomplement*/)){ rnrpn = fptr; }
	inline void setRelativeRPN(void (*fptr)(uint8_t group, uint8_t channel,uint8_t bank,  uint8_t index,
            int32_t value/*twoscomplement*/)){ rrpn = fptr; }
	inline void setPolyPressure(void (*fptr)(uint8_t group, uint8_t mt, uint8_t channel, uint8_t noteNumber, uint32_t pressure)){
        polyPressure = fptr; }

    inline void setRpnPerNote(void (*fptr)(uint8_t group, uint8_t channel, uint8_t noteNumber, uint8_t index,
            uint32_t value)){rpnPerNote = fptr; }
    inline void setNrpnPerNote(void (*fptr)(uint8_t group, uint8_t channel, uint8_t noteNumber, uint8_t index,
            uint32_t value)){nrpnPerNote = fptr; }

    inline void setPerNoteManage(void (*fptr)(uint8_t group, uint8_t channel, uint8_t noteNumber,
            bool detach, bool reset)){perNoteManage = fptr; }
    inline void setPerNotePB(void (*fptr)(uint8_t group, uint8_t channel, uint8_t noteNumber,
                                              uint32_t value)){perNotePB = fptr; }

	inline void setChannelPressure(void (*fptr)(uint8_t group, uint8_t mt, uint8_t channel, uint32_t pressure)){
        channelPressure = fptr; }
	inline void setPitchBend(void (*fptr)(uint8_t group, uint8_t mt, uint8_t channel, uint32_t value)){ pitchBend = fptr; }
	inline void setProgramChange(void (*fptr)(uint8_t group, uint8_t mt, uint8_t channel, uint8_t program, bool bankValid,
            uint8_t bank, uint8_t index)){ programChange = fptr; }
	//TODO per note etc

	inline void setTimingCode(void (*fptr)(uint8_t group, uint8_t timeCode)){ timingCode = fptr; }
	inline void setSongSelect(void (*fptr)(uint8_t group,uint8_t song)){ songSelect = fptr; }
	inline void setSongPositionPointer(void (*fptr)(uint8_t group,uint16_t position)){
        songPositionPointer = fptr; }
	inline void setTuneRequest(void (*fptr)(uint8_t group)){ tuneRequest = fptr; }
	inline void setTimingClock(void (*fptr)(uint8_t group)){ timingClock = fptr; }
	inline void setSeqStart(void (*fptr)(uint8_t group)){ seqStart = fptr; }
	inline void setSeqCont(void (*fptr)(uint8_t group)){ seqCont = fptr; }
	inline void setSeqStop(void (*fptr)(uint8_t group)){ seqStop = fptr; }
	inline void setActiveSense(void (*fptr)(uint8_t group)){ activeSense = fptr; }
	inline void setSystemReset(void (*fptr)(uint8_t group)){ systemReset = fptr; }


	inline void setMidiEndpoint(void (*fptr)(uint8_t majVer, uint8_t minVer, uint8_t filter)){
        midiEndpoint = fptr; }
	inline void setMidiEndpointNameNotify(void (*fptr)(uint8_t form, uint8_t nameLength, uint8_t* name)){
        midiEndpointName = fptr; }
    inline void setMidiEndpointProdIdNotify(void (*fptr)(uint8_t form, uint8_t nameLength, uint8_t* name)){
        midiEndpointProdId = fptr; }
	inline void setMidiEndpointInfoNotify(void (*fptr)(uint8_t majVer, uint8_t minVer, uint8_t numOfFuncBlocks, bool m2, bool m1, bool rxjr,
            bool txjr)){
        midiEndpointInfo = fptr; }
    inline void setMidiEndpointDeviceInfoNotify(void (*fptr)(std::array<uint8_t, 3> manuId, std::array<uint8_t, 2> familyId,
            std::array<uint8_t, 2> modelId, std::array<uint8_t, 4> version)){
        midiEndpointDeviceInfo = fptr; }
    inline void setJRProtocolReq(void (*fptr)(uint8_t protocol, bool jrrx, bool jrtx)){ midiEndpointJRProtocolReq = fptr;}
    inline void setJRProtocolNotify(void (*fptr)(uint8_t protocol, bool jrrx, bool jrtx)){
        midiEndpointJRProtocolNotify = fptr;}

    inline void setFunctionBlock(void (*fptr)(uint8_t filter, uint8_t fbIdx)){ functionBlock = fptr; }
    inline void setFunctionBlockNotify(void (*fptr)(uint8_t fbIdx, bool active,
                            uint8_t direction, uint8_t firstGroup, uint8_t groupLength,
                            bool midiCIValid, uint8_t midiCIVersion, uint8_t isMIDI1, uint8_t maxS8Streams)){ functionBlockInfo = fptr; }
    inline void setFunctionBlockNotify(void (*fptr)(uint8_t fbIdx, uint8_t form, uint8_t nameLength, uint8_t* name)){
        functionBlockName = fptr; }



	inline void setRawSysEx(void (*fptr)(uint8_t group, uint8_t *sysex ,uint8_t numbytes, uint8_t state)){
        sendOutSysex = fptr; }
};

#endif //UMP_PROCESSOR_H
