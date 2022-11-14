# MIDI-CI

```midiCIProcessor``` handles MIDI-CI messages and tries to reduce complexity in processing messages. It does 
not attempt to solve the logic and handling of that data. It is up to the application to send responses to queries 
from an Initiator.

```c++
midiCIProcessor MIDICIHandler;
MIDICIHandler.setCheckMUID(checkMUID);
MIDICIHandler.setRecvDiscovery(discoveryHandler);

```
## Process Handling Commands

#### inline void setCheckMUID(bool (*fptr)(uint8_t group, uint32_t muid))
#### void startSysex7(uint8_t group, uint8_t deviceId)
#### void endSysex7();
#### void processMIDICI(uint8_t s7Byte);

## Common MIDI-CI SysEx Methods


#### inline void setRecvDiscovery(void (\*fptr)(uint8_t group, uint32_t remoteMuid, uint8_t ciVer, uint8_t\* manuId, uint8_t\* famId, uint8_t\* modelId, uint8_t\* verId, uint8_t ciSupport, uint16_t maxSysex))
Process Incoming Discovery Request Device details or Reply to Discovery Device details. When the class recieves a Discovery Message it will automatically reply with a Reply to Discovery Message. This is sent to the function set by ```setRawSysEx```.

####  inline void setRecvDiscoveryReply(void (*fptr)(MIDICI ciDetails, std::array<uint8_t, 3> manuId, std::array<uint8_t, 2> familyId, std::array<uint8_t, 2> modelId, std::array<uint8_t, 4> version, uint8_t ciSupport, uint16_t maxSysex,  uint8_t outputPathId, uint8_t fbIdx))
#### inline void setRecvNAK(MIDICI ciDetails, uint8_t origSubID, uint8_t statusCode, uint8_t statusData, uint8_t* ackNakDetails, uint16_t messageLength, uint8_t* ackNakMessage)
#### inline void setRecvInvalidateMUID(void (\*fptr)(uint8_t group, uint32_t remoteMuid, uint32_t terminateMuid))

#### inline void setRecvUnknownMIDICI(void (*fptr)(MIDICI ciDetails, uint8_t s7Byte))
#### inline void setRecvEndpointInfo(void (*fptr)(MIDICI ciDetails, uint8_t status))

#### inline void setRecvEndpointInfoReply(void (*fptr)(MIDICI ciDetails, uint8_t status, uint16_t infoLength, uint8_t* infoData)

### Protocol Negotiation 
####  inline void setRecvProtocolAvailable(void (*fptr)(MIDICI ciDetails, uint8_t authorityLevel, uint8_t* protocol)

####  inline void setRecvSetProtocol(void (*fptr)(MIDICI ciDetails, uint8_t authorityLevel, uint8_t* protocol)

####  inline void setRecvSetProtocolConfirm(void (*fptr)(MIDICI ciDetails, uint8_t authorityLevel))
####  inline void setRecvSetProtocolTest(void (*fptr)(MIDICI ciDetails, uint8_t authorityLevel, bool testDataAccurate)

### Profile Configuration 
#### inline void setRecvProfileInquiry(void (*fptr)(MIDICI ciDetails))
#### inline void setRecvProfileEnabled(void (*fptr)(MIDICI ciDetails, std::array<uint8_t, 5>, uint8_t numberOfChannels))
#### inline void setRecvSetProfileRemoved(void (*fptr)(MIDICI ciDetails, std::array<uint8_t, 5>))
#### inline void setRecvProfileDisabled(void (*fptr)(MIDICI ciDetails, std::array<uint8_t, 5>, uint8_t numberOfChannels))
#### inline void setRecvProfileOn(void (*fptr)(MIDICI ciDetails, std::array<uint8_t, 5> profile, uint8_t numberOfChannels))
#### inline void setRecvProfileOff(void (*fptr)(MIDICI ciDetails, std::array<uint8_t, 5> profile))
#### inline void setRecvProfileDetails(void (*fptr)(MIDICI ciDetails, std::array<uint8_t, 5> profile, uint16_t datalen, uint8_t*  data, uint16_t part, bool lastByteOfSet))
#### inline void setRecvProfileDetailsInquiry(void (*fptr)(MIDICI ciDetails, std::array<uint8_t, 5> profile,   uint8_t InquiryTarget))
#### inline void setRecvProfileDetailsReply(void (*fptr)(MIDICI ciDetails, std::array<uint8_t, 5> profile, uint8_t InquiryTarget, uint16_t datalen, uint8_t*  data))

### Property Exchange
#### inline void setPECapabilities(void (*fptr)(MIDICI ciDetails, uint8_t numSimulRequests, uint8_t majVer, uint8_t minVer))
#### inline void setPECapabilitiesReply(void (*fptr)(MIDICI ciDetails, uint8_t numSimulRequests, uint8_t majVer, uint8_t minVer))
#### inline void setRecvPEGetInquiry(void (*fptr)(MIDICI ciDetails,  peHeader requestDetails))
#### inline void setRecvPESetReply(void (*fptr)(MIDICI ciDetails,  peHeader requestDetails))
#### inline void setRecvPESubReply(void (*fptr)(MIDICI ciDetails,  peHeader requestDetails))
#### inline void setRecvPENotify(void (*fptr)(MIDICI ciDetails,  peHeader requestDetails))
#### inline void setRecvPESetInquiry(void (*fptr)(MIDICI ciDetails,  peHeader requestDetails, uint16_t bodyLen, uint8_t*  body, bool lastByteOfChunk, bool lastByteOfSet))
#### inline void setRecvPESubInquiry(void (*fptr)(MIDICI ciDetails,  peHeader requestDetails, uint16_t bodyLen, uint8_t*  body, bool lastByteOfChunk, bool lastByteOfSet))

### Process Inquiry
#### inline void setRecvPICapabilities(void (*fptr)(MIDICI ciDetails))
#### inline void setRecvPICapabilitiesReply(void (*fptr)(MIDICI ciDetails, uint8_t supportedFeatures))

#### inline void setRecvPIMMReport(void (*fptr)(MIDICI ciDetails, uint8_t MDC, uint8_t systemBitmap,uint8_t chanContBitmap, uint8_t chanNoteBitmap))
#### inline void setRecvPIMMReportReply(void (*fptr)(MIDICI ciDetails, uint8_t systemBitmap, uint8_t chanContBitmap, uint8_t chanNoteBitmap)){
#### inline void setRecvPIMMEnd(void (*fptr)(MIDICI ciDetails)){recvPIMMReportEnd = fptr;}