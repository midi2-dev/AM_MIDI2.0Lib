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

#include "../include/umpProcessor.h"

void umpProcessor::clearUMP(){
    messPos = 0;
    umpMess[0]=0;
    umpMess[1]=0;
    umpMess[2]=0;
    umpMess[3]=0;
}

void umpProcessor::processUMP(uint32_t UMP){
	umpMess[messPos] = UMP;
		
	uint8_t mt = (umpMess[0] >> 28)  & 0xF;
	uint8_t group = (umpMess[0] >> 24) & 0xF;

	
	if(messPos == 0
        && (mt <= UMP_M1CVM || mt==0x6 || mt==0x7)
            ){ //32bit Messages

		if(mt == UMP_UTILITY){ //32 bits Utility Messages
			uint8_t status = (umpMess[0] >> 20) & 0xF;
			uint16_t timing = (umpMess[0] >> 16) & 0xFFFF;
			
			switch(status){
				case UTILITY_NOOP: // NOOP
				if(recvNOOP != nullptr) recvNOOP();
				break;
				case UTILITY_JRCLOCK: // JR Clock Message
					if(recvJRClock != nullptr) recvJRClock(timing);
					break;
				case UTILITY_JRTS: //JR Timestamp Message
					if(recvJRTimeStamp != nullptr) recvJRTimeStamp(timing);
					break;
                case UTILITY_DELTACLOCKTICK: //
                    if(recvDCTTickPQN != nullptr) recvDCTTickPQN(timing);
                    break;
                case UTILITY_DELTACLOCKSINCE: //
                    if(recvDCTickLastEv != nullptr) recvDCTickLastEv(timing);
                    break;
			}
			
		} else 
		if(mt == UMP_SYSTEM){ //32 bits System Real Time and System Common Messages (except System Exclusive)
			//Send notice
			uint8_t status = umpMess[0] >> 16 & 0xFF;
			switch(status){
				case TIMING_CODE:
				{
					uint8_t timing = (umpMess[0] >> 8) & 0x7F;
					if(timingCode != nullptr) timingCode(group, timing);
				}
				break;
				case SPP:
				{
					uint16_t position = ((umpMess[0] >> 8) & 0x7F)  + ((umpMess[0] & 0x7F) << 7);
					if(songPositionPointer != nullptr) songPositionPointer(group, position);
				}
				break;
				case SONG_SELECT:
				{
					uint8_t song = (umpMess[0] >> 8) & 0x7F;
					if(songSelect != nullptr) songSelect(group, song);
				}
				break;
				case TUNEREQUEST:
				    if(tuneRequest != nullptr) tuneRequest(group);
				break;
				case TIMINGCLOCK:
				    if(timingClock != nullptr) timingClock(group);
				break;
				case SEQSTART:
				    if(seqStart != nullptr) seqStart(group);
				break;
				case SEQCONT:
				    if(seqCont != nullptr) seqCont(group);
				break;
				case SEQSTOP:
				    if(seqStop != nullptr) seqStop(group);
				break;
				case ACTIVESENSE:
				    if(activeSense != nullptr) activeSense(group);
				break;
				case SYSTEMRESET:
				    if(systemReset != nullptr) systemReset(group);
				break;
			}
		
		} else 
		if(mt == UMP_M1CVM){ //32 Bits MIDI 1.0 Channel Voice Messages
			uint8_t status = umpMess[0] >> 16 & 0xF0;
			uint8_t channel = (umpMess[0] >> 16) & 0xF;
			uint8_t val1 = (umpMess[0] >> 8) & 0x7F;
			uint8_t val2 = umpMess[0] & 0x7F;

			switch(status){
				case NOTE_OFF: //Note Off
					if(midiNoteOff != nullptr) midiNoteOff(group, (uint8_t)UMP_M1CVM, channel, val1, scaleUp(val2,7,16), 0, 0);
					break;
				case NOTE_ON: //Note On
					if(midiNoteOn != nullptr) midiNoteOn(group, (uint8_t)UMP_M1CVM, channel, val1, scaleUp(val2,7,16), 0, 0);
					break;
				case KEY_PRESSURE: //Poly Pressure
					if(polyPressure != nullptr) polyPressure(group, (uint8_t)UMP_M1CVM, channel, val1, scaleUp(val2,7,32));
					break;	
				case CC: //CC
					if(controlChange != nullptr) controlChange(group, (uint8_t)UMP_M1CVM, channel, val1, scaleUp(val2,7,32));
					break;
				case PROGRAM_CHANGE: //Program Change Message
					if(programChange != nullptr) programChange(group, (uint8_t)UMP_M1CVM, channel, val1, false, 0, 0);
					break;
				case CHANNEL_PRESSURE: //Channel Pressure
					if(channelPressure != nullptr) channelPressure(group, (uint8_t)UMP_M1CVM, channel, scaleUp(val1,7,32));
					break;
				case PITCH_BEND: //PitchBend
					if(pitchBend != nullptr) pitchBend(group, (uint8_t)UMP_M1CVM, channel, scaleUp((val2 << 7) + val1,14,32));
					break;		
			}				
		}
        return;
		
	}else		
	if(messPos == 1
       && (mt == UMP_SYSEX7 || mt == UMP_M2CVM || mt==0x8 || mt==0x9  || mt==0xA)
        ){ //64bit Messages
		if(mt == UMP_SYSEX7){ //64 bits Data Messages (including System Exclusive)
			uint8_t numBytes  = (umpMess[0] >> 16) & 0xF;
			uint8_t status = (umpMess[0] >> 20) & 0xF;
            uint8_t sysex[6];

			if(numBytes > 0)sysex[0] =  (umpMess[0] >> 8) & 0x7F;
			if(numBytes > 1)sysex[1] =  umpMess[0] & 0x7F;
			if(numBytes > 2)sysex[2] =  (umpMess[1] >> 24) & 0x7F;
			if(numBytes > 3)sysex[3] =  (umpMess[1] >> 16) & 0x7F;
			if(numBytes > 4)sysex[4] =  (umpMess[1] >> 8) & 0x7F;
			if(numBytes > 5)sysex[5] =  umpMess[1] & 0x7F;

            if(sendOutSysex)sendOutSysex(group,sysex,numBytes,status);

		} else 
		if(mt == UMP_M2CVM){//64 bits MIDI 2.0 Channel Voice Messages
		
			uint8_t status = (umpMess[0] >> 16) & 0xF0;
			uint8_t channel = (umpMess[0] >> 16) & 0xF;
			uint8_t val1 = (umpMess[0] >> 8) & 0xFF;
			uint8_t val2 = umpMess[0] & 0xFF;
			
			switch(status){
				case NOTE_OFF: //Note Off
					if(midiNoteOff != nullptr) midiNoteOff(group, (uint8_t)UMP_M2CVM, channel, val1, umpMess[1] >> 16, val2, umpMess[1] & 65535);
					break;
				
				case NOTE_ON: //Note On
					if(midiNoteOn != nullptr) midiNoteOn(group, (uint8_t)UMP_M2CVM, channel, val1, umpMess[1] >> 16, val2, umpMess[1] & 65535);
					break;
					
				case KEY_PRESSURE: //Poly Pressure
					if(polyPressure != nullptr) polyPressure(group, (uint8_t)UMP_M2CVM, channel, val1, umpMess[1]);
					break;	
				
				case CC: //CC
					if(controlChange != nullptr) controlChange(group, (uint8_t)UMP_M2CVM, channel, val1, umpMess[1]);
					break;	
				
				case RPN: //RPN
					if(rpn != nullptr) rpn(group, channel, val1, val2, umpMess[1]);
					break;	
				
				case NRPN: //NRPN
					if(nrpn != nullptr) nrpn(group, channel, val1, val2, umpMess[1]);
					break;	
				
				case RPN_RELATIVE: //Relative RPN
					if(rrpn != nullptr) rrpn(group, channel, val1, val2, (int32_t)umpMess[1]/*twoscomplement*/);
					break;	

				case NRPN_RELATIVE: //Relative NRPN
					if(rnrpn != nullptr) rnrpn(group, channel, val1, val2, (int32_t)umpMess[1]/*twoscomplement*/);
					break;
				
				case PROGRAM_CHANGE: //Program Change Message
					if(programChange != nullptr) programChange(group, (uint8_t)UMP_M2CVM, channel, umpMess[1] >> 24, umpMess[0] & 1 , (umpMess[1] >> 8) & 0x7f , umpMess[1] & 0x7f);
					break;

				case CHANNEL_PRESSURE: //Channel Pressure
					if(channelPressure != nullptr) channelPressure(group, (uint8_t)UMP_M2CVM, channel, umpMess[1]);
					break;

				case PITCH_BEND: //PitchBend
					if(pitchBend != nullptr) pitchBend(group, (uint8_t)UMP_M2CVM, channel, umpMess[1]);
					break;	
					
				case PITCH_BEND_PERNOTE: //Per Note PitchBend 6
					if(perNotePB != 0) perNotePB(group, channel, val1, umpMess[1]);
					break;

				case NRPN_PERNOTE: //Assignable Per-Note Controller 1
                    if(nrpnPerNote != nullptr) nrpnPerNote(group, channel, val1, val2, umpMess[1]);
					break;	
					
				case RPN_PERNOTE: //Registered Per-Note Controller 0 
                    if(rpnPerNote != nullptr) rpnPerNote(group, channel, val1, val2, umpMess[1]);
					break;	
					
				case PERNOTE_MANAGE: //Per-Note Management Message
                    if(perNoteManage != nullptr) perNoteManage(group, channel, val1, (bool)(val2 & 2), (bool)(val2 & 1));
					break;	
					
			}
		}
        messPos =0;
	}else		
    if(messPos == 2
       && (mt == 0xB || mt == 0xC)
            ){ //96bit Messages
        messPos =0;
    }else
    if(messPos == 3
             && (mt == UMP_DATA || mt >= 0xD)
    ){ //128bit Messages

        if(mt == UMP_MIDI_ENDPOINT) { //128 bits UMP Stream Messages
            uint16_t status = (umpMess[0] >> 16) & 0x3FF;
            //uint8_t form = umpMess[0] >> 24 & 0x3;


            switch(status) {
                case MIDIENDPOINT: {
                    if (midiEndpoint != nullptr) midiEndpoint(
                            (umpMess[0]>>8) & 0xFF, //Maj Ver
                            umpMess[0] & 0xFF,  //Min Ver
                            umpMess[1] & 0xFF); //Filter
                    break;
                }
                case MIDIENDPOINT_INFO_NOTIFICATION:{
                    if (midiEndpointInfo != nullptr) midiEndpointInfo(
                                (umpMess[0]>>8) & 0xFF, //Maj Ver
                                umpMess[0] & 0xFF,  //Min Ver
                                (umpMess[1]>>24) & 0xFF, //Num Of Func Block
                                ((umpMess[1]>>9) & 0x1), //M2 Support
                                ((umpMess[1]>>8) & 0x1), //M1 Support
                                ((umpMess[1]>>1) & 0x1), //rxjr Support
                                (umpMess[1] & 0x1) //txjr Support
                                );
                    break;
                }

                case MIDIENDPOINT_DEVICEINFO_NOTIFICATION:
                    if(midiEndpointDeviceInfo != nullptr) {
                        midiEndpointDeviceInfo(
                                {(uint8_t)((umpMess[1] >> 16) & 0x7F),(uint8_t)((umpMess[1] >> 8) & 0x7F), (uint8_t)(umpMess[1] & 0x7F)},
                                {(uint8_t)((umpMess[2] >> 24) & 0x7F) , (uint8_t)((umpMess[2] >> 16) & 0x7F)},
                                {(uint8_t)((umpMess[2] >> 8) & 0x7F ), (uint8_t)(umpMess[2]  & 0x7F)},
                                {(uint8_t)((umpMess[3] >> 24) & 0x7F), (uint8_t)((umpMess[3] >> 16) & 0x7F),
                                 (uint8_t)( (umpMess[3] >> 8) & 0x7F), (uint8_t)(umpMess[3] & 0x7F)}
                        );
                    }
                    break;
                case MIDIENDPOINT_NAME_NOTIFICATION:
                case MIDIENDPOINT_PRODID_NOTIFICATION: {
                    uint8_t form = umpMess[0] >> 24 & 0x3;
                    uint8_t text[14];
                    uint8_t textLength =0 ;

                    if ((umpMess[0] >> 8) & 0xFF) text[textLength++] = (umpMess[0] >> 8) & 0xFF;
                    if (umpMess[0] & 0xFF) text[textLength++] = umpMess[0]  & 0xFF;
                    for(uint8_t i = 1; i<=3; i++){
                        for(uint8_t j = 24; j>=0; j-=8){
                            uint8_t c = (umpMess[i] >> j) & 0xFF;
                            if(c){
                                text[textLength++]=c;
                    }
                        }
                    }
                    if(status == MIDIENDPOINT_NAME_NOTIFICATION && midiEndpointName != nullptr) midiEndpointName(form,textLength,text);
                    if(status == MIDIENDPOINT_PRODID_NOTIFICATION && midiEndpointProdId != nullptr) midiEndpointProdId(form,textLength,text);
                    break;
                }

                case MIDIENDPOINT_PROTOCOL_REQUEST: //JR Protocol Req
                    if(midiEndpointJRProtocolReq != nullptr)
                        midiEndpointJRProtocolReq((umpMess[0] >> 8),
                                                   (umpMess[0] >> 1) & 1,
                                                   umpMess[0] & 1
                                                   );
                    break;
                case MIDIENDPOINT_PROTOCOL_NOTIFICATION: //JR Protocol Req
                    if(midiEndpointJRProtocolNotify != nullptr)
                        midiEndpointJRProtocolNotify((umpMess[0] >> 8),
                                                     (umpMess[0] >> 1) & 1,
                                                     umpMess[0] & 1
                                                    );
                    break;

                case FUNCTIONBLOCK:{
                    uint8_t filter = umpMess[0] & 0xFF;
                    uint8_t fbIdx = (umpMess[0] >> 8) & 0xFF;
                    if(functionBlock != nullptr) functionBlock(fbIdx, filter);
                    break;
                }

                case FUNCTIONBLOCK_INFO_NOTFICATION:
                    if(functionBlockInfo != nullptr) {
                        uint8_t fbIdx = (umpMess[0] >> 8) & 0x7F;
                        functionBlockInfo(
                                fbIdx, //fbIdx
                                (umpMess[0] >> 15) & 0x1, // active
                                umpMess[0] & 0x3, //dir
                                ((umpMess[1] >> 24) & 0x1F), //first group
                                ((umpMess[1] >> 16) & 0x1F), // group length
                                (umpMess[1] >> 15) & 0x1, // midiCIValid
                                ((umpMess[1] >> 8) & 0x7F), //midiCIVersion
                                ((umpMess[0]>>2)  & 0x3), //isMIDI 1
                                (umpMess[1]  & 0xFF) // max Streams
                        );
                    }
                    break;
                case FUNCTIONBLOCK_NAME_NOTIFICATION:{
                    uint8_t fbIdx = (umpMess[0] >> 8) & 0x7F;
                    uint8_t form = umpMess[0] >> 24 & 0x3;
                    uint8_t text[13];
                    uint8_t textLength =0 ;

                    if (umpMess[0] & 0xFF) text[textLength++] = umpMess[0]  & 0xFF;
                    for(uint8_t i = 1; i<=3; i++){
                        for(uint8_t j = 24; j>=0; j-=8){
                            uint8_t c = (umpMess[i] >> j) & 0xFF;
                            if(c){
                                text[textLength++]=c;
                    }
                        }
                    }

                    if(functionBlockName != nullptr) functionBlockName(fbIdx, form,textLength,text);
                    break;
                 }
                case STARTOFSEQ: {
                    if(startOfSeq != nullptr) startOfSeq();
                    break;
                }
                case ENDOFFILE: {
                    if(endOfFile != nullptr) endOfFile();
                    break;
                }

            }

        }else
        if(mt == UMP_DATA){ //128 bits Data Messages (including System Exclusive 8)
            uint8_t status = (umpMess[0] >> 20) & 0xF;
            //SysEx 8
            if(status <= 3){
                //SysEx 8
                /*uint8_t numbytes  = (umpMess[0] >> 16) & 0xF;
                uint8_t streamId  = (umpMess[0] >> 8) & 0xFF;
                if(status == 0 || status == 1){
                    startSysex7(group); //streamId
                }

                if(numbytes > 0)processSysEx(group, umpMess[0] & 0xFF);
                if(numbytes > 1)processSysEx(group, (umpMess[1] >> 24) & 0xFF);
                if(numbytes > 2)processSysEx(group, (umpMess[1] >> 16) & 0xFF);
                if(numbytes > 3)processSysEx(group, (umpMess[1] >> 8) & 0xFF);
                if(numbytes > 4)processSysEx(group, umpMess[1] & 0xFF);

                if(numbytes > 5)processSysEx(group, (umpMess[2] >> 24) & 0xFF);
                if(numbytes > 6)processSysEx(group, (umpMess[2] >> 16) & 0xFF);
                if(numbytes > 7)processSysEx(group, (umpMess[2] >> 8) & 0xFF);
                if(numbytes > 8)processSysEx(group, umpMess[2] & 0xFF);

                if(numbytes > 9)processSysEx(group, (umpMess[3] >> 24) & 0xFF);
                if(numbytes > 10)processSysEx(group, (umpMess[3] >> 16) & 0xFF);
                if(numbytes > 11)processSysEx(group, (umpMess[3] >> 8) & 0xFF);
                if(numbytes > 12)processSysEx(group, umpMess[3] & 0xFF);

                if(status == 0 || status == 3){
                    endSysex7(group);
                }*/

            }else if(status == 8 || status ==9){
                //Beginning of Mixed Data Set
                //uint8_t mdsId  = (umpMess[0] >> 16) & 0xF;

                if(status == 8){
                    /*uint16_t numValidBytes  = umpMess[0] & 0xFFFF;
                    uint16_t numChunk  = (umpMess[1] >> 16) & 0xFFFF;
                    uint16_t numOfChunk  = umpMess[1] & 0xFFFF;
                    uint16_t manuId  = (umpMess[2] >> 16) & 0xFFFF;
                    uint16_t deviceId  = umpMess[2] & 0xFFFF;
                    uint16_t subId1  = (umpMess[3] >> 16) & 0xFFFF;
                    uint16_t subId2  = umpMess[3] & 0xFFFF;*/
                }else{
                    // MDS bytes?
                }

            }

        }
        else
        if(mt == UMP_FLEX_DATA){ //128 bits Data Messages (including System Exclusive 8)
            uint8_t statusBank = (umpMess[0] >> 8) & 0xFF;
            uint8_t status = umpMess[0] & 0xFF;
            uint8_t channel = (umpMess[0] >> 16) & 0xF;
            uint8_t addrs = (umpMess[0] >> 18) & 0b11;
            uint8_t form = (umpMess[0] >> 20) & 0b11;
            //SysEx 8
            switch (statusBank){
                case FLEXDATA_COMMON:{ //Common/Configuration for MIDI File, Project, and Track
                    switch (status){
                        case FLEXDATA_COMMON_TEMPO: { //Set Tempo Message
                            if(flexTempo != nullptr) flexTempo(group, umpMess[1]);
                            break;
                        }
                        case FLEXDATA_COMMON_TIMESIG: { //Set Time Signature Message
                            if(flexTimeSig != nullptr) flexTimeSig(group,
                                                                 (umpMess[1] >> 24) & 0xFF,
                                                                 (umpMess[1] >> 16) & 0xFF,
                                                                 (umpMess[1] >> 8) & 0xFF
                                   );
                            break;
                        }
                        case FLEXDATA_COMMON_METRONOME: { //Set Metronome Message
                            if(flexMetronome != nullptr) flexMetronome(group,
                                                                   (umpMess[1] >> 24) & 0xFF,
                                                                   (umpMess[1] >> 16) & 0xFF,
                                                                   (umpMess[1] >> 8) & 0xFF,
                                                                   umpMess[1] & 0xFF,
                                                                   (umpMess[2] >> 24) & 0xFF,
                                                                   (umpMess[2] >> 16) & 0xFF
                                );
                            break;
                        }
                        case FLEXDATA_COMMON_KEYSIG: { //Set Key Signature Message
                            if(flexKeySig != nullptr) flexKeySig(group, addrs, channel,
                                                                   (umpMess[1] >> 24) & 0xFF,
                                                                   (umpMess[1] >> 16) & 0xFF
                                );
                            break;
                        }
                        case FLEXDATA_COMMON_CHORD: { //Set Chord Message
                            if(flexChord != nullptr) flexChord(group, addrs, channel,
                                                                       (umpMess[1] >> 28) & 0xF, //chShrpFlt
                                                                       (umpMess[1] >> 24) & 0xF, //chTonic
                                                                       (umpMess[1] >> 16) & 0xFF, //chType
                                                                       (umpMess[1] >> 12) & 0xF, //chAlt1Type
                                                                       (umpMess[1] >> 8) & 0xF,//chAlt1Deg
                                                                       (umpMess[1] >> 4) & 0xF,//chAlt2Type
                                                                       umpMess[1] & 0xF,//chAlt2Deg
                                                                       (umpMess[2] >> 28) & 0xF,//chAlt3Type
                                                                       (umpMess[2] >> 24) & 0xF,//chAlt3Deg
                                                                       (umpMess[2] >> 20) & 0xF,//chAlt4Type
                                                                       (umpMess[2] >> 16) & 0xF,//chAlt4Deg
                                                                       (umpMess[3] >> 28) & 0xF,//baShrpFlt
                                                                    (umpMess[3] >> 24) & 0xF,//baTonic
                                                                (umpMess[3] >> 16) & 0xFF,//baType
                                                               (umpMess[3] >> 12) & 0xF,//baAlt1Type
                                                               (umpMess[3] >> 8) & 0xF,//baAlt1Deg
                                                               (umpMess[3] >> 4) & 0xF,//baAlt2Type
                                                               umpMess[1] & 0xF//baAlt2Deg
                                );
                            break;
                        }
                    }
                    break;
                }
                case FLEXDATA_PERFORMANCE: //Performance Events
                case FLEXDATA_LYRIC:{ //Lyric Events
                    uint8_t textLength = 0;
                    uint8_t text[12];
                    for(uint8_t i = 1; i<=3; i++){
                        for(uint8_t j = 24; j>=0; j-=8){
                            uint8_t c = (umpMess[i] >> j) & 0xFF;
                            if(c){
                                text[textLength++]=c;
                            }
                        }
                    }
                    if(statusBank== FLEXDATA_LYRIC && flexLyric != nullptr) flexLyric(group, form, addrs, channel, status, text, textLength);
                    if(statusBank== FLEXDATA_PERFORMANCE && flexPerformance != nullptr) flexPerformance(group, form, addrs, channel, status, text, textLength);
                    break;

                }
            }

        }
		messPos =0;
	} else {
		messPos++;
	}
	
}






