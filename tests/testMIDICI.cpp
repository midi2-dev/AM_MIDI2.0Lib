#include "../include/midiCIMessageCreate.h"
#include "../include/midiCIProcessor.h"
#include <cstdio>
#include <cassert>
#include <cstring>

extern int testPassed;
extern int testFailed;

void passFailCI(bool condition, const char* message) {
    if (condition) {
        printf("  PASS: %s\n", message);
        testPassed++;
    } else {
        printf("  FAIL: %s\n", message);
        testFailed++;
    }
}

void testDiscovery() {
    printf("Test Discovery Request and Reply\n");
    uint8_t sysex[512];
    std::array<uint8_t, 3> manuId = {{0x00, 0x01, 0x02}};
    std::array<uint8_t, 2> famId = {{0x03, 0x04}};
    std::array<uint8_t, 2> modelId = {{0x05, 0x06}};
    std::array<uint8_t, 4> version = {{0x07, 0x08, 0x09, 0x0A}};
    uint32_t srcMUID = 0x1234567;
    uint32_t destMUID = 0x7654321;

    midiCIProcessor processor;
    
    // Discovery Request
    bool discoveryReceived = false;
    processor.setRecvDiscovery([&](MIDICI ciDetails, std::array<uint8_t, 3> rManuId, std::array<uint8_t, 2> rFamId, std::array<uint8_t, 2> rModelId, std::array<uint8_t, 4> rVersion, uint8_t rCiSupport, uint16_t rMaxSysex, uint8_t rOutputPathId) {
        discoveryReceived = true;
        passFailCI(ciDetails.remoteMUID == srcMUID, "Discovery Request MUID matches");
        passFailCI(rManuId == manuId, "Discovery Request ManuId matches");
    });

    uint16_t len = CIMessage::sendDiscoveryRequest(sysex, 0x01, srcMUID, manuId, famId, modelId, version, 0x0F, 0x200, 0);
    processor.startSysex7(0, 0x7F);
    for (uint16_t i = 0; i < len; i++) processor.processMIDICI(sysex[i]);
    processor.endSysex7();
    passFailCI(discoveryReceived, "Discovery Request received");

    // Discovery Reply
    bool discoveryReplyReceived = false;
    processor.setRecvDiscoveryReply([&](MIDICI ciDetails, std::array<uint8_t, 3> rManuId, std::array<uint8_t, 2> rFamId, std::array<uint8_t, 2> rModelId, std::array<uint8_t, 4> rVersion, uint8_t rCiSupport, uint16_t rMaxSysex, uint8_t rOutputPathId, uint8_t fbIdx) {
        discoveryReplyReceived = true;
        passFailCI(ciDetails.remoteMUID == srcMUID, "Discovery Reply MUID matches");
        passFailCI(ciDetails.localMUID == destMUID, "Discovery Reply Dest MUID matches");
    });

    len = CIMessage::sendDiscoveryReply(sysex, 0x01, srcMUID, destMUID, manuId, famId, modelId, version, 0x0F, 0x200, 0, 0);
    processor.startSysex7(0, 0x7F);
    for (uint16_t i = 0; i < len; i++) processor.processMIDICI(sysex[i]);
    processor.endSysex7();
    passFailCI(discoveryReplyReceived, "Discovery Reply received");
}

void testProtocols() {
    printf("Test Protocol Messages\n");
    uint8_t sysex[512];
    uint32_t srcMUID = 0x1111111;
    uint32_t destMUID = 0x2222222;
    uint8_t protocols[] = {0x01, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00};
    uint8_t currentProtocol[] = {0x02, 0x00, 0x00, 0x00, 0x00};

    midiCIProcessor processor;

    // Protocol Negotiation
    int protocolAvailCount = 0;
    bool protocolConfirmReceived = false;
    processor.setRecvProtocolAvailable([&](MIDICI ciDetails, uint8_t authority, uint8_t* rProtocol) {
        protocolAvailCount++;
    });
    processor.setRecvSetProtocolConfirm([&](MIDICI ciDetails, uint8_t authority) {
        protocolConfirmReceived = true;
    });

    uint16_t len = CIMessage::sendProtocolNegotiation(sysex, 0x02, srcMUID, destMUID, 0x10, 2, protocols, currentProtocol);
    processor.startSysex7(0, 0x7F);
    for (uint16_t i = 0; i < len; i++) processor.processMIDICI(sysex[i]);
    processor.endSysex7();
    passFailCI(protocolAvailCount == 2, "Protocol Negotiation available protocols received");
    passFailCI(protocolConfirmReceived, "Protocol Negotiation confirm received");

    // Set Protocol
    bool setProtocolReceived = false;
    processor.setRecvSetProtocol([&](MIDICI ciDetails, uint8_t authority, uint8_t* rProtocol) {
        setProtocolReceived = true;
        passFailCI(memcmp(rProtocol, currentProtocol, 5) == 0, "Set Protocol matches");
    });
    len = CIMessage::sendSetProtocol(sysex, 0x02, srcMUID, destMUID, 0x10, currentProtocol);
    processor.startSysex7(0, 0x7F);
    for (uint16_t i = 0; i < len; i++) processor.processMIDICI(sysex[i]);
    processor.endSysex7();
    passFailCI(setProtocolReceived, "Set Protocol received");

    // Protocol Test
    bool protocolTestReceived = false;
    processor.setRecvSetProtocolTest([&](MIDICI ciDetails, uint8_t authority, bool accurate) {
        protocolTestReceived = true;
        passFailCI(accurate, "Protocol Test accurate");
    });
    len = CIMessage::sendProtocolTest(sysex, 0x02, srcMUID, destMUID, 0x10);
    processor.startSysex7(0, 0x7F);
    for (uint16_t i = 0; i < len; i++) processor.processMIDICI(sysex[i]);
    processor.endSysex7();
    passFailCI(protocolTestReceived, "Protocol Test received");
}

void testProfiles() {
    printf("Test Profile Messages\n");
    uint8_t sysex[512];
    uint32_t srcMUID = 0x3333333;
    uint32_t destMUID = 0x4444444;
    std::array<uint8_t, 5> profile = {{0x7F, 0x01, 0x02, 0x03, 0x04}};

    midiCIProcessor processor;

    // Profile Inquiry
    bool inquiryReceived = false;
    processor.setRecvProfileInquiry([&](MIDICI ciDetails) {
        inquiryReceived = true;
        passFailCI(ciDetails.remoteMUID == srcMUID, "Profile Inquiry MUID matches");
    });
    uint16_t len = CIMessage::sendProfileListRequest(sysex, 0x02, srcMUID, destMUID, 0x7F);
    processor.startSysex7(0, 0x7F);
    for (uint16_t i = 0; i < len; i++) processor.processMIDICI(sysex[i]);
    processor.endSysex7();
    passFailCI(inquiryReceived, "Profile Inquiry received");

    // Profile On
    bool profileOnReceived = false;
    processor.setRecvProfileOn([&](MIDICI ciDetails, std::array<uint8_t, 5> rProfile, uint8_t channels) {
        profileOnReceived = true;
        passFailCI(rProfile == profile, "Profile On matches");
    });
    len = CIMessage::sendProfileOn(sysex, 0x02, srcMUID, destMUID, 0x7F, profile, 1);
    processor.startSysex7(0, 0x7F);
    for (uint16_t i = 0; i < len; i++) processor.processMIDICI(sysex[i]);
    processor.endSysex7();
    passFailCI(profileOnReceived, "Profile On received");

    // Profile Specific Data
    bool profileSpecificReceived = false;
    uint8_t specData[] = {0x10, 0x20, 0x30, 0x40};
    processor.setRecvProfileSpecificData([&](MIDICI ciDetails, std::array<uint8_t, 5> rProfile, uint16_t dLen, uint8_t* data, uint16_t part, bool last) {
        profileSpecificReceived = true;
        passFailCI(dLen == 4 && data[0] == 0x10, "Profile Specific Data matches");
    });
    len = CIMessage::sendProfileSpecificData(sysex, 0x02, srcMUID, destMUID, 0x7F, profile, 4, specData);
    processor.startSysex7(0, 0x7F);
    for (uint16_t i = 0; i < len; i++) processor.processMIDICI(sysex[i]);
    processor.endSysex7();
    passFailCI(profileSpecificReceived, "Profile Specific Data received");
}

void testInvalidateMUID() {
    printf("Test Invalidate MUID\n");
    uint8_t sysex[512];
    uint32_t srcMUID = 0x1234567;
    uint32_t terminateMUID = 0x7654321;

    midiCIProcessor processor;
    bool invalidateReceived = false;
    processor.setRecvInvalidateMUID([&](MIDICI ciDetails, uint32_t rTerminateMUID) {
        invalidateReceived = true;
        passFailCI(rTerminateMUID == terminateMUID, "Terminate MUID matches");
    });

    uint16_t len = CIMessage::sendInvalidateMUID(sysex, 0x01, srcMUID, terminateMUID);
    processor.startSysex7(0, 0x7F);
    for (uint16_t i = 0; i < len; i++) processor.processMIDICI(sysex[i]);
    processor.endSysex7();
    passFailCI(invalidateReceived, "Invalidate MUID received");
}

void testACKNAK() {
    printf("Test ACK/NAK Messages\n");
    uint8_t sysex[512];
    uint32_t srcMUID = 0x5555555;
    uint32_t destMUID = 0x6666666;
    uint8_t details[5] = {1, 2, 3, 4, 5};
    uint8_t msg[] = "Error message";

    midiCIProcessor processor;

    // NAK
    bool nakReceived = false;
    processor.setRecvNAK([&](MIDICI ciDetails, uint8_t origSubID, uint8_t statusCode, uint8_t statusData, uint8_t* rDetails, uint16_t rLen, uint8_t* rMsg) {
        nakReceived = true;
        passFailCI(statusCode == 0x11, "NAK status code matches");
        passFailCI(rLen == sizeof(msg) - 1, "NAK message length matches");
    });
    uint16_t len = CIMessage::sendNAK(sysex, 0x02, srcMUID, destMUID, 0x7F, 0x01, 0x11, 0x22, details, sizeof(msg) - 1, msg);
    processor.startSysex7(0, 0x7F);
    for (uint16_t i = 0; i < len; i++) {
        processor.processMIDICI(sysex[i]);
    }
    processor.endSysex7();
    passFailCI(nakReceived, "NAK received");

    // ACK
    bool ackReceived = false;
    processor.setRecvACK([&](MIDICI ciDetails, uint8_t origSubID, uint8_t statusCode, uint8_t statusData, uint8_t* rDetails, uint16_t rLen, uint8_t* rMsg) {
        ackReceived = true;
        passFailCI(statusCode == 0x06, "ACK status code matches");
    });
    len = CIMessage::sendACK(sysex, 0x02, srcMUID, destMUID, 0x7F, 0x01, 0x06, 0x00, details, 0, nullptr);
    processor.startSysex7(0, 0x7F);
    for (uint16_t i = 0; i < len; i++) {
        processor.processMIDICI(sysex[i]);
    }
    processor.endSysex7();
    passFailCI(ackReceived, "ACK received");
}

void testPE() {
    printf("Test Property Exchange Messages\n");
    uint8_t sysex[512];
    uint32_t srcMUID = 0x7777777;
    uint32_t destMUID = 0x8888888;

    midiCIProcessor processor;

    // PE Capabilities
    bool peCapReceived = false;
    processor.setPECapabilities([&](MIDICI ciDetails, uint8_t numSimul, uint8_t maj, uint8_t min) {
        peCapReceived = true;
        passFailCI(numSimul == 1, "PE Capabilities numSimul matches");
    });
    uint16_t len = CIMessage::sendPECapabilityRequest(sysex, 0x02, srcMUID, destMUID, 1, 1, 0);
    processor.startSysex7(0, 0x7F);
    for (uint16_t i = 0; i < len; i++) processor.processMIDICI(sysex[i]);
    processor.endSysex7();
    passFailCI(peCapReceived, "PE Capabilities received");

    // PE Get Inquiry
    bool peGetInquiryReceived = false;
    std::string header = "{\"resource\":\"Resource\"}";
    processor.setRecvPEGetInquiry([&](MIDICI ciDetails, std::string rHeader) {
        peGetInquiryReceived = true;
        passFailCI(rHeader == header, "PE Get Inquiry header matches");
    });
    len = CIMessage::sendPEGet(sysex, 0x02, srcMUID, destMUID, 0x01, header.length(), (uint8_t*)header.c_str());
    processor.startSysex7(0, 0x7F);
    for (uint16_t i = 0; i < len; i++) processor.processMIDICI(sysex[i]);
    processor.endSysex7();
    passFailCI(peGetInquiryReceived, "PE Get Inquiry received");
}

void testProcessInquiry() {
    printf("Test Process Inquiry Messages\n");
    uint8_t sysex[512];
    uint32_t srcMUID = 0x9999999;
    uint32_t destMUID = 0xAAAAAAA;

    midiCIProcessor processor;

    // Process Inquiry Capabilities
    bool piCapReceived = false;
    processor.setRecvPICapabilities([&](MIDICI ciDetails) {
        piCapReceived = true;
    });
    uint16_t len = CIMessage::sendPICapabilityRequest(sysex, 0x02, srcMUID, destMUID);
    processor.startSysex7(0, 0x7F);
    for (uint16_t i = 0; i < len; i++) processor.processMIDICI(sysex[i]);
    processor.endSysex7();
    passFailCI(piCapReceived, "Process Inquiry Capabilities received");

    // Process Inquiry Capabilities Reply
    bool piCapReplyReceived = false;
    processor.setRecvPICapabilitiesReply([&](MIDICI ciDetails, uint8_t features) {
        piCapReplyReceived = true;
        passFailCI(features == 0x7F, "Process Inquiry Capabilities features match");
    });
    len = CIMessage::sendPICapabilityReply(sysex, 0x02, srcMUID, destMUID, 0x7F);
    processor.startSysex7(0, 0x7F);
    for (uint16_t i = 0; i < len; i++) processor.processMIDICI(sysex[i]);
    processor.endSysex7();
    passFailCI(piCapReplyReceived, "Process Inquiry Capabilities Reply received");
}

void runMIDICITests() {
    testDiscovery();
   // testProtocols();
    testProfiles();
    testInvalidateMUID();
    testACKNAK();
    testPE();
    testProcessInquiry();
}
