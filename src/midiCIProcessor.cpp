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

#include "midiCIProcessor.h"

void midiCIProcessor::endSysex7(){
    if(midici._reqTupleSet){
        cleanupRequest(midici._peReqIdx);
    }
}

void midiCIProcessor::startSysex7(uint8_t group, uint8_t deviceId){

    sysexPos = 0;
    buffer[0]='\0';
    intTemp[0]=0;
    intTemp[1]=0;
    intTemp[2]=0;
    intTemp[3]=0;
    midici =  MIDICI();
    midici.deviceId = deviceId;
    midici.umpGroup = group;
    midici.refpoint = refpoint;
}

void midiCIProcessor::cleanupRequest(reqId peReqIdx){
    peHeaderStr.erase(peReqIdx);
}

void midiCIProcessor::processMIDICI(uint8_t s7Byte){
    sysexPos++;
	if(sysexPos == 4){
		midici.ciType =  s7Byte;
	}
	
	if(sysexPos == 5){
	    midici.ciVer =  s7Byte;
	}
	if(sysexPos >= 6 && sysexPos <= 9){
        midici.remoteMUID += (uint32_t)s7Byte << (7 * (sysexPos - 6));
	}
	
	if(sysexPos >= 10 && sysexPos <= 13){
        midici.localMUID += (uint32_t)s7Byte << (7 * (sysexPos - 10));
	}
	
	if(sysexPos >= 13
       && midici.localMUID != M2_CI_BROADCAST
       && checkMUID && !checkMUID(midici.umpGroup, midici.localMUID, refpoint)
        ){
		return; //Not for this device
	}
	
	//break up each Process based on ciType
    if(sysexPos >= 13) {
        switch (midici.ciType) {
            case MIDICI_DISCOVERYREPLY: //Discovery Reply
            case MIDICI_DISCOVERY: { //Discovery Request
                if (sysexPos >= 14 && sysexPos <= 24) {
                    buffer[sysexPos - 14] = s7Byte;
                }
                if (sysexPos == 25) {
                    intTemp[0] = s7Byte; // ciSupport
                }
                if (sysexPos >= 26 && sysexPos <= 29) {
                    intTemp[1] += (uint16_t)s7Byte << (7 * (sysexPos - 26)); //maxSysEx
                }

                bool complete = false;
                if (sysexPos == 29 && midici.ciVer == 1) {
                    complete = true;
                }
                else if (sysexPos == 30){
                    intTemp[2] = s7Byte; //output path id
                    if(midici.ciType==MIDICI_DISCOVERY) {
                        complete = true;
                    }
                }
                else if (sysexPos == 31){
                    intTemp[3] = s7Byte; //fbIdx id
                    if(midici.ciType==MIDICI_DISCOVERYREPLY) {
                        complete = true;
                    }
                }

                if (complete) {
                    //debug("  - Discovery Request 28 ");

                    if(midici.ciType==MIDICI_DISCOVERY) {
                        if (recvDiscoveryRequest != nullptr) recvDiscoveryRequest(

                                midici,
                                {buffer[0],buffer[1],buffer[2]},
                                {buffer[3], buffer[4]},
                                {buffer[5], buffer[6]},
                                {buffer[7], buffer[8],
                                 buffer[9], buffer[10]},
                                (uint8_t) intTemp[0],
                                intTemp[1],
                                (uint8_t) intTemp[2]
                                //intTemp[3],
                               // &(buffer[11])
                        );
                    }else{
                        if (recvDiscoveryReply != nullptr) recvDiscoveryReply(

                                midici,
                                {buffer[0],buffer[1],buffer[2]},
                                {buffer[3], buffer[4]},
                                {buffer[5], buffer[6]},
                                {buffer[7], buffer[8],
                                 buffer[9], buffer[10]},
                                (uint8_t) intTemp[0],
                                intTemp[1],
                                (uint8_t) intTemp[2],
                                (uint8_t) intTemp[3]
                                //&(buffer[11])
                        );
                    }
                }
                break;
            }

            case MIDICI_INVALIDATEMUID: //MIDI-CI Invalidate MUID Message

                if (sysexPos >= 14 && sysexPos <= 17) {
                    buffer[sysexPos - 14] = s7Byte;
                }

                //terminate MUID
                if (sysexPos == 17 && recvInvalidateMUID != nullptr) {

                    uint32_t terminateMUID = (uint32_t)buffer[0]
                            + ((uint32_t)buffer[1] << 7)
                            + ((uint32_t)buffer[2] << 14)
                            + ((uint32_t)buffer[3] << 21);
                    recvInvalidateMUID(midici, terminateMUID);
                }
                break;
            case MIDICI_ENDPOINTINFO:{
                if (sysexPos == 14 && midici.ciVer > 1 && recvEndPointInfo!= nullptr) {
                    recvEndPointInfo(midici,s7Byte); // uint8_t origSubID,
                }
                break;
            }
            case MIDICI_ENDPOINTINFO_REPLY:{
                bool complete = false;
                if(midici.ciVer < 2) return;
                if (sysexPos == 14 && recvEndPointInfo!= nullptr) {
                    intTemp[0] = s7Byte;
                }
                if(sysexPos == 15 || sysexPos == 16){
                    intTemp[1] += (uint16_t)s7Byte << (7 * (sysexPos - 15 ));
                    return;
                }
                if (sysexPos >= 17 && sysexPos <= 16 + intTemp[1]){
                    buffer[sysexPos - 17] = s7Byte; //Info Data
                }if (sysexPos == 17 + intTemp[1]){
                    complete = true;
                }

                if (complete) {
                    recvEndPointInfoReply(midici,
                                     (uint8_t) intTemp[0],
                                     intTemp[1],
                                     buffer
                                     );
                }
                break;
            }
            case MIDICI_ACK:
            case MIDICI_NAK: {
                bool complete = false;

                if (sysexPos == 14 && midici.ciVer == 1) {
                    complete = true;
                } else if (sysexPos == 14 && midici.ciVer > 1) {
                    intTemp[0] = s7Byte; // uint8_t origSubID,
                }

                if (sysexPos == 15) {
                    intTemp[1] = s7Byte; //statusCode
                }

                if (sysexPos == 16) {
                    intTemp[2] = s7Byte; //statusData
                }

                if (sysexPos >= 17 && sysexPos <= 21){
                    buffer[sysexPos - 17] = s7Byte; //ackNakDetails
                }

                if(sysexPos == 22 || sysexPos == 23){
                    intTemp[3] += (uint16_t)s7Byte << (7 * (sysexPos - 22 ));
                }

            if (sysexPos >= 24 && sysexPos < 24 + intTemp[3]){
                    buffer[sysexPos - 24] = s7Byte; //product ID
                }
                if (sysexPos == 23 + intTemp[3]){
                    complete = true;
                }
                if (sysexPos == 23 && intTemp[3] == 0 && midici.ciVer > 1){
                    complete = true;
                }

                if (complete) {
                    uint8_t ackNakDetails[5] = {buffer[0], buffer[1],
                                                buffer[2],
                                                buffer[3],
                                                buffer[4]};

                    if (midici.ciType == MIDICI_NAK && recvNAK != nullptr)
                        recvNAK(

                            midici,
                            (uint8_t) intTemp[0],
                            (uint8_t) intTemp[1],
                            (uint8_t) intTemp[2],
                            ackNakDetails,
                            intTemp[3],
                            buffer
                    );
                    if (midici.ciType == MIDICI_ACK && midici.ciVer > 1 && recvACK != nullptr)
                        recvACK(

                            midici,
                            (uint8_t) intTemp[0],
                            (uint8_t) intTemp[1],
                            (uint8_t) intTemp[2],
                            ackNakDetails,
                            intTemp[3],
                            buffer
                        );
                }
                break;
            }

#ifdef M2_ENABLE_PROTOCOL
            case MIDICI_PROTOCOL_NEGOTIATION:
            case MIDICI_PROTOCOL_NEGOTIATION_REPLY:
            case MIDICI_PROTOCOL_SET:
            case MIDICI_PROTOCOL_TEST:
            case MIDICI_PROTOCOL_TEST_RESPONDER:
            case MIDICI_PROTOCOL_CONFIRM:
                processProtocolSysex(s7Byte);
                break;
#endif

#ifndef M2_DISABLE_PROFILE
            case MIDICI_PROFILE_INQUIRY: //Profile Inquiry
            case MIDICI_PROFILE_INQUIRYREPLY: //Reply to Profile Inquiry
            case MIDICI_PROFILE_SETON: //Set Profile On Message
            case MIDICI_PROFILE_SETOFF: //Set Profile Off Message
            case MIDICI_PROFILE_ENABLED: //Set Profile Enabled Message
            case MIDICI_PROFILE_DISABLED: //Set Profile Disabled Message
            case MIDICI_PROFILE_SPECIFIC_DATA: //ProfileSpecific Data
            case MIDICI_PROFILE_DETAILS_INQUIRY:
            case MIDICI_PROFILE_DETAILS_REPLY:
                processProfileSysex(s7Byte);
                break;
#endif


#ifndef M2_DISABLE_PE
            case MIDICI_PE_CAPABILITY: //Inquiry: Property Exchange Capabilities
            case MIDICI_PE_CAPABILITYREPLY: //Reply to Property Exchange Capabilities
            case MIDICI_PE_GET:  // Inquiry: Get Property Data
            case MIDICI_PE_GETREPLY: // Reply To Get Property Data - Needs Work!
            case MIDICI_PE_SET: // Inquiry: Set Property Data
            case MIDICI_PE_SETREPLY: // Reply To Inquiry: Set Property Data
            case MIDICI_PE_SUB: // Inquiry: Subscribe Property Data
            case MIDICI_PE_SUBREPLY: // Reply To Subscribe Property Data
            case MIDICI_PE_NOTIFY: // Notify
                processPESysex(s7Byte);
                break;
#endif

#ifndef M2_DISABLE_PROCESSINQUIRY
            case MIDICI_PI_CAPABILITY:
            case MIDICI_PI_CAPABILITYREPLY:
            case MIDICI_PI_MM_REPORT:
            case MIDICI_PI_MM_REPORT_REPLY:
            case MIDICI_PI_MM_REPORT_END:
                processPISysex(s7Byte);
                break;
#endif
            default:
                if (recvUnknownMIDICI) {
                    recvUnknownMIDICI(midici, s7Byte);
                }
                break;

        }
    }
}

void midiCIProcessor::processProtocolSysex(uint8_t s7Byte){
    switch (midici.ciType){

        case MIDICI_PROTOCOL_NEGOTIATION:
        case MIDICI_PROTOCOL_NEGOTIATION_REPLY: {
            //Authority Level
            if (sysexPos == 14 ) {
                intTemp[0] = s7Byte;
            }
            //Number of Supported Protocols (np)
            if (sysexPos == 15 ) {
                intTemp[1] = s7Byte;
            }

            int protocolOffset = intTemp[1] * 5 + 15;

            if (sysexPos > 15 && sysexPos <= protocolOffset) {
                uint8_t pos = (sysexPos - 16) % 5;
                buffer[pos] = s7Byte;
                if ((sysexPos - 15) % 5 == 0 && recvProtocolAvailable != nullptr) {
                    uint8_t protocol[5] = {buffer[0], buffer[1],
                                           buffer[2], buffer[3],
                                           buffer[4]};
                    recvProtocolAvailable(midici, (uint8_t) intTemp[0], protocol);
                }
            }
            if(midici.ciVer > 1){
                if (sysexPos > protocolOffset && sysexPos <= protocolOffset+5){
                    buffer[sysexPos-protocolOffset-1] = s7Byte;
                }
                if (sysexPos == protocolOffset+5){
                    if (recvSetProtocolConfirm != nullptr)recvSetProtocolConfirm(midici, (uint8_t) intTemp[0]);
                }
            }
            break;
        }

        case MIDICI_PROTOCOL_SET: //Set Profile On Message
            //Authority Level
            if (sysexPos == 14 ) {
                intTemp[0] = s7Byte;
            }
            if(sysexPos >= 15 && sysexPos <= 19){
                buffer[sysexPos-15] = s7Byte;
            }
            if (sysexPos == 19 && recvSetProtocol != nullptr){
                uint8_t protocol[5] = {buffer[0], buffer[1], buffer[2], buffer[3], buffer[4]};
                recvSetProtocol(midici, (uint8_t) intTemp[0], protocol);
            }
            break;

        case MIDICI_PROTOCOL_TEST_RESPONDER:
        case MIDICI_PROTOCOL_TEST:
            //Authority Level
            if (sysexPos == 14 ) {
                intTemp[0] = s7Byte;
                intTemp[1] = 1;
            }
            if(sysexPos >= 15 && sysexPos <= 62){
                if(s7Byte != sysexPos - 15){
                    intTemp[1] = 0;
                }
            }
            if (sysexPos == 62 && recvProtocolTest != nullptr){
                recvProtocolTest(midici, (uint8_t) intTemp[0], !!(intTemp[1]));
            }


            break;

        case MIDICI_PROTOCOL_CONFIRM: //Set Profile Off Message
            //Authority Level
            if (sysexPos == 14 ) {
                intTemp[0] = s7Byte;
                if (recvSetProtocolConfirm != nullptr){
                    recvSetProtocolConfirm(midici, (uint8_t) intTemp[0]);
                }
            }
            break;
    }
}

void midiCIProcessor::processProfileSysex(uint8_t s7Byte){
    switch (midici.ciType){
        case MIDICI_PROFILE_INQUIRY: //Profile Inquiry
            if (sysexPos == 13 && recvProfileInquiry != nullptr){
                recvProfileInquiry(midici);
            }
            break;
        case MIDICI_PROFILE_INQUIRYREPLY: { //Reply to Profile Inquiry
            //Enabled Profiles Length
            if (sysexPos == 14 || sysexPos == 15) {
                intTemp[0] += (uint16_t)s7Byte << (7 * (sysexPos - 14));
            }

            //Disabled Profile Length
            int enabledProfileOffset = intTemp[0] * 5 + 14;
            if (
                    sysexPos == enabledProfileOffset
                    || sysexPos == 1 + enabledProfileOffset
            ) {
                intTemp[1] += (uint16_t)s7Byte << (7 * (sysexPos - enabledProfileOffset));
            }

            if (sysexPos >= 16 && sysexPos < enabledProfileOffset) {
                uint8_t pos = (sysexPos - 14) % 5;
                buffer[pos] = s7Byte;
                if (pos == 4 && recvSetProfileEnabled != nullptr) {

                    recvSetProfileEnabled(midici, {buffer[0], buffer[1],
                                                   buffer[2], buffer[3],
                                                   buffer[4]},0);
                }
            }

            if (sysexPos >= 2 + enabledProfileOffset &&
                sysexPos < enabledProfileOffset + intTemp[1] * 5) {
                uint8_t pos = (sysexPos - 14) % 5;
                buffer[pos] = s7Byte;
                if (pos == 4 && recvSetProfileDisabled != nullptr) {
                    recvSetProfileDisabled(midici, {buffer[0], buffer[1],
                                                    buffer[2], buffer[3],
                                                    buffer[4]}
                            ,0);
                }
            }
            break;
        }

        case MIDICI_PROFILE_ADD:
        case MIDICI_PROFILE_REMOVE:
        case MIDICI_PROFILE_ENABLED:
        case MIDICI_PROFILE_DISABLED:
        case MIDICI_PROFILE_SETOFF:
        case MIDICI_PROFILE_SETON: { //Set Profile On Message
            bool complete = false;
            if (sysexPos >= 14 && sysexPos <= 18) {
                buffer[sysexPos - 14] = s7Byte;
            }
            if (sysexPos == 18 &&
                (midici.ciVer == 1 || midici.ciType==MIDICI_PROFILE_ADD || midici.ciType==MIDICI_PROFILE_REMOVE)
                    ){
                complete = true;
            }
            if(midici.ciVer > 1 && (sysexPos == 19 || sysexPos == 20)){
                intTemp[0] += (uint16_t)s7Byte << (7 * (sysexPos - 19 ));
            }
            if (sysexPos == 20 && midici.ciVer > 1){
                complete = true;
            }

            if(complete){
                if (midici.ciType == MIDICI_PROFILE_ADD && recvSetProfileDisabled != nullptr)
                    recvSetProfileDisabled(midici, {buffer[0], buffer[1],
                                                    buffer[2], buffer[3],
                                                    buffer[4]}, 0);

                if (midici.ciType == MIDICI_PROFILE_REMOVE && recvSetProfileRemoved != nullptr)
                    recvSetProfileRemoved(midici, {buffer[0], buffer[1],
                                                   buffer[2], buffer[3],
                                                   buffer[4]});

                if (midici.ciType == MIDICI_PROFILE_SETON && recvSetProfileOn != nullptr)
                    recvSetProfileOn(midici, {buffer[0], buffer[1],
                                              buffer[2], buffer[3],
                                              buffer[4]}, (uint8_t)intTemp[0]);

                if (midici.ciType == MIDICI_PROFILE_SETOFF && recvSetProfileOff != nullptr)
                    recvSetProfileOff(midici, {buffer[0], buffer[1],
                                               buffer[2], buffer[3],
                                               buffer[4]});

                if (midici.ciType == MIDICI_PROFILE_ENABLED && recvSetProfileEnabled != nullptr)
                    recvSetProfileEnabled(midici, {buffer[0], buffer[1],
                                                   buffer[2], buffer[3],
                                                   buffer[4]}, (uint8_t)intTemp[0]);

                if (midici.ciType == MIDICI_PROFILE_DISABLED && recvSetProfileDisabled != nullptr)
                    recvSetProfileDisabled(midici, {buffer[0], buffer[1],
                                                    buffer[2], buffer[3],
                                                    buffer[4]}, (uint8_t)intTemp[0]);

            }
            break;
        }

        case MIDICI_PROFILE_DETAILS_INQUIRY:{
            if (sysexPos >= 14 && sysexPos <= 18) {
                buffer[sysexPos - 14] = s7Byte;
            }
            if (sysexPos == 19 && recvSetProfileDetailsInquiry != nullptr){ //Inquiry Target
                recvSetProfileDetailsInquiry(midici, {buffer[0], buffer[1],
                                                      buffer[2], buffer[3],
                                                      buffer[4]}, s7Byte);
            }

            break;
        }

        case MIDICI_PROFILE_DETAILS_REPLY:{
            if (sysexPos >= 14 && sysexPos <= 18) {
                buffer[sysexPos - 14] = s7Byte;
            }
            if (sysexPos == 19){//Inquiry Target
                buffer[5] = s7Byte;
            }

            if(sysexPos == 20 || sysexPos == 21){ //Inquiry Target Data length (dl)
                intTemp[0] += (uint16_t)s7Byte << (7 * (sysexPos - 20 ));
            }

            if (sysexPos >= 22 && sysexPos <= 22 + intTemp[0]){
                buffer[sysexPos - 23 + 6] = s7Byte; //product ID
            }

            if (sysexPos == 22 + intTemp[0] && recvSetProfileDetailsInquiry != nullptr){
                recvSetProfileDetailsReply(midici, {buffer[0], buffer[1],
                                                    buffer[2], buffer[3],
                                                    buffer[4]},
                                           buffer[5],
                                           intTemp[0],
                                           &(buffer[6])
                );
            }

            break;
        }

        case MIDICI_PROFILE_SPECIFIC_DATA:
            //Profile
            if(sysexPos >= 14 && sysexPos <= 18){
                buffer[sysexPos-14] = s7Byte;
                return;
            }
            if(sysexPos >= 19 && sysexPos <= 22){ //Length of Following Profile Specific Data
                intTemp[0] += (uint32_t)s7Byte << (7 * (sysexPos - 19 ));
                intTemp[1] = 1;
                return;
            }


            //******************

            uint16_t charOffset = (sysexPos - 23) % S7_BUFFERLEN;
            uint16_t dataLength = intTemp[0];
            if(
                    (sysexPos >= 23 && sysexPos <= 22 + dataLength)
                    || 	(dataLength == 0 && sysexPos == 22)
                    ){
                if(dataLength != 0 )buffer[charOffset] = s7Byte;

                bool lastByteOfSet = (sysexPos == 22 + dataLength);

                if(charOffset == S7_BUFFERLEN -1
                   || sysexPos == 22 + dataLength
                   || dataLength == 0
                        ){
                    recvProfileSpecificData(midici, {buffer[0], buffer[1],
                                                buffer[2], buffer[3],
                                                buffer[4]}, charOffset+1, buffer, intTemp[1], lastByteOfSet);
                    intTemp[1]++;
                }
            }


            //***********

            break;
    }
}

void midiCIProcessor::processPESysex(uint8_t s7Byte){

    switch (midici.ciType){
        case MIDICI_PE_CAPABILITY:
        case MIDICI_PE_CAPABILITYREPLY:{
            bool complete = false;

            if(sysexPos == 14){
                buffer[0] = s7Byte;
            }

            if(sysexPos == 14 && midici.ciVer == 1){
                complete = true;
            }

            if(sysexPos == 15){
                buffer[1] = s7Byte;
            }
            if(sysexPos == 16){
                buffer[2] = s7Byte;
                complete = true;
            }

            if(complete){
                if(midici.ciType == MIDICI_PE_CAPABILITY && recvPECapabilities != nullptr)
                    recvPECapabilities(midici,
                                       buffer[0],
                                       buffer[1],
                                       buffer[2]
                    );

                if(midici.ciType == MIDICI_PE_CAPABILITYREPLY && recvPECapabilities != nullptr)
                    recvPECapabilitiesReplies(midici,
                                              buffer[0],
                                              buffer[1],
                                              buffer[2]
                    );

            }

            break;
        }
        default: {

            if (sysexPos == 14) {
                midici._peReqIdx = std::make_tuple(midici.remoteMUID,s7Byte);
                midici._reqTupleSet = true; //Used for cleanup
                //peRequestDetails[midici._peReqIdx] = peHeader();
                midici.requestId = s7Byte;
                intTemp[0]=0;
                return;
            }


            if (sysexPos == 15 || sysexPos == 16) { //header Length
                intTemp[0] += s7Byte << (7 * (sysexPos - 15));
                return;
            }

            uint16_t headerLength = intTemp[0];

            if (sysexPos == 17 && midici.numChunk == 1){
                peHeaderStr[midici._peReqIdx] = "";
            }

            if (sysexPos >= 17 && sysexPos <= 16 + headerLength) {
                uint16_t charOffset = (sysexPos - 17);
                buffer[charOffset] = s7Byte;
                peHeaderStr[midici._peReqIdx].push_back(s7Byte);


                if (sysexPos == 16 + headerLength) {

                    switch (midici.ciType) {
                        case MIDICI_PE_GET:
                            if (recvPEGetInquiry != nullptr) {
                                recvPEGetInquiry(midici, peHeaderStr[midici._peReqIdx]);
                                cleanupRequest(midici._peReqIdx);
                            }
                            break;
                        case MIDICI_PE_SETREPLY:
                            if (recvPESetReply != nullptr) {
                                recvPESetReply(midici, peHeaderStr[midici._peReqIdx]);
                                cleanupRequest(midici._peReqIdx);
                            }
                            break;
                        case MIDICI_PE_SUBREPLY:
                            if (recvPESubReply != nullptr) {
                                recvPESubReply(midici, peHeaderStr[midici._peReqIdx]);
                                cleanupRequest(midici._peReqIdx);
                            }
                            break;
                        case MIDICI_PE_NOTIFY:
                            if (recvPENotify != nullptr) {
                                recvPENotify(midici, peHeaderStr[midici._peReqIdx]);
                                cleanupRequest(midici._peReqIdx);
                            }
                            break;
                    }
                }
            }

            if (sysexPos == 17 + headerLength || sysexPos == 18 + headerLength) {
                midici.totalChunks +=
                        s7Byte << (7 * (sysexPos - 17 - headerLength));
                return;
            }

            if (sysexPos == 19 + headerLength || sysexPos == 20 + headerLength) {
                midici.numChunk += s7Byte << (7 * (sysexPos - 19 - headerLength));
                return;
            }

            if (sysexPos == 21 + headerLength) { //Body Length
                intTemp[1] = s7Byte;
                return;
            }
            if (sysexPos == 22 + headerLength) { //Body Length
                intTemp[1] += s7Byte << 7;
            }

            uint16_t bodyLength = intTemp[1];
            uint16_t initPos = 23 + headerLength;
            uint16_t charOffset = (sysexPos - initPos) % S7_BUFFERLEN;

            if (
                    (sysexPos >= initPos && sysexPos <= initPos - 1 + bodyLength)
                    || (bodyLength == 0 && sysexPos == initPos - 1)
                    ) {
                if (bodyLength != 0)buffer[charOffset] = s7Byte;

                bool lastByteOfSet = (
                        midici.numChunk == midici.totalChunks &&
                        sysexPos == initPos - 1 + bodyLength);
                bool lastByteOfChunk = (bodyLength == 0 || sysexPos == initPos - 1 + bodyLength);


                if (charOffset == S7_BUFFERLEN - 1 || lastByteOfChunk) {
                    if (midici.ciType == MIDICI_PE_GETREPLY && recvPEGetReply != nullptr) {
                        recvPEGetReply(midici, peHeaderStr[midici._peReqIdx],
                                         charOffset + 1, buffer, lastByteOfChunk, lastByteOfSet);
                    }

                    if (midici.ciType == MIDICI_PE_SUB && recvPESubInquiry != nullptr) {
                        recvPESubInquiry(midici, peHeaderStr[midici._peReqIdx],
                                         charOffset + 1, buffer, lastByteOfChunk, lastByteOfSet);
                    }

                    if (midici.ciType == MIDICI_PE_SET && recvPESetInquiry != nullptr) {
                        recvPESetInquiry(midici, peHeaderStr[midici._peReqIdx],
                                         charOffset + 1, buffer, lastByteOfChunk, lastByteOfSet);
                    }
                    midici.partialChunkCount++;
                }

                if (lastByteOfSet) {
                    cleanupRequest(midici._peReqIdx);
                }
            }
            break;
        }
    }

}

void midiCIProcessor::processPISysex(uint8_t s7Byte) {
    if(midici.ciVer == 1) return;

    switch (midici.ciType) {
        case MIDICI_PI_CAPABILITY: {
            if (sysexPos == 13 && recvPICapabilities != nullptr) {
                recvPICapabilities(midici);
            }
            break;
        }
        case MIDICI_PI_CAPABILITYREPLY: {
            if (sysexPos == 14 && recvPICapabilitiesReply != nullptr) {
                recvPICapabilitiesReply(midici,s7Byte);
            }
            break;
        }
        case MIDICI_PI_MM_REPORT_END: {
            if (sysexPos == 13 && recvPIMMReportEnd != nullptr) {
                recvPIMMReportEnd(midici);
            }
            break;
        }
        case MIDICI_PI_MM_REPORT:{
            if (sysexPos == 14) {//MDC
                buffer[0] = s7Byte;
            }
            if (sysexPos == 15) {//Bitmap of requested System Message Types
                buffer[1] = s7Byte;
            }
            if (sysexPos == 17) {//Bitmap of requested Channel Controller Message Types
                buffer[2] = s7Byte;
            }
            if (sysexPos == 18 && recvPIMMReport != nullptr){
                recvPIMMReport(midici,
                               buffer[0],
                               buffer[1],
                               buffer[2],
                               s7Byte);
            }
            break;
        }
        case MIDICI_PI_MM_REPORT_REPLY: {
            if (sysexPos == 14) {//Bitmap of requested System Message Types
                buffer[0] = s7Byte;
            }
            if (sysexPos == 16) {//Bitmap of requested Channel Controller Message Types
                buffer[1] = s7Byte;
            }
            if (sysexPos == 17 && recvPIMMReportReply != nullptr){
                recvPIMMReportReply(midici,
                                    buffer[0],
                                    buffer[1],
                                    s7Byte);
            }
            break;
        }
        default: {
            break;
        }
    }
}
