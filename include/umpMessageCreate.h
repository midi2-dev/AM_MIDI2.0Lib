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

#ifndef MESSAGE_CREATE_H
#define MESSAGE_CREATE_H
#include <array>
#include <cstdint>
#include "utils.h"

namespace UMPMessage {

    inline uint32_t m1Create(uint8_t group, uint8_t status, uint8_t val1, uint8_t val2){
        return (((UMP_SYSTEM << 4) + (group & 0xF) + 0L) << 24)
        + (((status & 0xFF) + 0L) << 16)
        + (((val1  & 0x7F) + 0L) << 8) + (val2  & 0x7F);
    }

    inline uint32_t mt2Create(uint8_t group,  uint8_t status, uint8_t channel, uint8_t val1, uint8_t val2){
        uint32_t message;
        message = ((UMP_M1CVM << 4) + (group & 0xF) + 0L) << 24;
        message +=  ((status & 0xF0) + (channel & 0xF)  + 0L) << 16;
        message +=  ((int)val1 & 0x7F)  << 8;
        message += val2  & 0x7F;
        return message;
    }

    inline uint32_t mt4CreateFirstWord(uint8_t group,  uint8_t status, uint8_t channel, uint8_t val1, uint8_t val2){
        uint32_t message;
        message = ((UMP_M2CVM << 4) + (group & 0xF) + 0L) << 24;
        message +=  ((status & 0xF0) + (channel & 0xF)  + 0L) << 16;
        message +=  (int)val1  << 8;
        message += val2;
        return message;
    }

    inline uint32_t mt0NOOP(){
        return 0;
    }

    inline uint32_t mt0JRClock(uint16_t clockTime){
        return  ((UTILITY_JRCLOCK + 0L) << 20) + clockTime;
    }

    inline uint32_t mt0JRTimeStamp(uint16_t timestamp){
        return ((UTILITY_JRTS + 0L) << 20) + timestamp;
    }

    inline uint32_t mt0DeltaClockTick(uint16_t ticksPerQtrNote){
        return ((UTILITY_DELTACLOCKTICK + 0L) << 20) + ticksPerQtrNote;
    }

    inline uint32_t mt0DeltaTicksSinceLast(uint16_t noTicksSince){
        return ((UTILITY_DELTACLOCKSINCE + 0L) << 20) + noTicksSince;
    }

    inline uint32_t mt1MTC(uint8_t group, uint8_t timeCode){
        return m1Create(group, TIMING_CODE, timeCode, 0);
    }

    inline uint32_t mt1SPP(uint8_t group, uint16_t position){
        return m1Create(group, SPP, position & 0x7F , (position >> 7) & 0x7F );
    }

    inline uint32_t mt1SongSelect(uint8_t group, uint8_t song){
        return m1Create(group, SONG_SELECT, song, 0 );
    }

    inline uint32_t mt1TuneRequest(uint8_t group){
        return m1Create(group, TUNEREQUEST, 0, 0 );
    }

    inline uint32_t mt1TimingClock(uint8_t group){
        return m1Create(group, TIMINGCLOCK, 0, 0 );
    }

    inline uint32_t mt1SeqStart(uint8_t group){
        return m1Create(group, SEQSTART, 0, 0 );
    }

    inline uint32_t mt1SeqCont(uint8_t group){
        return m1Create(group, SEQCONT, 0, 0 );
    }

    inline uint32_t mt1SeqStop(uint8_t group){
        return m1Create(group, SEQSTOP, 0, 0 );
    }

    inline uint32_t mt1ActiveSense(uint8_t group){
        return m1Create(group, ACTIVESENSE, 0, 0 );
    }

    inline uint32_t mt1SystemReset(uint8_t group){
        return m1Create(group, SYSTEMRESET, 0, 0 );
    }


    inline uint32_t mt2NoteOn(uint8_t group, uint8_t channel, uint8_t noteNumber, uint8_t velocity){
        return mt2Create(group,  NOTE_ON, channel, noteNumber , velocity );
    }
    inline uint32_t mt2NoteOff(uint8_t group, uint8_t channel, uint8_t noteNumber, uint8_t velocity){
        return mt2Create(group,  NOTE_OFF, channel, noteNumber, velocity );
    }

    inline uint32_t mt2PolyPressure(uint8_t group, uint8_t channel, uint8_t noteNumber, uint8_t pressure){
        return mt2Create(group,  KEY_PRESSURE, channel , noteNumber, pressure );
    }
    inline uint32_t mt2CC(uint8_t group, uint8_t channel, uint8_t index, uint8_t value){
        return mt2Create(group,  CC, channel, index , value );
    }
    inline uint32_t mt2ProgramChange(uint8_t group, uint8_t channel, uint8_t program){
        return mt2Create(group,  PROGRAM_CHANGE, channel, program, 0);
    }
    inline uint32_t mt2ChannelPressure(uint8_t group, uint8_t channel, uint8_t pressure){
        return mt2Create(group,  CHANNEL_PRESSURE, channel, pressure, 0);
    }
    inline uint32_t mt2PitchBend(uint8_t group, uint8_t channel, uint16_t value){
        return mt2Create(group,  PITCH_BEND, channel, value & 0x7F, (value >> 7) & 0x7F);
    }

    inline std::array<uint32_t, 2> mt3Sysex7(uint8_t group, uint8_t status, uint8_t numBytes, std::array<uint8_t, 6> sx){
        std::array<uint32_t, 2> umpMess = {0,0};
        umpMess[0] = (0x3 << 28) + (group << 24) + (status << 20)+ (numBytes << 16);
        if(numBytes > 0 ) umpMess[0] += (sx[0] << 8);
        if(numBytes > 1 ) umpMess[0] += sx[1];
        if(numBytes > 2 ) umpMess[1] += (sx[2] << 24);
        if(numBytes > 3 ) umpMess[1] += (sx[3] << 16);
        if(numBytes > 4 ) umpMess[1] += (sx[4] << 8);
        if(numBytes > 5 ) umpMess[1] += sx[5];

        return umpMess;
    }




    inline std::array<uint32_t, 2> mt4NoteOn(uint8_t group, uint8_t channel, uint8_t noteNumber, uint16_t velocity, uint8_t attributeType, uint16_t attributeData){
        std::array<uint32_t, 2> umpMess  = {0,0};
	    umpMess[0] = mt4CreateFirstWord(group,  NOTE_ON, channel, noteNumber, attributeType);
	    umpMess[1] = velocity << 16;
	    umpMess[1] += attributeData;
	    return umpMess;
    }

    inline std::array<uint32_t, 2> mt4NoteOff(uint8_t group, uint8_t channel, uint8_t noteNumber, uint16_t velocity, uint8_t attributeType, uint16_t attributeData){
        std::array<uint32_t, 2> umpMess  = {0,0};
	    umpMess[0] = mt4CreateFirstWord(group,  NOTE_OFF, channel, noteNumber, attributeType);
	    umpMess[1] = velocity << 16;
	    umpMess[1] += attributeData;
	    return umpMess;
    }

inline std::array<uint32_t, 2> mt4CPolyPressure(uint8_t group, uint8_t channel, uint8_t noteNumber, uint32_t pressure){
    std::array<uint32_t, 2> umpMess  = {0,0};
	umpMess[0] = mt4CreateFirstWord(group,  KEY_PRESSURE, channel, noteNumber, 0);
	umpMess[1] = pressure;
	return umpMess;
}

inline std::array<uint32_t, 2> mt4PitchBend(uint8_t group, uint8_t channel, uint32_t pitch){
    std::array<uint32_t, 2> umpMess  = {0,0};
	umpMess[0] = mt4CreateFirstWord(group,  PITCH_BEND, channel, 0, 0);
	umpMess[1] = pitch;
	return umpMess;
}

inline std::array<uint32_t, 2> mt4CC(uint8_t group, uint8_t channel, uint8_t index, uint32_t value){
    std::array<uint32_t, 2> umpMess  = {0,0};
	umpMess[0] = mt4CreateFirstWord(group,  CC , channel, index, 0);
	umpMess[1] = value;
	return umpMess;
}

inline std::array<uint32_t, 2> mt4RPN(uint8_t group, uint8_t channel,uint8_t bank,  uint8_t index, uint32_t value){
    std::array<uint32_t, 2> umpMess  = {0,0};;
	umpMess[0] = mt4CreateFirstWord(group,  RPN , channel, bank, index);
	umpMess[1] = value;
	return umpMess;
}

inline std::array<uint32_t, 2> mt4NRPN(uint8_t group, uint8_t channel,uint8_t bank,  uint8_t index, uint32_t value){
    std::array<uint32_t, 2> umpMess  = {0,0};
	umpMess[0] = mt4CreateFirstWord(group,  NRPN , channel, bank, index);
	umpMess[1] = value;
	return umpMess;
}


inline std::array<uint32_t, 2> mt4RelativeRPN(uint8_t group, uint8_t channel,uint8_t bank,  uint8_t index, int32_t value){
    std::array<uint32_t, 2> umpMess  = {0,0};
	umpMess[0] = mt4CreateFirstWord(group,  RPN_RELATIVE , channel, bank, index);
	umpMess[1] = (uint32_t)value;
	return umpMess;
}

inline std::array<uint32_t, 2> mt4RelativeNRPN(uint8_t group, uint8_t channel,uint8_t bank,  uint8_t index, int32_t value){
    std::array<uint32_t, 2> umpMess  = {0,0};
	umpMess[0] = mt4CreateFirstWord(group,  NRPN_RELATIVE , channel, bank, index);
	umpMess[1] = (uint32_t)value;
	return umpMess;
}

inline std::array<uint32_t, 2> mt4ChannelPressure(uint8_t group, uint8_t channel,uint32_t pressure){
    std::array<uint32_t, 2> umpMess  = {0,0};
	umpMess[0] = mt4CreateFirstWord(group,  CHANNEL_PRESSURE, channel, 0, 0);
	umpMess[1] = pressure;
	return umpMess;
}

inline std::array<uint32_t, 2> mt4ProgramChange(uint8_t group, uint8_t channel, uint8_t program, bool bankValid, uint8_t bank, uint8_t index){
	std::array<uint32_t, 2> umpMess  = {0,0};
	umpMess[0] = mt4CreateFirstWord(group,  PROGRAM_CHANGE, channel, 0, bankValid ? 1 : 0);
	umpMess[1] = ((uint32_t) program << 24) + (bankValid ? ((uint32_t) bank << 8) + index : 0);
	return umpMess;
}

    // Per-Note Expression (MIDI 2.0)
    inline std::array<uint32_t, 2> mt4PerNotePitchBend(uint8_t group, uint8_t channel,
                                                        uint8_t noteNumber, uint32_t pitch){
        std::array<uint32_t, 2> umpMess = {0,0};
        umpMess[0] = mt4CreateFirstWord(group, PITCH_BEND_PERNOTE, channel, noteNumber, 0);
        umpMess[1] = pitch;
        return umpMess;
    }

    inline std::array<uint32_t, 2> mt4PerNoteCC(uint8_t group, uint8_t channel,
                                                 uint8_t noteNumber, uint8_t index, uint32_t value){
        std::array<uint32_t, 2> umpMess = {0,0};
        umpMess[0] = mt4CreateFirstWord(group, NRPN_PERNOTE, channel, noteNumber, index);
        umpMess[1] = value;
        return umpMess;
    }

    inline std::array<uint32_t, 2> mt4PerNoteRPN(uint8_t group, uint8_t channel,
                                                  uint8_t noteNumber, uint8_t index, uint32_t value){
        std::array<uint32_t, 2> umpMess = {0,0};
        umpMess[0] = mt4CreateFirstWord(group, RPN_PERNOTE, channel, noteNumber, index);
        umpMess[1] = value;
        return umpMess;
    }

    inline std::array<uint32_t, 2> mt4PerNoteManage(uint8_t group, uint8_t channel,
                                                     uint8_t noteNumber, uint8_t optionFlags){
        std::array<uint32_t, 2> umpMess = {0,0};
        umpMess[0] = mt4CreateFirstWord(group, PERNOTE_MANAGE, channel, noteNumber, optionFlags);
        umpMess[1] = 0;
        return umpMess;
    }

    //TODO mt5 Sysex8
inline std::array<uint32_t, 4> mt5MDSHeader(uint8_t group, uint8_t mds, uint16_t numberOfBytes, uint16_t totalChunks, uint16_t chunkNumber,
                                            uint16_t manuId, uint16_t deviceId, uint16_t subId1, uint16_t subId2){
    std::array<uint32_t, 4> umpMess  = {0,0,0,0};
    umpMess[0] = (uint32_t) (0x5 << 28) + (((group & 0xF) + 0L) << 24) +  (0x8 << 20) + ((mds & 0xF) << 16)+numberOfBytes;
    umpMess[1] = ((totalChunks + 0L) << 16) +chunkNumber;
    umpMess[2] = ((manuId + 0L) << 16) +deviceId;
    umpMess[3] = ((subId1 + 0L) << 16) +subId2;

    return umpMess;
}

inline std::array<uint32_t, 4> mt5MDSPayload(uint8_t group, uint8_t mds, uint8_t * data, uint8_t dataLength){
        std::array<uint32_t, 4> umpMess  = {0,0,0,0};
        umpMess[0] = (uint32_t) (0x5 << 28) + (((group & 0xF) + 0L) << 24) +  (0x9 << 20) + ((mds & 0xF) << 16);
        int offset=0;
        if(offset < dataLength)umpMess[0] += (data[offset++] << 8);
        if(offset < dataLength)umpMess[0] += data[offset++];
        for(uint8_t i=1;i<4;i++){
            if(offset < dataLength)umpMess[i] += (data[offset++] << 24);
            if(offset < dataLength)umpMess[i] += (data[offset++] << 16);
            if(offset < dataLength)umpMess[i] += (data[offset++] << 8);
            if(offset < dataLength)umpMess[i] += data[offset++];
        }
        return umpMess;
    }

// Flex Data (MT=0xD) creation functions
// addrs: 0=Channel, 1=Group, 0xFF=NoChannel  form: 0=Complete, 1=Start, 2=Continue, 3=End

inline std::array<uint32_t, 4> mtDFlexData(uint8_t group, uint8_t form, uint8_t addrs,
                                            uint8_t channel, uint8_t statusBank, uint8_t status){
    std::array<uint32_t, 4> umpMess = {0,0,0,0};
    umpMess[0] = ((uint32_t)UMP_FLEX_DATA << 28)
               + (((uint32_t)group & 0xF) << 24)
               + (((uint32_t)form  & 0x3) << 22)
               + (((uint32_t)addrs & 0x3) << 20)
               + (((uint32_t)channel & 0xF) << 16)
               + (((uint32_t)statusBank & 0xFF) << 8)
               + ((uint32_t)status & 0xFF);
    return umpMess;
}

inline std::array<uint32_t, 4> mtDFlexTempo(uint8_t group, uint8_t addrs, uint8_t channel,
                                             uint32_t num10nsPQN){
    auto umpMess = mtDFlexData(group, 0, addrs, channel, FLEXDATA_COMMON, FLEXDATA_COMMON_TEMPO);
    umpMess[1] = num10nsPQN;
    return umpMess;
}

inline std::array<uint32_t, 4> mtDFlexTimeSig(uint8_t group, uint8_t addrs, uint8_t channel,
                                               uint8_t numerator, uint8_t denominator,
                                               uint8_t num32Notes){
    auto umpMess = mtDFlexData(group, 0, addrs, channel, FLEXDATA_COMMON, FLEXDATA_COMMON_TIMESIG);
    umpMess[1] = ((uint32_t)numerator  << 24)
               + ((uint32_t)denominator << 16)
               + ((uint32_t)num32Notes  <<  8);
    return umpMess;
}

inline std::array<uint32_t, 4> mtDFlexMetronome(uint8_t group, uint8_t addrs, uint8_t channel,
                                                 uint8_t numClkpPriCli, uint8_t bAccP1,
                                                 uint8_t bAccP2, uint8_t bAccP3,
                                                 uint8_t numSubDivCli1, uint8_t numSubDivCli2){
    auto umpMess = mtDFlexData(group, 0, addrs, channel, FLEXDATA_COMMON, FLEXDATA_COMMON_METRONOME);
    umpMess[1] = ((uint32_t)numClkpPriCli << 24)
               + ((uint32_t)bAccP1        << 16)
               + ((uint32_t)bAccP2        <<  8)
               + ((uint32_t)bAccP3);
    umpMess[2] = ((uint32_t)numSubDivCli1 << 24)
               + ((uint32_t)numSubDivCli2 << 16);
    return umpMess;
}

inline std::array<uint32_t, 4> mtDFlexKeySig(uint8_t group, uint8_t addrs, uint8_t channel,
                                              uint8_t sharpFlats, uint8_t tonic){
    auto umpMess = mtDFlexData(group, 0, addrs, channel, FLEXDATA_COMMON, FLEXDATA_COMMON_KEYSIG);
    umpMess[1] = ((uint32_t)sharpFlats << 24)
               + ((uint32_t)tonic      << 16);
    return umpMess;
}

inline std::array<uint32_t, 4> mtDFlexChord(uint8_t group, uint8_t addrs, uint8_t channel,
                                             uint8_t chShrpFlt, uint8_t chTonic,
                                             uint8_t chType,
                                             uint8_t chAlt1Type, uint8_t chAlt1Deg,
                                             uint8_t chAlt2Type, uint8_t chAlt2Deg,
                                             uint8_t chAlt3Type, uint8_t chAlt3Deg,
                                             uint8_t chAlt4Type, uint8_t chAlt4Deg,
                                             uint8_t baShrpFlt,  uint8_t baTonic,
                                             uint8_t baType,
                                             uint8_t baAlt1Type, uint8_t baAlt1Deg,
                                             uint8_t baAlt2Type, uint8_t baAlt2Deg){
    auto umpMess = mtDFlexData(group, 0, addrs, channel, FLEXDATA_COMMON, FLEXDATA_COMMON_CHORD);
    umpMess[1] = (((uint32_t)chShrpFlt  & 0xF) << 28)
               + (((uint32_t)chTonic    & 0xF) << 24)
               + (((uint32_t)chType     & 0xFF) << 16)
               + (((uint32_t)chAlt1Type & 0xF) << 12)
               + (((uint32_t)chAlt1Deg  & 0xF) <<  8)
               + (((uint32_t)chAlt2Type & 0xF) <<  4)
               + ( (uint32_t)chAlt2Deg  & 0xF);
    umpMess[2] = (((uint32_t)chAlt3Type & 0xF) << 28)
               + (((uint32_t)chAlt3Deg  & 0xF) << 24)
               + (((uint32_t)chAlt4Type & 0xF) << 20)
               + (((uint32_t)chAlt4Deg  & 0xF) << 16);
    umpMess[3] = (((uint32_t)baShrpFlt  & 0xF) << 28)
               + (((uint32_t)baTonic    & 0xF) << 24)
               + (((uint32_t)baType     & 0xFF) << 16)
               + (((uint32_t)baAlt1Type & 0xF) << 12)
               + (((uint32_t)baAlt1Deg  & 0xF) <<  8)
               + (((uint32_t)baAlt2Type & 0xF) <<  4)
               + ( (uint32_t)baAlt2Deg  & 0xF);
    return umpMess;
}

inline std::array<uint32_t, 4> mtDFlexTextData(uint8_t group, uint8_t form, uint8_t addrs,
                                                uint8_t channel, uint8_t statusBank,
                                                uint8_t status, uint8_t* text, uint8_t textLen){
    auto umpMess = mtDFlexData(group, form, addrs, channel, statusBank, status);
    uint8_t offset = 0;
    for(uint8_t i = 1; i <= 3; i++){
        for(int shift = 24; shift >= 0; shift -= 8){
            if(offset < textLen) umpMess[i] += ((uint32_t)text[offset++] << shift);
        }
    }
    return umpMess;
}

inline std::array<uint32_t, 4> mtDFlexPerformance(uint8_t group, uint8_t form, uint8_t addrs,
                                                   uint8_t channel, uint8_t status,
                                                   uint8_t* text, uint8_t textLen){
    return mtDFlexTextData(group, form, addrs, channel, FLEXDATA_PERFORMANCE, status, text, textLen);
}

inline std::array<uint32_t, 4> mtDFlexLyric(uint8_t group, uint8_t form, uint8_t addrs,
                                             uint8_t channel, uint8_t status,
                                             uint8_t* text, uint8_t textLen){
    return mtDFlexTextData(group, form, addrs, channel, FLEXDATA_LYRIC, status, text, textLen);
}

inline std::array<uint32_t, 4> mtFMidiEndpoint(uint8_t filter){
    std::array<uint32_t, 4> umpMess  = {0,0,0,0};
	umpMess[0] = (uint32_t) ((0xF << 28) + (UMP_VER_MAJOR << 8) +  UMP_VER_MINOR);
    umpMess[1] = filter;
	return umpMess;
}

inline std::array<uint32_t, 4> mtFMidiEndpointInfoNotify(uint8_t numOfFuncBlock, bool m2, bool m1, bool rxjr, bool txjr){
    std::array<uint32_t, 4> umpMess = {0,0,0,0};
    umpMess[0] = (uint32_t) ((0xF << 28) + (MIDIENDPOINT_INFO_NOTIFICATION << 16) + (UMP_VER_MAJOR << 8) +  UMP_VER_MINOR);
    umpMess[1] = (numOfFuncBlock << 24)
            + (m2 << 9)
            + (m1 << 8)
            + (rxjr << 1)
            + txjr;
    return umpMess;
}

inline std::array<uint32_t, 4> mtFMidiEndpointDeviceInfoNotify(std::array<uint8_t, 3> manuId, std::array<uint8_t, 2> familyId,
                                                  std::array<uint8_t, 2> modelId, std::array<uint8_t, 4> version){
    std::array<uint32_t, 4> umpMess = {0,0,0,0};
    umpMess[0] = (uint32_t) ((0xF << 28) + (MIDIENDPOINT_DEVICEINFO_NOTIFICATION << 16)) /*+  numOfFuncBlock*/;

    umpMess[1] = (manuId[0] << 16)
                  + (manuId[1] << 8)
                  + manuId[2] ;
    umpMess[2] = (familyId[0] << 24)
                 + (familyId[1] << 16)
                 + (modelId[0] << 8)
                 + modelId[1] ;
    umpMess[3] = (version[0] << 24)
            +(version[1] << 16)
            +(version[2] << 8)
            +version[3];
    return umpMess;
}

inline std::array<uint32_t, 4> mtFMidiEndpointTextNotify(uint16_t replyType, uint8_t offset, uint8_t* text, uint8_t textLen){
    std::array<uint32_t, 4> umpMess = {0,0,0,0};
    uint8_t form = 0;
    if(offset==0){
        if(textLen>14)form = 1;
    }else{
        if(offset+13 < textLen){
            form = 2;
        }else{
            form = 3;
        }
    }
    umpMess[0] = (0xF << 28) + (form << 26) + (replyType << 16);
    if(offset < textLen)umpMess[0] += (text[offset++] << 8);
    if(offset < textLen)umpMess[0] += text[offset++];
    for(uint8_t i=1;i<4;i++){
        if(offset < textLen)umpMess[i] += (text[offset++] << 24);
        if(offset < textLen)umpMess[i] += (text[offset++] << 16);
        if(offset < textLen)umpMess[i] += (text[offset++] << 8);
        if(offset < textLen)umpMess[i] += text[offset++];
    }
    return umpMess;
}

inline std::array<uint32_t, 4> mtFFunctionBlock(uint8_t fbIdx, uint8_t filter ){
    std::array<uint32_t, 4> umpMess = {0,0,0,0};
    umpMess[0] = (0xF << 28) + (FUNCTIONBLOCK << 16) + (fbIdx << 8) + filter;
    return umpMess;
}

inline std::array<uint32_t, 4> mtFFunctionBlockInfoNotify(uint8_t fbIdx, bool active, uint8_t direction,
                                                 bool sender, bool recv, uint8_t firstGroup, uint8_t groupLength,
                                                 uint8_t midiCISupport, uint8_t isMIDI1, uint8_t maxS8Streams){
    std::array<uint32_t, 4> umpMess = {0,0,0,0};
    umpMess[0] = (0xF << 28) + (FUNCTIONBLOCK_INFO_NOTFICATION << 16)
                + ((active?1:0) << 15)
                + (fbIdx << 8)
                + (recv << 4)
                + (sender << 5)
                + (isMIDI1 << 2)
                + direction;
    umpMess[1] = (firstGroup  << 24)
                 + (groupLength << 16)
                 + (midiCISupport << 8)
                 + maxS8Streams;
    return umpMess;
}

inline std::array<uint32_t, 4> mtFFunctionBlockNameNotify(uint8_t fbIdx, uint8_t offset, uint8_t* text, uint8_t textLen){
    std::array<uint32_t, 4> umpMess = {0,0,0,0};
    uint8_t form = 0;
    if(offset==0){
        if(textLen>13)form = 1;
    }else{
        if(offset+12 < textLen){
            form = 2;
        }else{
            form = 3;
        }
    }
    umpMess[0] = (0xF << 28) + (form << 26) + (FUNCTIONBLOCK_NAME_NOTIFICATION << 16)+ (fbIdx << 8);
    if(offset < textLen)umpMess[0] += text[offset++];
    for(uint8_t i=1;i<4;i++){
        if(offset < textLen)umpMess[i] += (text[offset++] << 24);
        if(offset < textLen)umpMess[i] += (text[offset++] << 16);
        if(offset < textLen)umpMess[i] += (text[offset++] << 8);
        if(offset < textLen)umpMess[i] += text[offset++];
    }
    return umpMess;
}

inline std::array<uint32_t, 4> mtFStartOfSeq(){
    std::array<uint32_t, 4> umpMess = {0,0,0,0};
    umpMess[0] = (uint32_t) ((0xF << 28) + (STARTOFSEQ << 16));
    return umpMess;

}

inline std::array<uint32_t, 4> mtFEndOfFile(){
    std::array<uint32_t, 4> umpMess = {0,0,0,0};
    umpMess[0] = (uint32_t) ((0xF << 28) + (ENDOFFILE << 16));
    return umpMess;
}


inline std::array<uint32_t, 4> mtFRequestProtocol(uint8_t protocol, bool jrrx, bool jrtx){
    std::array<uint32_t, 4> umpMess  = {0,0,0,0};
    umpMess[0] = (0xF << 28) +  (MIDIENDPOINT_STREAMCONFIG_REQUEST << 16)
            + (protocol << 8)
              + (jrrx << 1)
              + jrtx;
    return umpMess;

}

inline std::array<uint32_t, 4> mtFNotifyProtocol( uint8_t protocol, bool jrrx, bool jrtx){
    std::array<uint32_t, 4> umpMess  = {0,0,0,0};
    umpMess[0] = (0xF << 28) +  (MIDIENDPOINT_STREAMCONFIG_NOTIFICATION << 16)
                 + (protocol << 8)
                 + (jrrx << 1)
                 + jrtx;
    return umpMess;
}

}
#endif

