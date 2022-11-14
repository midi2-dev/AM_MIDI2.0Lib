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
The ```midiCIProcessor``` takes in 1 byte at a time when processing MIDI-CI. This is done so longer complex SysEx does 
not require lots of memory and processing can occur as the  UMP Sysex is delivered.

```c++ 
midiCIProcessor midiciMain1;
bool isProcMIDICI = false;

void processUMPSysex(uint8_t group, uint8_t *sysex ,uint8_t length, uint8_t state){
    //Example of Processing UMP into MIDI-CI processor
    if(state==1 && sysex[0] == S7UNIVERSAL_NRT && sysex[2] == S7MIDICI){
        if(group==0) {
            midiciMain1.startSysex7(group, sysex[1]);
            isProcMIDICI = true;
        }
    }
    for (int i = 0; i < length; i++) {
        if(group==0 && isProcMIDICI){
            midiciMain1.processMIDICI(sysex[i]);
        }else{
            //Process other SysEx
        }
    }
    if(state==3 && group==0 && isProcMIDICI){
        midiciMain1.endSysex7();
        isProcMIDICI = false;
    }
}
```

#### void startSysex7(uint8_t group, uint8_t deviceId)
#### void endSysex7();
#### void processMIDICI(uint8_t s7Byte);

#### inline void setCheckMUID(checkMUIDCallback)
This is a simple check to make sure that the message being processed is meant for this application. 
Return true if it is a match.

```c++
uint32_t m2procMUID = random(0xFFFFEFF);

bool checkMUIDCallback(uint8_t group, uint32_t muid){
    return (m2procMUID==muid);
}
```

_Note: it is recommended that all instances of this class support this callback._

## Common structs in use

#### MIDICI
```c++
struct MIDICI{
    uint8_t umpGroup; //zero-based
    uint8_t deviceId; //0x00-0x0F Channels 1-16, 0x7E Group, 0x7F Function Blocks
    uint8_t ciType;
    uint8_t ciVer; //0x1 - v1.1, 0x2 - v1.2
    uint32_t remoteMUID;
    uint32_t localMUID;
};
```
Most of the callbacks will include a MIDI-CI struct that contain core information from MIDI-CI SysEx.
The ```umpGroup``` variable is set by startSysex7 method.

#### peHeader
```c++
struct peHeader {
    uint8_t requestId;
    char resource[PE_HEAD_BUFFERLEN];
    char resId[PE_HEAD_BUFFERLEN];
    uint8_t command; //MIDICI_PE_COMMAND_START MIDICI_PE_COMMAND_END MIDICI_PE_COMMAND_PARTIAL MIDICI_PE_COMMAND_FULL MIDICI_PE_COMMAND_NOTIFY 
    uint8_t action; //MIDICI_PE_ACTION_COPY MIDICI_PE_ACTION_MOVE MIDICI_PE_ACTION_DELETE MIDICI_PE_ACTION_CREATE_DIR 4
    char subscribeId[PE_HEAD_BUFFERLEN];
    char path[EXP_MIDICI_PE_EXPERIMENTAL_PATH];
    int  offset;
    int  limit;
    int  status;
    bool partial;
    int mutualEncoding; // MIDICI_PE_ASCII MIDICI_PE_MCODED7 MIDICI_PE_MCODED7ZLIB 
    char mediaType[PE_HEAD_BUFFERLEN];
};
```
Property Exchange MIDI-CI messages also have a struct to contain data that is needed for processing Property Exchange
messages.

## Common MIDI-CI SysEx Methods

#### inline void setRecvDiscovery(recvDiscoveryCallback)
```c++
void recvDiscoveryCallback(struct MIDICI ciDetails, std::array<uint8_t, 3> manuId, std::array<uint8_t, 2> familyId,
                   std::array<uint8_t, 2> modelId, std::array<uint8_t, 4> version, uint8_t remoteciSupport,
                   uint16_t remotemaxSysex, uint8_t outputPathId
){
    //All MIDI-CI Devices shall reply to a Discovery message
    printf("Received Discover on Group %d remote MUID: %d\n", ciDetails.umpGroup, ciDetails.remoteMUID);
    unint8_t sysexBuffer[64];
    int len = sendDiscoveryReply(sysexBuffer, MIDICI_MESSAGEFORMATVERSION,m2procMUID, ciDetails.remoteMUID,
                             {DEVICE_MFRID}, {DEVICE_FAMID}, {DEVICE_MODELID},
                             {DEVICE_VERSIONID},0,
                             512, outputPathId,
                             0 //fbIdx
    );
    sendOutSysex(ciDetails.umpGroup, sysexBuffer ,len );
}
```

_Note: it is recommended that all instances of this class support this callback._

####  inline void setRecvDiscoveryReply(recvDiscoveryReplyCallback)

```c++
void recvDiscoveryReplyCallback(struct MIDICI ciDetails, std::array<uint8_t, 3> manuId, std::array<uint8_t, 2> familyId,
                   std::array<uint8_t, 2> modelId, std::array<uint8_t, 4> version, uint8_t remoteciSupport,
                   uint16_t remotemaxSysex, uint8_t outputPathId, uint8_t functionBlockIdx
){
    printf("Received Discover Reply on Group %d remote MUID: %d\n", ciDetails.umpGroup, ciDetails.remoteMUID);
}
```

#### inline void setRecvNAK(recvNAKCallback)
```c++
void recvNAKCallback(struct MIDICI ciDetails, uint8_t origSubID, uint8_t statusCode, uint8_t statusData, 
        uint8_t* ackNakDetails, uint16_t messageLength, uint8_t* ackNakMessage
){
    printf("Received NAK on Group %d remote MUID: %d\n", ciDetails.umpGroup, ciDetails.remoteMUID);
}
```

#### inline void setRecvInvalidateMUID(void (\*fptr)(uint8_t group, uint32_t remoteMuid, uint32_t terminateMuid))

#### inline void setRecvUnknownMIDICI(void (\*fptr)(MIDICI ciDetails, uint8_t s7Byte))
#### inline void setRecvEndpointInfo(void (\*fptr)(MIDICI ciDetails, uint8_t status))

#### inline void setRecvEndpointInfoReply(void (\*fptr)(MIDICI ciDetails, uint8_t status, uint16_t infoLength, uint8_t\* infoData)

### Protocol Negotiation (MIDI-CI 1.1)
Protocol Negotiation is deprecated from MIDI-CI 1.2 onwards. However, Devices that support a later version of MIDI-CI
can still respond and handle Protocol Negotiation.

####  inline void setRecvProtocolAvailable(void (\*fptr)(MIDICI ciDetails, uint8_t authorityLevel, uint8_t\* protocol)
####  inline void setRecvSetProtocol(void (\*fptr)(MIDICI ciDetails, uint8_t authorityLevel, uint8_t\* protocol)
####  inline void setRecvSetProtocolConfirm(void (\*fptr)(MIDICI ciDetails, uint8_t authorityLevel))
####  inline void setRecvSetProtocolTest(void (\*fptr)(MIDICI ciDetails, uint8_t authorityLevel, bool testDataAccurate)

### Profile Configuration 
#### inline void setRecvProfileInquiry(void (\*fptr)(MIDICI ciDetails))

#### inline void setRecvProfileEnabled(void (\*fptr)(MIDICI ciDetails, std::array<uint8_t, 5>, uint8_t numberOfChannels))
#### inline void setRecvSetProfileRemoved(void (\*fptr)(MIDICI ciDetails, std::array<uint8_t, 5>))
#### inline void setRecvProfileDisabled(void (\*fptr)(MIDICI ciDetails, std::array<uint8_t, 5>, uint8_t numberOfChannels))
#### inline void setRecvProfileOn(void (\*fptr)(MIDICI ciDetails, std::array<uint8_t, 5> profile, uint8_t numberOfChannels))
#### inline void setRecvProfileOff(void (\*fptr)(MIDICI ciDetails, std::array<uint8_t, 5> profile))
#### inline void setRecvProfileDetails(void (\*fptr)(MIDICI ciDetails, std::array<uint8_t, 5> profile, uint16_t datalen, uint8_t\*  data, uint16_t part, bool lastByteOfSet))
#### inline void setRecvProfileDetailsInquiry(void (\*fptr)(MIDICI ciDetails, std::array<uint8_t, 5> profile,   uint8_t InquiryTarget))
#### inline void setRecvProfileDetailsReply(void (\*fptr)(MIDICI ciDetails, std::array<uint8_t, 5> profile, uint8_t InquiryTarget, uint16_t datalen, uint8_t\*  data))

### Property Exchange
#### inline void setPECapabilities(void (\*fptr)(MIDICI ciDetails, uint8_t numSimulRequests, uint8_t majVer, uint8_t minVer))
#### inline void setPECapabilitiesReply(void (\*fptr)(MIDICI ciDetails, uint8_t numSimulRequests, uint8_t majVer, uint8_t minVer))
#### inline void setRecvPEGetInquiry(void (\*fptr)(MIDICI ciDetails,  peHeader requestDetails))
#### inline void setRecvPESetReply(void (\*fptr)(MIDICI ciDetails,  peHeader requestDetails))
#### inline void setRecvPESubReply(void (\*fptr)(MIDICI ciDetails,  peHeader requestDetails))
#### inline void setRecvPENotify(void (\*fptr)(MIDICI ciDetails,  peHeader requestDetails))
#### inline void setRecvPESetInquiry(void (\*fptr)(MIDICI ciDetails,  peHeader requestDetails, uint16_t bodyLen, uint8_t\*  body, bool lastByteOfChunk, bool lastByteOfSet))
#### inline void setRecvPESubInquiry(void (\*fptr)(MIDICI ciDetails,  peHeader requestDetails, uint16_t bodyLen, uint8_t\*  body, bool lastByteOfChunk, bool lastByteOfSet))

### Process Inquiry
#### inline void setRecvPICapabilities(void (\*fptr)(MIDICI ciDetails))
#### inline void setRecvPICapabilitiesReply(void (\*fptr)(MIDICI ciDetails, uint8_t supportedFeatures))

#### inline void setRecvPIMMReport(void (\*fptr)(MIDICI ciDetails, uint8_t MDC, uint8_t systemBitmap,uint8_t chanContBitmap, uint8_t chanNoteBitmap))
#### inline void setRecvPIMMReportReply(void (\*fptr)(MIDICI ciDetails, uint8_t systemBitmap, uint8_t chanContBitmap, uint8_t chanNoteBitmap)){
#### inline void setRecvPIMMEnd(void (\*fptr)(MIDICI ciDetails)){recvPIMMReportEnd = fptr;}