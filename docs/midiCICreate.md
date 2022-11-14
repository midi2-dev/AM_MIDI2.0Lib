# MIDI-CI SysEx Creation
These function create the SysEx needed for various MIDI-CI messages. It creates the complete SysEx uint8_t array 
minus the 0xF0 and 0xF7 at beginning and end of the SysEx.

For each message a sysex buffer is needed. The return of each function is the length of the buffer used e.g.:
```c++
uint8_t sysexBuffer[512];
int len = sendDiscoveryReply(sysexBuffer, MIDICI_MESSAGEFORMATVERSION,m2procMUID, ciDetails.remoteMUID,
                             {DEVICE_MFRID}, {DEVICE_FAMID}, {DEVICE_MODELID},
                             {DEVICE_VERSIONID},0,
                             512, outputPathId, 0
);
sendOutSysextoUMP(umpGroup, sysexBuffer, len);
```
_Note: It is upto the application to make sure that the buffer is larger that the created message_

The SysEx created will depend on the ```midiCIVer``` value specified. For example if a Discovery Reply message was sent where 
the value is set to __1__, then the message created will not have Output Path Id and Function Block Index.

MIDI-CI Messages that are only available from certain version will return a length of 0 if the ```midiCIVer``` is too low.

#### uint16_t sendDiscoveryRequest(uint8_t* sysex, uint8_t midiCIVer, uint32_t srcMUID, std::array<uint8_t, 3> manuId, std::array<uint8_t, 2> familyId, std::array<uint8_t, 2> modelId, std::array<uint8_t, 4> version, uint8_t ciSupport, uint16_t sysExMax, uint8_t outputPathId)
#### uint16_t sendDiscoveryReply(uint8_t* sysex, uint8_t midiCIVer, uint32_t srcMUID, uint32_t destMuid, std::array<uint8_t, 3> manuId, std::array<uint8_t, 2> familyId, std::array<uint8_t, 2> modelId, std::array<uint8_t, 4> version, uint8_t ciSupport, uint16_t sysExMax, uint8_t outputPathId, uint8_t fbIdx)
#### uint16_t sendEndpointInfoRequest(uint8_t* sysex, uint8_t midiCIVer, uint32_t srcMUID, uint32_t destMuid, uint8_t status);
#### uint16_t sendEndpointInfoReply(uint8_t* sysex, uint8_t midiCIVer, uint32_t srcMUID, uint32_t destMuid, uint8_t status, uint16_t infoLength, uint8_t* infoData)

#### uint16_t sendACK(uint8_t* sysex, uint8_t midiCIVer, uint32_t srcMUID, uint32_t destMuid, uint8_t originalSubId, uint8_t statusCode, uint8_t statusData, uint8_t* ackNakDetails, uint16_t messageLength, uint8_t* ackNakMessage)
#### uint16_t sendNAK(uint8_t* sysex, uint8_t midiCIVer, uint32_t srcMUID, uint32_t destMuid, uint8_t originalSubId, uint8_t statusCode, uint8_t statusData, uint8_t* ackNakDetails, uint16_t messageLength, uint8_t* ackNakMessage)

#### uint16_t sendInvalidateMUID(uint8_t* sysex, uint8_t midiCIVer, uint32_t srcMUID, uint32_t terminateMuid);

## Protocol Negotiation (MIDI-CI 1.1)
Protocol Negotiation is deprecated from MIDI-CI 1.2 onwards. However, Devices that support a later version of MIDI-CI
can still respond and handle Protocol Negotiation.

#### uint16_t sendProtocolNegotiation(uint8_t* sysex, uint8_t midiCIVer, uint32_t srcMUID, uint32_t destMuid, uint8_t authorityLevel, uint8_t numProtocols, uint8_t* protocols, uint8_t* currentProtocol)
#### uint16_t sendProtocolNegotiationReply(uint8_t* sysex, uint8_t midiCIVer, uint32_t srcMUID, uint32_t destMuid, uint8_t authorityLevel, uint8_t numProtocols, uint8_t* protocols)
#### uint16_t sendSetProtocol(uint8_t* sysex, uint8_t midiCIVer, uint32_t srcMUID, uint32_t destMuid, uint8_t authorityLevel, uint8_t* protocol)
#### uint16_t sendProtocolTest(uint8_t* sysex, uint8_t midiCIVer, uint32_t srcMUID, uint32_t destMuid, uint8_t authorityLevel)
#### uint16_t sendProtocolTestResponder(uint8_t* sysex, uint8_t midiCIVer, uint32_t srcMUID, uint32_t destMuid, uint8_t authorityLevel)

## Profile Negotiation
#### uint16_t sendProfileListRequest(uint8_t* sysex, uint8_t midiCIVer, uint32_t srcMUID, uint32_t destMuid,  uint8_t destination)
#### uint16_t sendProfileListResponse(uint8_t* sysex, uint8_t midiCIVer, uint32_t srcMUID, uint32_t destMuid, uint8_t destination, uint8_t profilesEnabledLen, uint8_t* profilesEnabled, uint8_t profilesDisabledLen, uint8_t* profilesDisabled)
```profilesEnabledLen``` and ```profilesDisabledLen``` represent how many Profiles. ```profilesEnabled``` and ```profilesDisabled``` arguments should be 5 times the length of ```profilesEnabledLen``` and ```profilesDisabledLen``` respectively.

```c++
void handleProfileInquiry(uint8_t group, uint32_t remoteMUID, uint8_t destination){  
  uint8_t profileNone[0] = {};
  uint8_t sysexBuffer[512];
  int len;
  
  // If a Profile Inquiry is received where destination = 0x7F, you should also return 
  // the Profiles on each channel. In this example Destination of 0 = channel 1, so
  // the Profile is also returned for Channel 1 or destination = 0x7F
  if(destination == 0 || destination == 0x7F){
    uint8_t profileDrawBar[5] = {0x7E, 0x40, 0x01, 0x01};
    len = sendProfileListResponse(group, remoteMUID, 1, 0, 1, profileDrawBar, 0, profileNone);
    sendOutSysextoUMP(umpGroup, sysexBuffer, len);
  }

  if(destination == 0x7F){
   len = sendProfileListResponse(group, remoteMUID, 1, 0x7F, 0, profileNone, 0, profileNone);
   sendOutSysextoUMP(umpGroup, sysexBuffer, len);
  }
}

midi2Processor MIDI2 (0,2,2);
...
MIDI2.setRecvProfileInquiry(profileInquiry);
```

#### uint16_t sendProfileAdd(uint8_t* sysex, uint8_t midiCIVer, uint32_t srcMUID, uint32_t destMuid, uint8_t destination, std::array<uint8_t, 5> profile)
#### uint16_t sendProfileRemove(uint8_t* sysex, uint8_t midiCIVer, uint32_t srcMUID, uint32_t destMuid, uint8_t destination, std::array<uint8_t, 5> profile)

#### uint16_t sendProfileOn(uint8_t* sysex, uint8_t midiCIVer, uint32_t srcMUID, uint32_t destMuid, uint8_t destination, std::array<uint8_t, 5> profile, uint8_t numberOfChannels)
#### uint16_t sendProfileOff(uint8_t* sysex, uint8_t midiCIVer, uint32_t srcMUID, uint32_t destMuid, uint8_t destination, std::array<uint8_t, 5> profile)
#### uint16_t sendProfileEnabled(uint8_t* sysex, uint8_t midiCIVer, uint32_t srcMUID, uint32_t destMuid, uint8_t destination, std::array<uint8_t, 5> profile, uint8_t numberOfChannels)
#### uint16_t sendProfileDisabled(uint8_t* sysex, uint8_t midiCIVer, uint32_t srcMUID, uint32_t destMuid, uint8_t destination, std::array<uint8_t, 5> profile, uint8_t numberOfChannels)

#### uint16_t sendProfileSpecificData(uint8_t* sysex, uint8_t midiCIVer, uint32_t srcMUID, uint32_t destMuid, uint8_t destination, std::array<uint8_t, 5> profile, uint16_t datalen, uint8_t*  data)
#### uint16_t sendProfileDetailsInquiry(uint8_t* sysex, uint8_t midiCIVer,  uint32_t srcMUID, uint32_t destMuid, uint8_t destination, std::array<uint8_t, 5> profile, uint8_t InquiryTarget)
#### uint16_t sendProfileDetailsReply(uint8_t* sysex, uint8_t midiCIVer,  uint32_t srcMUID, uint32_t destMuid, uint8_t destination, std::array<uint8_t, 5> profile, uint8_t InquiryTarget, uint16_t datalen, uint8_t*  data)

## Property Exchange
#### uint16_t sendPECapabilityRequest(uint8_t* sysex, uint8_t midiCIVer, uint32_t srcMUID, uint32_t destMuid,  uint8_t numSimulRequests, uint8_t majVer, uint8_t minVer)
#### uint16_t sendPECapabilityReply(uint8_t* sysex, uint8_t midiCIVer, uint32_t srcMUID, uint32_t destMuid,  uint8_t numSimulRequests, uint8_t majVer, uint8_t minVer)

#### uint16_t sendPEGet(uint8_t* sysex, uint8_t midiCIVer, uint32_t srcMUID, uint32_t destMuid, uint8_t requestId, uint16_t headerLen, uint8_t* header)
#### uint16_t sendPESet(uint8_t* sysex, uint8_t midiCIVer, uint32_t srcMUID, uint32_t destMuid, uint8_t requestId, uint16_t headerLen, uint8_t* header, uint16_t numberOfChunks, uint16_t numberOfThisChunk, uint16_t bodyLength , uint8_t* body)

#### uint16_t sendPESub(uint8_t* sysex, uint8_t midiCIVer, uint32_t srcMUID, uint32_t destMuid, uint8_t requestId, uint16_t headerLen, uint8_t* header, uint16_t numberOfChunks, uint16_t numberOfThisChunk, uint16_t bodyLength , uint8_t* body)
#### uint16_t sendPEGetReply(uint8_t* sysex, uint8_t midiCIVer, uint32_t srcMUID, uint32_t destMuid, uint8_t requestId, uint16_t headerLen, uint8_t* header, uint16_t numberOfChunks, uint16_t numberOfThisChunk, uint16_t bodyLength , uint8_t* body )

#### uint16_t sendPESubReply(uint8_t* sysex, uint8_t midiCIVer, uint32_t srcMUID, uint32_t destMuid, uint8_t requestId, uint16_t headerLen, uint8_t* header)
#### uint16_t sendPENotify(uint8_t* sysex, uint8_t midiCIVer, uint32_t srcMUID, uint32_t destMuid, uint8_t requestId, uint16_t headerLen, uint8_t* header)
#### uint16_t sendPESetReply(uint8_t* sysex, uint8_t midiCIVer, uint32_t srcMUID, uint32_t destMuid, uint8_t requestId, uint16_t headerLen, uint8_t* header)


## Process Inquiry
#### uint16_t sendPICapabilityRequest(uint8_t* sysex, uint8_t midiCIVer, uint32_t srcMUID, uint32_t destMuid)
#### uint16_t sendPICapabilityReply(uint8_t* sysex, uint8_t midiCIVer, uint32_t srcMUID, uint32_t destMuid,  uint8_t supportedFeatures)
#### uint16_t sendPIMMReport(uint8_t* sysex, uint8_t midiCIVer, uint32_t srcMUID, uint32_t destMuid, uint8_t destination, uint8_t MDC,  uint8_t systemBitmap, uint8_t chanContBitmap, uint8_t chanNoteBitmap)
#### uint16_t sendPIMMReportReply(uint8_t* sysex, uint8_t midiCIVer, uint32_t srcMUID, uint32_t destMuid, uint8_t destination, uint8_t systemBitmap, uint8_t chanContBitmap, uint8_t chanNoteBitmap)
#### uint16_t sendPIMMReportEnd(uint8_t* sysex, uint8_t midiCIVer, uint32_t srcMUID, uint32_t destMuid, uint8_t destination)