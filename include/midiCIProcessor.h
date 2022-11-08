//
// Created by andrew on 31/10/22.
//

#ifndef MIDI2CPP_MIDICIPROCESSOR_H
#define MIDI2CPP_MIDICIPROCESSOR_H

#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <map>

#include "utils.h"

class midiCIProcessor{
private:
    MIDICI midici;
    uint8_t buffer[S7_BUFFERLEN];
    /*
     * in Discovery this is [sysexID1,sysexID2,sysexID3,famId1,famid2,modelId1,modelId2,ver1,ver2,ver3,ver4,...product Id]
     * in Profiles this is [pf1, pf1, pf3, pf4, pf5]
     * in Protocols this is [pr1, pr2, pr3, pr4, pr5]
     */

    uint16_t intTemp[4];
    /* in Discovery this is [ciSupport, maxSysex, output path id]
     * in Profile Inquiry Reply, this is [Enabled Profiles Length, Disabled Profile Length]
     * in Profile On/Off/Enabled/Disabled, this is [numOfChannels]
     * in PE this is [header length, Body Length]
     */
    uint16_t sysexPos;

    //MIDI-CI  callbacks
    bool (*checkMUID)(uint8_t group, uint32_t muid) = nullptr;
    void (*recvDiscoveryRequest)(MIDICI ciDetails,
                                 std::array<uint8_t, 3> manuId, std::array<uint8_t, 2> familyId,
                                 std::array<uint8_t, 2> modelId, std::array<uint8_t, 4> version, uint8_t ciSupport,
                                 uint16_t maxSysex, uint8_t outputPathId/*, uint8_t productInstanceIdLength,
                                 uint8_t* productInstanceId*/) = nullptr;
    void (*recvDiscoveryReply)(MIDICI ciDetails, std::array<uint8_t, 3> manuId, std::array<uint8_t, 2> familyId,
                               std::array<uint8_t, 2> modelId, std::array<uint8_t, 4> version, uint8_t ciSupport, uint16_t maxSysex,
                               uint8_t outputPathId,
                               uint8_t fbIdx
    ) = nullptr;
    void (*recvEndPointInfo)(MIDICI ciDetails, uint8_t status) = nullptr;
    void (*recvEndPointInfoReply)(MIDICI ciDetails, uint8_t status, uint16_t infoLength,
                                  uint8_t* infoData) = nullptr;
    void (*recvNAK)(MIDICI ciDetails, uint8_t origSubID, uint8_t statusCode,
                    uint8_t statusData, uint8_t* ackNakDetails, uint16_t messageLength,
                    uint8_t* ackNakMessage) = nullptr;
    void (*recvACK)(MIDICI ciDetails, uint8_t origSubID, uint8_t statusCode,
                    uint8_t statusData, uint8_t* ackNakDetails, uint16_t messageLength,
                    uint8_t* ackNakMessage) = nullptr;
    void (*recvInvalidateMUID)(MIDICI ciDetails, uint32_t terminateMuid) = nullptr;
    void (*recvUnknownMIDICI)(MIDICI ciDetails, uint8_t s7Byte) = nullptr;

//Protocol Negotiation
    void processProtocolSysex(uint8_t s7Byte);
    void (*recvProtocolAvailable)(MIDICI ciDetails, uint8_t authorityLevel, uint8_t* protocol) = nullptr;
    void (*recvSetProtocol)(MIDICI ciDetails, uint8_t authorityLevel, uint8_t* protocol) = nullptr;
    void (*recvSetProtocolConfirm)(MIDICI ciDetails, uint8_t authorityLevel) = nullptr;
    void (*recvProtocolTest)(MIDICI ciDetails, uint8_t authorityLevel, bool testDataAccurate) = nullptr;

//Profiles
    void (*recvProfileInquiry)(MIDICI ciDetails) = nullptr;
    void (*recvSetProfileEnabled)(MIDICI ciDetails, std::array<uint8_t, 5> profile, uint8_t numberOfChannels) = nullptr;
    void (*recvSetProfileRemoved)(MIDICI ciDetails, std::array<uint8_t, 5>) = nullptr;
    void (*recvSetProfileDisabled)(MIDICI ciDetails, std::array<uint8_t, 5>, uint8_t numberOfChannels) = nullptr;
    void (*recvSetProfileOn)(MIDICI ciDetails, std::array<uint8_t, 5> profile, uint8_t numberOfChannels) = nullptr;
    void (*recvSetProfileOff)(MIDICI ciDetails, std::array<uint8_t, 5> profile) = nullptr;
    void (*recvProfileDetails)(MIDICI ciDetails, std::array<uint8_t, 5> profile, uint16_t datalen, uint8_t*  data,
                               uint16_t part, bool lastByteOfSet) = nullptr;
    void (*recvSetProfileDetailsInquiry)(MIDICI ciDetails, std::array<uint8_t, 5> profile, uint8_t InquiryTarget) = nullptr;
    void (*recvSetProfileDetailsReply)(MIDICI ciDetails, std::array<uint8_t, 5> profile, uint8_t InquiryTarget,
                                       uint16_t datalen, uint8_t*  data) = nullptr;

    void processProfileSysex(uint8_t s7Byte);

//Property Exchange
    std::map<reqId ,peHeader> peRequestDetails;

    void (*recvPECapabilities)(MIDICI ciDetails, uint8_t numSimulRequests, uint8_t majVer, uint8_t minVer) = nullptr;
    void (*recvPECapabilitiesReplies)(MIDICI ciDetails, uint8_t numSimulRequests, uint8_t majVer, uint8_t minVer) = nullptr;
    void (*recvPEGetInquiry)(MIDICI ciDetails, peHeader requestDetails) = nullptr;
    void (*recvPESetReply)(MIDICI ciDetails, peHeader requestDetails) = nullptr;
    void (*recvPESubReply)(MIDICI ciDetails, peHeader requestDetails) = nullptr;
    void (*recvPENotify)(MIDICI ciDetails, peHeader requestDetails) = nullptr;
    void (*recvPESetInquiry)(MIDICI ciDetails, peHeader requestDetails, uint16_t bodyLen, uint8_t*  body,
                             bool lastByteOfChunk, bool lastByteOfSet) = nullptr;
    void (*recvPESubInquiry)(MIDICI ciDetails, peHeader requestDetails, uint16_t bodyLen, uint8_t*  body,
                             bool lastByteOfChunk, bool lastByteOfSet) = nullptr;

    void cleanupRequest(reqId peRequestIdx);
    void processPEHeader(reqId peRequestIdx, uint8_t s7Byte);
    void processPESysex(uint8_t s7Byte);

//Process Inquiry
    void (*recvPICapabilities)(MIDICI ciDetails) = nullptr;
    void (*recvPICapabilitiesReply)(MIDICI ciDetails, uint8_t supportedFeatures) = nullptr;

    void (*recvPIMMReport)(MIDICI ciDetails, uint8_t MDC, uint8_t systemBitmap,
                           uint8_t chanContBitmap, uint8_t chanNoteBitmap) = nullptr;
    void (*recvPIMMReportReply)(MIDICI ciDetails, uint8_t systemBitmap,
                                uint8_t chanContBitmap, uint8_t chanNoteBitmap) = nullptr;
    void (*recvPIMMReportEnd)(MIDICI ciDetails) = nullptr;

    void processPISysex(uint8_t s7Byte);

public:



    void endSysex7();
    void startSysex7(uint8_t group, uint8_t deviceId);
    void processMIDICI(uint8_t s7Byte);


    inline void setRecvDiscovery(void (*fptr)(MIDICI ciDetails,std::array<uint8_t, 3> manuId, std::array<uint8_t, 2> familyId,
                                              std::array<uint8_t, 2> modelId, std::array<uint8_t, 4> version, uint8_t ciSupport, uint16_t maxSysex,
                                              uint8_t outputPathId
            //, uint8_t productInstanceIdLength, uint8_t* productInstanceId
    )){ recvDiscoveryRequest = fptr;}
    inline void setRecvDiscoveryReply(void (*fptr)(MIDICI ciDetails, std::array<uint8_t, 3> manuId, std::array<uint8_t, 2> familyId,
                                                   std::array<uint8_t, 2> modelId, std::array<uint8_t, 4> version, uint8_t ciSupport, uint16_t maxSysex,
                                                   uint8_t outputPathId,
                                                   uint8_t fbIdx
            //, uint8_t productInstanceIdLength, uint8_t* productInstanceId
    )){ recvDiscoveryReply = fptr;}
    inline void setRecvNAK(void (*fptr)(MIDICI ciDetails, uint8_t origSubID, uint8_t statusCode,
                                        uint8_t statusData, uint8_t* ackNakDetails, uint16_t messageLength,
                                        uint8_t* ackNakMessage)){ recvNAK = fptr;}
    inline void setRecvACK(void (*fptr)(MIDICI ciDetails, uint8_t origSubID, uint8_t statusCode,
                                        uint8_t statusData, uint8_t* ackNakDetails, uint16_t messageLength,
                                        uint8_t* ackNakMessage)){ recvACK = fptr;}
    inline void setRecvInvalidateMUID(void (*fptr)(MIDICI ciDetails, uint32_t terminateMuid)){
        recvInvalidateMUID = fptr;}
    inline void setRecvUnknownMIDICI(void (*fptr)(MIDICI ciDetails,
                                                  uint8_t s7Byte)){ recvUnknownMIDICI = fptr;}

    inline void setCheckMUID(bool (*fptr)(uint8_t group, uint32_t muid)){ checkMUID = fptr; }
    inline void setRecvEndpointInfo(void (*fptr)(MIDICI ciDetails, uint8_t status)){ recvEndPointInfo = fptr;}
    inline void setRecvEndpointInfoReply(void (*fptr)(MIDICI ciDetails, uint8_t status, uint16_t infoLength,
                                                      uint8_t* infoData)){ recvEndPointInfoReply = fptr;}

    //Protocol Negotiation
    inline void setRecvProtocolAvailable(void (*fptr)(MIDICI ciDetails, uint8_t authorityLevel,
                                                      uint8_t* protocol)){ recvProtocolAvailable = fptr;}
    inline void setRecvSetProtocol(void (*fptr)(MIDICI ciDetails, uint8_t authorityLevel,
                                                uint8_t* protocol)){ recvSetProtocol = fptr;}
    inline void setRecvSetProtocolConfirm(void (*fptr)(MIDICI ciDetails, uint8_t authorityLevel)){
        recvSetProtocolConfirm = fptr;}
    inline void setRecvSetProtocolTest(void (*fptr)(MIDICI ciDetails, uint8_t authorityLevel,
                                                    bool testDataAccurate)){ recvProtocolTest = fptr;}

    //Profiles
    inline void setRecvProfileInquiry(void (*fptr)(MIDICI ciDetails)){ recvProfileInquiry = fptr;}
    inline void setRecvProfileEnabled(void (*fptr)(MIDICI ciDetails, std::array<uint8_t, 5>, uint8_t numberOfChannels)){
        recvSetProfileEnabled = fptr;}
    inline void setRecvSetProfileRemoved(void (*fptr)(MIDICI ciDetails, std::array<uint8_t, 5>)){
        recvSetProfileRemoved = fptr;}
    inline void setRecvProfileDisabled(void (*fptr)(MIDICI ciDetails, std::array<uint8_t, 5>, uint8_t numberOfChannels)){
        recvSetProfileDisabled = fptr;}
    inline void setRecvProfileOn(void (*fptr)(MIDICI ciDetails, std::array<uint8_t, 5> profile, uint8_t numberOfChannels)){
        recvSetProfileOn = fptr;}
    inline void setRecvProfileOff(void (*fptr)(MIDICI ciDetails, std::array<uint8_t, 5> profile)){
        recvSetProfileOff = fptr;}
    inline void setRecvProfileDetails(void (*fptr)(MIDICI ciDetails, std::array<uint8_t, 5> profile, uint16_t datalen,
                                                   uint8_t*  data, uint16_t part, bool lastByteOfSet)){ recvProfileDetails = fptr;}
    inline void setRecvProfileDetailsInquiry(void (*fptr)(MIDICI ciDetails, std::array<uint8_t, 5> profile,
                                                          uint8_t InquiryTarget)){recvSetProfileDetailsInquiry = fptr;}
    inline void setRecvProfileDetailsReply(void (*fptr)(MIDICI ciDetails, std::array<uint8_t, 5> profile,
                                                        uint8_t InquiryTarget, uint16_t datalen, uint8_t*  data)){
        recvSetProfileDetailsReply = fptr;}

    //Property Exchange
    inline void setPECapabilities(void (*fptr)(MIDICI ciDetails, uint8_t numSimulRequests, uint8_t majVer, uint8_t minVer)){
        recvPECapabilities = fptr;}
    inline void setPECapabilitiesReply(void (*fptr)(MIDICI ciDetails, uint8_t numSimulRequests, uint8_t majVer, uint8_t minVer)){
        recvPECapabilitiesReplies = fptr;}
    inline void setRecvPEGetInquiry(void (*fptr)(MIDICI ciDetails,  peHeader requestDetails)){
        recvPEGetInquiry = fptr;}
    inline void setRecvPESetReply(void (*fptr)(MIDICI ciDetails,  peHeader requestDetails)){
        recvPESetReply = fptr;}
    inline void setRecvPESubReply(void (*fptr)(MIDICI ciDetails,  peHeader requestDetails)){
        recvPESubReply = fptr;}
    inline void setRecvPENotify(void (*fptr)(MIDICI ciDetails,  peHeader requestDetails)){
        recvPENotify = fptr;}
    inline void setRecvPESetInquiry(void (*fptr)(MIDICI ciDetails,  peHeader requestDetails,
                                                 uint16_t bodyLen, uint8_t*  body, bool lastByteOfChunk, bool lastByteOfSet)){ recvPESetInquiry = fptr;}
    inline void setRecvPESubInquiry(void (*fptr)(MIDICI ciDetails,  peHeader requestDetails,
                                                 uint16_t bodyLen, uint8_t*  body, bool lastByteOfChunk, bool lastByteOfSet)){ recvPESubInquiry = fptr;}

//Process Inquiry

    inline void setRecvPICapabilities(void (*fptr)(MIDICI ciDetails)){
        recvPICapabilities = fptr;}
    inline void setRecvPICapabilitiesReply(void (*fptr)(MIDICI ciDetails, uint8_t supportedFeatures)){
        recvPICapabilitiesReply = fptr;}

    inline void setRecvPIMMReport(void (*fptr)(MIDICI ciDetails, uint8_t MDC, uint8_t systemBitmap,
                                               uint8_t chanContBitmap, uint8_t chanNoteBitmap)){
        recvPIMMReport = fptr;}
    inline void setRecvPIMMReportReply(void (*fptr)(MIDICI ciDetails, uint8_t systemBitmap,
                                                    uint8_t chanContBitmap, uint8_t chanNoteBitmap)){
        recvPIMMReportReply = fptr;}
    inline void setRecvPIMMEnd(void (*fptr)(MIDICI ciDetails)){recvPIMMReportEnd = fptr;}

};

#endif //MIDI2CPP_MIDICIPROCESSOR_H
