# Property Exchange Handling in midi2Processor
This enable processing of MIDI-CI Property Exchange. These methods are available if ```#define M2_DISABLE_PE``` is not set.

Property Exchange requires a bit more memory than other parts of MIDI-CI. Enabling this will increase memory requirements.

### void sendPECapabilityRequest(uint8_t group, uint32_t remoteMuid, uint8_t ciVer, uint8_t numRequests)

This will generate the SysEx data that is then set to the function provided by ```setRawSysEx```, which is then sent based on how the applications wishes to send out data.

### void sendPEGet(uint8_t group, uint32_t remoteMuid, uint8_t ciVer, uint8_t requestId, uint16_t headerLen, uint8_t* header)

This will generate the SysEx data that is then set to the function provided by ```setRawSysEx```, which is then sent based on how the applications wishes to send out data.

### void sendPEGetReply(uint8_t group, uint32_t remoteMuid, uint8_t ciVer, uint8_t requestId, uint16_t headerLen, uint8_t\* header, int numberOfChunks, int numberOfThisChunk, uint16_t bodyLength , uint8_t\* body)

This will generate the SysEx data that is then set to the function provided by ```setRawSysEx```, which is then sent based on how the applications wishes to send out data.

### inline void setRecvPECapabilities(void (\*fptr)(uint8_t group, uint32_t remoteMuid, uint8_t numRequests))
### inline void setRecvPEGetInquiry(void (\*fptr)(uint8_t group, uint32_t remoteMuid,  peHeader requestDetails))
### inline void setRecvPESetInquiry(void (\*fptr)(uint8_t group, uint32_t remoteMuid,  peHeader requestDetails, uint8_t bodyLen, uint8_t\*  body))