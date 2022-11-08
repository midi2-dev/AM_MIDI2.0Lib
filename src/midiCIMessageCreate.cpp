//
// Created by andrew on 31/10/22.
//

#include "../include/midiCIMessageCreate.h"

void setBytesFromNumbers(uint8_t* message, uint32_t number, uint16_t * start, uint8_t amount){
    for(int amountC = amount; amountC>0;amountC--){
        message[(*start)++] = number & 127;
        number = number >> 7;
    }
}

void concatSysexArray(uint8_t* sysex,uint16_t *start, uint8_t* add,  uint8_t len ){
    uint8_t i;
    for(i=0; i<len; i++){
        sysex[(*start)++] = add[i];
    }
}


void createCIHeader(uint8_t* sysexHeader, MIDICI midiCiHeader){
    sysexHeader[0]=S7UNIVERSAL_NRT;
    sysexHeader[1]=midiCiHeader.deviceId;//MIDI_PORT;
    sysexHeader[2]=S7MIDICI;
    sysexHeader[3]=midiCiHeader.ciType;
    sysexHeader[4]=midiCiHeader.ciVer;
    uint16_t length = 5;
    setBytesFromNumbers(sysexHeader, midiCiHeader.localMUID, &length, 4);
    setBytesFromNumbers(sysexHeader, midiCiHeader.remoteMUID, &length, 4);
}

uint16_t sendDiscovery(uint8_t* sysex, uint8_t midiCIVer, uint8_t ciType, uint32_t srcMUID, uint32_t destMUID,
                                    std::array<uint8_t, 3> manuId, std::array<uint8_t, 2> familyId,
                                    std::array<uint8_t, 2> modelId, std::array<uint8_t, 4> version,
                                    uint8_t ciSupport, uint16_t sysExMax,
                                    uint8_t outputPathId,
                                    uint8_t fbIdx
){

    MIDICI midiCiHeader;
    midiCiHeader.ciType = ciType;
    midiCiHeader.localMUID = srcMUID;
    midiCiHeader.remoteMUID = destMUID;
    midiCiHeader.ciVer = midiCIVer;
    createCIHeader(sysex, midiCiHeader);
    uint16_t length = 13;
    concatSysexArray(sysex,&length,manuId.data(),3);
    concatSysexArray(sysex,&length,familyId.data(),2);
    concatSysexArray(sysex,&length,modelId.data(),2);
    concatSysexArray(sysex,&length,version.data(),4);

    //Capabilities
    sysex[length++]=ciSupport;
    setBytesFromNumbers(sysex, sysExMax, &length, 4);
    if(midiCIVer<2){
        return length;
    }
    sysex[length++]=outputPathId;

    if(ciType==MIDICI_DISCOVERY){
        return length;
    }else{
        sysex[length++]=fbIdx;
        return length;
    }
}

uint16_t sendDiscoveryRequest(uint8_t* sysex, uint8_t midiCIVer,  uint32_t srcMUID,
                                           std::array<uint8_t, 3> manuId, std::array<uint8_t, 2> familyId,
                                           std::array<uint8_t, 2> modelId, std::array<uint8_t, 4> version,
                                           uint8_t ciSupport, uint16_t sysExMax,
                                           uint8_t outputPathId
) {
    return sendDiscovery(sysex, midiCIVer,MIDICI_DISCOVERY, srcMUID, M2_CI_BROADCAST,
                  manuId, familyId,
                  modelId, version,
                  ciSupport, sysExMax,
                  outputPathId,
                  0
    );
}

uint16_t sendDiscoveryReply(uint8_t* sysex, uint8_t midiCIVer, uint32_t srcMUID, uint32_t destMuid,
                                         std::array<uint8_t, 3> manuId, std::array<uint8_t, 2> familyId,
                                         std::array<uint8_t, 2> modelId, std::array<uint8_t, 4> version,
                                         uint8_t ciSupport, uint16_t sysExMax,
                                         uint8_t outputPathId,
                                         uint8_t fbIdx
){
    return sendDiscovery(sysex, midiCIVer,MIDICI_DISCOVERYREPLY, srcMUID, destMuid,
                  manuId, familyId,
                  modelId, version,
                  ciSupport, sysExMax,
                  outputPathId,
                  fbIdx
    );
}

uint16_t sendEndpointInfoRequest(uint8_t* sysex, uint8_t midiCIVer, uint32_t srcMUID, uint32_t destMuid, uint8_t status) {

    if(midiCIVer<2) return 0;

    MIDICI midiCiHeader;
    midiCiHeader.ciType = MIDICI_ENDPOINTINFO;
    midiCiHeader.ciVer = midiCIVer;
    midiCiHeader.localMUID = srcMUID;
    midiCiHeader.remoteMUID = destMuid;
    createCIHeader(sysex, midiCiHeader);
    sysex[13] = status;
    return 14;
}

uint16_t sendEndpointInfoReply(uint8_t* sysex, uint8_t midiCIVer, uint32_t srcMUID, uint32_t destMuid, uint8_t status,
                                            uint16_t infoLength, uint8_t* infoData) {
    if(midiCIVer<2) return 0;

    MIDICI midiCiHeader;
    midiCiHeader.ciType = MIDICI_ENDPOINTINFO_REPLY;
    midiCiHeader.ciVer = midiCIVer;
    midiCiHeader.localMUID = srcMUID;
    midiCiHeader.remoteMUID = destMuid;
    createCIHeader(sysex, midiCiHeader);
    sysex[13] = status;
    uint16_t length = 14;
    setBytesFromNumbers(sysex, infoLength, &length, 2);
    concatSysexArray(sysex,&length,infoData,infoLength);
    return length;
}

uint16_t sendACKNAK(uint8_t* sysex, uint8_t midiCIVer, uint8_t ciType, uint32_t srcMUID, uint32_t destMuid,
                                 uint8_t originalSubId, uint8_t statusCode,
                                 uint8_t statusData, uint8_t* ackNakDetails, uint16_t messageLength,
                                 uint8_t* ackNakMessage){
    MIDICI midiCiHeader;
    midiCiHeader.ciType = ciType;
    midiCiHeader.ciVer = midiCIVer;
    midiCiHeader.localMUID = srcMUID;
    midiCiHeader.remoteMUID = destMuid;
    createCIHeader(sysex, midiCiHeader);
    uint16_t length = 13;
    if(midiCIVer<2){
        return length;
    }

    sysex[length++]=originalSubId;
    sysex[length++]=statusCode;
    sysex[length++]=statusData;

    concatSysexArray(sysex,&length,ackNakDetails,5);
    setBytesFromNumbers(sysex, messageLength, &length, 2);
    concatSysexArray(sysex,&length,ackNakMessage,messageLength);
    return length;
}

uint16_t sendACK(uint8_t* sysex, uint8_t midiCIVer, uint32_t srcMUID, uint32_t destMuid, uint8_t originalSubId, uint8_t statusCode,
                              uint8_t statusData, uint8_t* ackNakDetails, uint16_t messageLength,
                              uint8_t* ackNakMessage) {

    return sendACKNAK(sysex, midiCIVer,MIDICI_ACK, srcMUID, destMuid, originalSubId, statusCode, statusData, ackNakDetails,
               messageLength, ackNakMessage);

}

uint16_t sendNAK(uint8_t* sysex, uint8_t midiCIVer, uint32_t srcMUID, uint32_t destMuid, uint8_t originalSubId, uint8_t statusCode,
                              uint8_t statusData, uint8_t* ackNakDetails, uint16_t messageLength,
                              uint8_t* ackNakMessage) {

    return sendACKNAK(sysex, midiCIVer,MIDICI_NAK, srcMUID, destMuid, originalSubId, statusCode, statusData, ackNakDetails,
               messageLength, ackNakMessage);

}


uint16_t sendInvalidateMUID(uint8_t* sysex, uint8_t midiCIVer, uint32_t srcMUID, uint32_t terminateMuid){
    MIDICI midiCiHeader;
    midiCiHeader.ciType = MIDICI_INVALIDATEMUID;
    midiCiHeader.ciVer = midiCIVer;
    midiCiHeader.localMUID = srcMUID;
    createCIHeader(sysex, midiCiHeader);
    setBytesFromNumbers(sysex, terminateMuid, 0, 4);
    return 17;
}



//Protocol Negotiation
uint16_t sendProtocolNegotiation(uint8_t* sysex, uint8_t midiCIVer, uint32_t srcMUID, uint32_t destMuid,
                                             uint8_t authorityLevel, uint8_t numProtocols, uint8_t* protocols
                                             , uint8_t* currentProtocol){
    MIDICI midiCiHeader;
    midiCiHeader.ciType = MIDICI_PROTOCOL_NEGOTIATION;
    midiCiHeader.localMUID = srcMUID;
    midiCiHeader.remoteMUID = destMuid;
    midiCiHeader.ciVer = midiCIVer;
    createCIHeader(sysex, midiCiHeader);
    sysex[13] = authorityLevel;
    uint16_t length = 14;
    concatSysexArray(sysex,&length,protocols,numProtocols*5);
    if(midiCIVer<2){
        return length;
    }
    concatSysexArray(sysex,&length,currentProtocol,5);
    return length;

}

uint16_t sendProtocolNegotiationReply(uint8_t* sysex, uint8_t midiCIVer, uint32_t srcMUID, uint32_t destMuid,
                                             uint8_t authorityLevel, uint8_t numProtocols, uint8_t* protocols ){
    
    
    MIDICI midiCiHeader;
    midiCiHeader.ciType = MIDICI_PROTOCOL_NEGOTIATION_REPLY;
    midiCiHeader.localMUID = srcMUID;
    midiCiHeader.remoteMUID = destMuid;
    midiCiHeader.ciVer = midiCIVer;
    createCIHeader(sysex, midiCiHeader);
    sysex[13] = authorityLevel;
    uint16_t length = 14;
    concatSysexArray(sysex,&length,protocols,numProtocols*5);
    return length;
}


uint16_t sendSetProtocol(uint8_t* sysex, uint8_t midiCIVer, uint32_t srcMUID, uint32_t destMuid,
                                 uint8_t authorityLevel, uint8_t* protocol){
    MIDICI midiCiHeader;
    midiCiHeader.ciType = MIDICI_PROTOCOL_SET;
    midiCiHeader.localMUID = srcMUID;
    midiCiHeader.remoteMUID = destMuid;
    midiCiHeader.ciVer = midiCIVer;
    createCIHeader(sysex, midiCiHeader);
    sysex[13] = authorityLevel;
    uint16_t length = 14;
    concatSysexArray(sysex,&length,protocol,5);
    return length;
}
uint16_t sendProtocolTest(uint8_t* sysex, uint8_t midiCIVer, uint32_t srcMUID, uint32_t destMuid,
                                 uint8_t authorityLevel){
    MIDICI midiCiHeader;
    midiCiHeader.ciType = MIDICI_PROTOCOL_TEST;
    midiCiHeader.localMUID = srcMUID;
    midiCiHeader.remoteMUID = destMuid;
    midiCiHeader.ciVer = midiCIVer;
    createCIHeader(sysex, midiCiHeader);
    sysex[13] = authorityLevel;
    uint16_t length = 14;
    uint8_t testData[48]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47};
    concatSysexArray(sysex,&length,testData,48);
    return length;
}

uint16_t sendProtocolTestResponder(uint8_t* sysex, uint8_t midiCIVer, uint32_t srcMUID, uint32_t destMuid,
                                      uint8_t authorityLevel){
    MIDICI midiCiHeader;
    midiCiHeader.ciType = MIDICI_PROTOCOL_TEST_RESPONDER;
    midiCiHeader.localMUID = srcMUID;
    midiCiHeader.remoteMUID = destMuid;
    midiCiHeader.ciVer = midiCIVer;
    createCIHeader(sysex, midiCiHeader);
    sysex[13] = authorityLevel;
    uint16_t length = 14;
    uint8_t testData[48]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47};
    concatSysexArray(sysex,&length,testData,48);
    return length;
}

//Profiles

uint16_t sendProfileListRequest(uint8_t* sysex, uint8_t midiCIVer,  uint32_t srcMUID, uint32_t destMuid, uint8_t destination){
    MIDICI midiCiHeader;
    midiCiHeader.ciType = MIDICI_PROFILE_INQUIRY;
    midiCiHeader.localMUID = srcMUID;
    midiCiHeader.remoteMUID = destMuid;
    midiCiHeader.deviceId = destination;
    midiCiHeader.ciVer = midiCIVer;
    createCIHeader(sysex, midiCiHeader);
    return 13;
}


uint16_t sendProfileListResponse(uint8_t* sysex, uint8_t midiCIVer,  uint32_t srcMUID, uint32_t destMuid, uint8_t destination, uint8_t profilesEnabledLen, uint8_t* profilesEnabled, uint8_t profilesDisabledLen , uint8_t* profilesDisabled ){
    MIDICI midiCiHeader;
    midiCiHeader.ciType = MIDICI_PROFILE_INQUIRYREPLY;
    midiCiHeader.localMUID = srcMUID;
    midiCiHeader.remoteMUID = destMuid;
    midiCiHeader.deviceId = destination;
    midiCiHeader.ciVer = midiCIVer;
    createCIHeader(sysex, midiCiHeader);
    uint16_t length = 13;
    setBytesFromNumbers(sysex, profilesEnabledLen, &length, 2);
    concatSysexArray(sysex,&length,profilesEnabled,profilesEnabledLen*5);
    setBytesFromNumbers(sysex, profilesDisabledLen, &length, 2);
    concatSysexArray(sysex,&length,profilesDisabled,profilesDisabledLen*5);
    return length;
}

uint16_t sendProfileMessage(uint8_t* sysex, uint8_t midiCIVer,  uint32_t srcMUID, uint32_t destMuid, uint8_t destination,
                                         std::array<uint8_t, 5> profile,
                                         uint8_t numberOfChannels, uint8_t ciType){
    MIDICI midiCiHeader;
    midiCiHeader.ciType = ciType;
    midiCiHeader.localMUID = srcMUID;
    midiCiHeader.remoteMUID = destMuid;
    midiCiHeader.deviceId = destination;
    midiCiHeader.ciVer = midiCIVer;
    createCIHeader(sysex, midiCiHeader);
    uint16_t length = 13;
    concatSysexArray(sysex,&length,profile.data(),5);
    if(midiCIVer==1 || ciType==MIDICI_PROFILE_ADD || ciType==MIDICI_PROFILE_REMOVE){
        return length;
    }
    setBytesFromNumbers(sysex, numberOfChannels, &length, 2);
    return length;

}

uint16_t sendProfileAdd(uint8_t* sysex, uint8_t midiCIVer,  uint32_t srcMUID, uint32_t destMuid, uint8_t destination,
                                     std::array<uint8_t, 5> profile){
    return sendProfileMessage(sysex, midiCIVer, srcMUID, destMuid, destination, profile, 0,
                       (uint8_t) MIDICI_PROFILE_ADD);
}

uint16_t sendProfileRemove(uint8_t* sysex, uint8_t midiCIVer,  uint32_t srcMUID, uint32_t destMuid, uint8_t destination,
                                        std::array<uint8_t, 5> profile){
    return sendProfileMessage(sysex, midiCIVer, srcMUID, destMuid, destination, profile, 0,
                       (uint8_t) MIDICI_PROFILE_REMOVE);
}

uint16_t sendProfileOn(uint8_t* sysex, uint8_t midiCIVer,  uint32_t srcMUID, uint32_t destMuid, uint8_t destination,
                                    std::array<uint8_t, 5> profile, uint8_t numberOfChannels){
    return sendProfileMessage(sysex, midiCIVer, srcMUID, destMuid, destination, profile, numberOfChannels,
                       (uint8_t) MIDICI_PROFILE_SETON);
}

uint16_t sendProfileOff(uint8_t* sysex, uint8_t midiCIVer,  uint32_t srcMUID, uint32_t destMuid, uint8_t destination,
                                     std::array<uint8_t, 5> profile){
    return sendProfileMessage(sysex, midiCIVer, srcMUID, destMuid, destination, profile, 0,
                       (uint8_t) MIDICI_PROFILE_SETOFF);
}

uint16_t sendProfileEnabled(uint8_t* sysex, uint8_t midiCIVer,  uint32_t srcMUID, uint32_t destMuid, uint8_t destination,
                                         std::array<uint8_t, 5> profile,
                                         uint8_t numberOfChannels){
    return sendProfileMessage(sysex, midiCIVer, srcMUID, destMuid, destination, profile, numberOfChannels,
                       (uint8_t) MIDICI_PROFILE_ENABLED);
}

uint16_t sendProfileDisabled(uint8_t* sysex, uint8_t midiCIVer,  uint32_t srcMUID, uint32_t destMuid, uint8_t destination,
                                          std::array<uint8_t, 5> profile,
                                          uint8_t numberOfChannels){
    return sendProfileMessage(sysex, midiCIVer, srcMUID, destMuid, destination, profile, numberOfChannels,
                       (uint8_t) MIDICI_PROFILE_DISABLED);
}


uint16_t sendProfileSpecificData(uint8_t* sysex, uint8_t midiCIVer,  uint32_t srcMUID, uint32_t destMuid, uint8_t destination,
                                              std::array<uint8_t, 5> profile, uint16_t datalen, uint8_t*  data){

    MIDICI midiCiHeader;
    midiCiHeader.ciType = MIDICI_PROFILE_SPECIFIC_DATA;
    midiCiHeader.localMUID = srcMUID;
    midiCiHeader.remoteMUID = destMuid;
    midiCiHeader.deviceId = destination;
    midiCiHeader.ciVer = midiCIVer;
    createCIHeader(sysex, midiCiHeader);
    uint16_t length = 13;
    concatSysexArray(sysex,&length,profile.data(),5);
    setBytesFromNumbers(sysex, datalen, &length, 4);
    concatSysexArray(sysex,&length,data,datalen);
    return length;
}

uint16_t sendProfileDetailsInquiry(uint8_t* sysex, uint8_t midiCIVer,  uint32_t srcMUID, uint32_t destMuid, uint8_t destination,
                                                std::array<uint8_t, 5> profile, uint8_t InquiryTarget){
    if(midiCIVer < 2) return 0;
    MIDICI midiCiHeader;
    midiCiHeader.ciType = MIDICI_PROFILE_SPECIFIC_DATA;
    midiCiHeader.localMUID = srcMUID;
    midiCiHeader.remoteMUID = destMuid;
    midiCiHeader.deviceId = destination;
    midiCiHeader.ciVer = midiCIVer;
    createCIHeader(sysex, midiCiHeader);
    uint16_t length = 13;
    concatSysexArray(sysex,&length,profile.data(),5);
    sysex[length++] = InquiryTarget;
    return length;
}

uint16_t sendProfileDetailsReply(uint8_t* sysex, uint8_t midiCIVer,  uint32_t srcMUID, uint32_t destMuid, uint8_t destination,
                                              std::array<uint8_t, 5> profile, uint8_t InquiryTarget, uint16_t datalen, uint8_t*  data){
    if(midiCIVer < 2) return 0;
    MIDICI midiCiHeader;
    midiCiHeader.ciType = MIDICI_PROFILE_SPECIFIC_DATA;
    midiCiHeader.localMUID = srcMUID;
    midiCiHeader.remoteMUID = destMuid;
    midiCiHeader.deviceId = destination;
    midiCiHeader.ciVer = midiCIVer;
    createCIHeader(sysex, midiCiHeader);
    uint16_t length = 13;
    concatSysexArray(sysex,&length,profile.data(),5);
    sysex[length++] = InquiryTarget;
    setBytesFromNumbers(sysex, datalen, &length, 2);
    concatSysexArray(sysex,&length,data,datalen);
    return length;
}


// Property Exchange

uint16_t sendPECapabilityRequest(uint8_t* sysex, uint8_t midiCIVer, uint32_t srcMUID, uint32_t destMuid,
                                              uint8_t numSimulRequests, uint8_t majVer, uint8_t minVer){
    
    MIDICI midiCiHeader;
    midiCiHeader.ciType = MIDICI_PE_CAPABILITY;
    midiCiHeader.localMUID = srcMUID;
    midiCiHeader.remoteMUID = destMuid;
    midiCiHeader.ciVer = midiCIVer;
    createCIHeader(sysex, midiCiHeader);
    sysex[13] = numSimulRequests;
    if(midiCIVer==1){
        return 14;
    }
    sysex[14]=majVer;
    sysex[15]=minVer;
    return 16;
}

uint16_t sendPECapabilityReply(uint8_t* sysex, uint8_t midiCIVer, uint32_t srcMUID, uint32_t destMuid,
                                            uint8_t numSimulRequests, uint8_t majVer, uint8_t minVer){
    MIDICI midiCiHeader;
    midiCiHeader.ciType = MIDICI_PE_CAPABILITYREPLY;
    midiCiHeader.localMUID = srcMUID;
    midiCiHeader.remoteMUID = destMuid;
    midiCiHeader.ciVer = midiCIVer;
    createCIHeader(sysex, midiCiHeader);
    sysex[13] = numSimulRequests;
    if(midiCIVer==1){
        return 14;
    }
    sysex[14]=majVer;
    sysex[15]=minVer;
    return 16;
}



uint16_t sendPEWithBody(uint8_t* sysex, uint8_t midiCIVer, uint32_t srcMUID, uint32_t destMuid, uint8_t requestId,
                                     uint16_t headerLen, uint8_t* header, uint16_t numberOfChunks, uint16_t numberOfThisChunk,
                                     uint16_t bodyLength, uint8_t* body, uint8_t ciType){
    MIDICI midiCiHeader;
    midiCiHeader.ciType = ciType;
    midiCiHeader.localMUID = srcMUID;
    midiCiHeader.remoteMUID = destMuid;
    midiCiHeader.ciVer = midiCIVer;
    createCIHeader(sysex, midiCiHeader);
    sysex[13] = requestId;
    uint16_t length = 14;
    setBytesFromNumbers(sysex, headerLen, &length, 2);
    concatSysexArray(sysex, &length, header,headerLen);
    setBytesFromNumbers(sysex, numberOfChunks, &length, 2);
    setBytesFromNumbers(sysex, numberOfThisChunk, &length, 2);
    setBytesFromNumbers(sysex, bodyLength, &length, 2);
    concatSysexArray(sysex, &length, body,bodyLength);
    return length;
}

uint16_t sendPESub(uint8_t* sysex, uint8_t midiCIVer, uint32_t srcMUID, uint32_t destMuid, uint8_t requestId,
                                uint16_t headerLen, uint8_t* header, uint16_t numberOfChunks, uint16_t numberOfThisChunk,
                                uint16_t bodyLength , uint8_t* body ){
    return sendPEWithBody(sysex, midiCIVer, srcMUID, destMuid, requestId, headerLen, header, numberOfChunks, numberOfThisChunk,
                   bodyLength , body , (uint8_t) MIDICI_PE_SUB);
}

uint16_t sendPESet(uint8_t* sysex, uint8_t midiCIVer, uint32_t srcMUID, uint32_t destMuid, uint8_t requestId,
                                uint16_t headerLen, uint8_t* header, uint16_t numberOfChunks, uint16_t numberOfThisChunk,
                                uint16_t bodyLength , uint8_t* body ){
    return sendPEWithBody(sysex, midiCIVer, srcMUID, destMuid, requestId, headerLen, header, numberOfChunks, numberOfThisChunk,
                   bodyLength , body , (uint8_t) MIDICI_PE_SET);
}

uint16_t sendPEGetReply(uint8_t* sysex, uint8_t midiCIVer, uint32_t srcMUID, uint32_t destMuid, uint8_t requestId,
                                     uint16_t headerLen, uint8_t* header, uint16_t numberOfChunks,
                                     uint16_t numberOfThisChunk, uint16_t bodyLength , uint8_t* body ){
    return sendPEWithBody(sysex, midiCIVer, srcMUID, destMuid, requestId, headerLen, header, numberOfChunks, numberOfThisChunk,
                   bodyLength , body , (uint8_t) MIDICI_PE_GETREPLY);
}


uint16_t sendPEHeaderOnly(uint8_t* sysex, uint8_t midiCIVer, uint32_t srcMUID, uint32_t destMuid, uint8_t requestId,
                                       uint16_t headerLen, uint8_t* header, uint8_t ciType){
    MIDICI midiCiHeader;
    midiCiHeader.ciType = ciType;
    midiCiHeader.localMUID = srcMUID;
    midiCiHeader.remoteMUID = destMuid;
    midiCiHeader.ciVer = midiCIVer;
    createCIHeader(sysex, midiCiHeader);
    sysex[13] = requestId;
    uint16_t length = 14;
    setBytesFromNumbers(sysex, headerLen, &length, 2);
    concatSysexArray(sysex, &length, header,headerLen);
    setBytesFromNumbers(sysex, 1, &length, 2);
    setBytesFromNumbers(sysex, 1, &length, 2);
    setBytesFromNumbers(sysex, 0, &length, 2);
    return length;
}

uint16_t sendPEGet(uint8_t* sysex, uint8_t midiCIVer, uint32_t srcMUID, uint32_t destMuid, uint8_t requestId,
                                uint16_t headerLen, uint8_t* header){
    return sendPEHeaderOnly(sysex, midiCIVer,  srcMUID, destMuid, requestId, headerLen, header, (uint8_t) MIDICI_PE_GET);
}

uint16_t sendPESubReply(uint8_t* sysex, uint8_t midiCIVer, uint32_t srcMUID, uint32_t destMuid, uint8_t requestId,
                                     uint16_t headerLen, uint8_t* header){
    return sendPEHeaderOnly(sysex, midiCIVer,  srcMUID, destMuid, requestId, headerLen, header, (uint8_t) MIDICI_PE_SUBREPLY);
}

uint16_t sendPENotify(uint8_t* sysex, uint8_t midiCIVer, uint32_t srcMUID, uint32_t destMuid, uint8_t requestId,
                                   uint16_t headerLen, uint8_t* header){
    return sendPEHeaderOnly(sysex, midiCIVer,  srcMUID, destMuid, requestId, headerLen, header, (uint8_t) MIDICI_PE_NOTIFY);
}

uint16_t sendPESetReply(uint8_t* sysex, uint8_t midiCIVer, uint32_t srcMUID, uint32_t destMuid, uint8_t requestId,
                                     uint16_t headerLen, uint8_t* header){
    return sendPEHeaderOnly(sysex, midiCIVer,  srcMUID, destMuid, requestId, headerLen, header, (uint8_t) MIDICI_PE_SETREPLY);
}
//*****
//uint16_t sendPEGetReplyStreamStart(uint8_t* sysex, uint8_t midiCIVer, uint32_t srcMUID, uint32_t destMuid, uint8_t requestId,
//                                                uint16_t headerLen, uint8_t* header, uint16_t numberOfChunks,
//                                                uint16_t numberOfThisChunk, uint16_t bodyLength){
//
//    MIDICI midiCiHeader;
//    midiCiHeader.ciType = MIDICI_PE_GETREPLY;
//    midiCiHeader.localMUID = srcMUID;
//    midiCiHeader.remoteMUID = destMuid;
//    midiCiHeader.ciVer = midiCIVer;
//    createCIHeader(sysex, midiCiHeader);
//    sysex[13] = requestId;
//    uint16_t length = 14;
//    setBytesFromNumbers(sysex, headerLen, 1, 2);
//    length +=2;
//    concatSysexArray(sysex, length, header,headerLen);
//    length += headerLen;
//
//    setBytesFromNumbers(sysex, numberOfChunks, 0, 2);
//    setBytesFromNumbers(sysex, numberOfThisChunk, 2, 2);
//    setBytesFromNumbers(sysex, bodyLength, 4, 2);
//    sendOutSysex(group,sysex,6,2);
//}
//
//uint16_t sendPEGetReplyStreamContinue(uint8_t* sysex, uint8_t midiCIVer, uint16_t partialLength, uint8_t* part, bool last ){
//
//    sendOutSysex(group,part,partialLength, last?3:2);
//}

//Process Inquiry
uint16_t sendPICapabilityRequest(uint8_t* sysex, uint8_t midiCIVer, uint32_t srcMUID, uint32_t destMuid){
    if(midiCIVer==1) return 0;

    MIDICI midiCiHeader;
    midiCiHeader.ciType = MIDICI_PI_CAPABILITY;
    midiCiHeader.localMUID = srcMUID;
    midiCiHeader.remoteMUID = destMuid;
    midiCiHeader.ciVer = midiCIVer;
    createCIHeader(sysex, midiCiHeader);
    return 13;
}

uint16_t sendPICapabilityReply(uint8_t* sysex, uint8_t midiCIVer,  uint32_t srcMUID, uint32_t destMuid, uint8_t supportedFeatures){


    MIDICI midiCiHeader;
    midiCiHeader.ciType = MIDICI_PI_CAPABILITYREPLY;
    midiCiHeader.localMUID = srcMUID;
    midiCiHeader.remoteMUID = destMuid;
    midiCiHeader.ciVer = midiCIVer;
    createCIHeader(sysex, midiCiHeader);
    sysex[13] = supportedFeatures;
    return 14;
}


uint16_t sendPIMMReport(uint8_t* sysex, uint8_t midiCIVer,  uint32_t srcMUID, uint32_t destMuid, uint8_t destination,
                                     uint8_t MDC, uint8_t systemBitmap,
                                     uint8_t chanContBitmap, uint8_t chanNoteBitmap){
    MIDICI midiCiHeader;
    midiCiHeader.ciType = MIDICI_PI_MM_REPORT;
    midiCiHeader.localMUID = srcMUID;
    midiCiHeader.remoteMUID = destMuid;
    midiCiHeader.deviceId = destination;
    midiCiHeader.ciVer = midiCIVer;
    createCIHeader(sysex, midiCiHeader);
    sysex[13] = MDC;
    sysex[14] = systemBitmap;
    sysex[15] = 0;
    sysex[16] = chanContBitmap;
    sysex[17] = chanNoteBitmap;
    return 18;
}

uint16_t sendPIMMReportReply(uint8_t* sysex, uint8_t midiCIVer,  uint32_t srcMUID, uint32_t destMuid, uint8_t destination,
                                          uint8_t systemBitmap,
                                          uint8_t chanContBitmap, uint8_t chanNoteBitmap){

    MIDICI midiCiHeader;
    midiCiHeader.ciType = MIDICI_PI_MM_REPORT_REPLY;
    midiCiHeader.localMUID = srcMUID;
    midiCiHeader.remoteMUID = destMuid;
    midiCiHeader.deviceId = destination;
    midiCiHeader.ciVer = midiCIVer;
    createCIHeader(sysex, midiCiHeader);
    sysex[13] = systemBitmap;
    sysex[14] = 0;
    sysex[15] = chanContBitmap;
    sysex[16] = chanNoteBitmap;
    return 17;
}

uint16_t sendPIMMReportEnd(uint8_t* sysex, uint8_t midiCIVer,  uint32_t srcMUID, uint32_t destMuid, uint8_t destination){
    MIDICI midiCiHeader;
    midiCiHeader.ciType = MIDICI_PI_MM_REPORT_END;
    midiCiHeader.localMUID = srcMUID;
    midiCiHeader.remoteMUID = destMuid;
    midiCiHeader.deviceId = destination;
    midiCiHeader.ciVer = midiCIVer;
    createCIHeader(sysex, midiCiHeader);
    return 13;
}



