# umpProcessor
Processing of UMP messages.

This class allows the application to set up a set of callbacks relevant to the applications needs.

__Example Setup__
```c++
umpProcessor UMPHandler;

UMPHandler.setNoteOn(handleNoteOn);
UMPHandler.setNoteOff(handleNoteOff);
UMPHandler.setControlChange(handleCC);
UMPHandler.setProgramChange(handleProgramChange);
UMPHandler.setTimingClock(handleClock);

UMPHandler.setMidiEndpoint(midiendpoint);
UMPHandler.setFunctionBlock(functionblock);
UMPHandler.setRawSysEx(processUMPSysex);
...
```

## Common Methods
### void processUMP(uint32_t UMP)
Process incoming UMP messages broken up into 32bit words.
### void clearUMP()
Reset the current processing of incoming UMP.


## Common Channel Voice Message Handlers

These are the common handles for UMP Messages received and processed by ```processUMP```. 

```umpProcessor ``` makes some distinction between different Protocols. This means that Channel Voice Messages (e.g. 
Note On) handlers are called the same way regardless if is a MIDI 1.0 Channel Voice Message (Message Type 2) or a MIDI 
2.0 Channel Voice Message (Message Type 4). MIDI 1.0 Channel Voice Message values are scaled to match MIDI 2.0 Messages. 

This allows for ```umpProcessor``` to process both types of Channel Voice Messages simultaneously.

Jitter Reduction Messages are also handled using handlers for those Messages. It is up to the 
application to manage the combination of JR messages and other UMP messages.

If the application does not use a particular message it does not need to be handled.

### inline void setNoteOff(noteOffCallback)
Set the callable function when a Note Off is processed by ```processUMP```
```c++ 
void noteOffCallback(uint8_t group, uint8_t messageType, uint8_t channel, uint8_t noteNumber, uint16_t velocity, 
        uint8_t attributeType, uint16_t attributeData){
    printf("->MIDI Off: CH %d Note: %d Velocity: %d", channel, noteNumber, velocity);
} 
```
* ```group``` is the UMP Group
* ```messageType``` is the UMP Message Type 

_Note: Velocity is scaled to 16 bits if ```messageType``` equals 0x2_


### inline void setMidiNoteOn(noteOnCallback)
_See Note Off Callback for structure of ```noteOnCallback```_

### inline void setControlChange(ccCallBack)
```c++ 
void ccCallback(uint8_t group, uint8_t messageType, uint8_t channel, uint8_t index, uint32_t value){
    printf("->MIDI CC: CH %d index: %d value: %d", channel, messageType, value);
} 
```
_Note: Value is scaled to 32 bits if ```messageType``` equals 0x2_

### inline void setRPN(rpnCallBack)
```c++ 
void rpnCallback(uint8_t group, uint8_t channel, uint8_t bank, uint8_t index, uint32_t value){
    printf("->MIDI RPN: CH %d index: %d value: %d", channel, messageType, value);
} 
```
_Note: This message is only triggered when a MIDI 2.0 RPN message is sent. This is not triggered when a MIDI 1.0 RPN 
message is sent. Those messages are processed using the function set by ```setControlChange```_

### inline void setNRPN(nrpnCallBack)
_See RPN Callback for structure of ```nrpnCallback```_
_Note: This message is only triggered when a MIDI 2.0 RPN message is sent. This is not triggered when a MIDI 1.0 RPN
message is sent. Those messages are processed using the function set by ```setControlChange```_



### inline void setRelativeRPN(void (*fptr)(uint8_t group, uint8_t channel,uint8_t bank,  uint8_t index, int32_t value))

MIDI 2.0 Channel Voice Message only.

### inline void setRelativeNRPN(void (*fptr)(uint8_t group, uint8_t channel,uint8_t bank,  uint8_t index, int32_t value)

MIDI 2.0 Channel Voice Message only.

### inline void setPolyPressure(void (\*fptr)(uint8_t group, uint8_t channel, uint8_t noteNumber, uint32_t pressure))
### inline void setChannelPressure(void (\*fptr)(uint8_t group, uint8_t channel, uint32_t pressure))
### inline void setPitchBend(void (\*fptr)(uint8_t group, uint8_t channel, uint32_t value))
### inline void setProgramChange(void (\*fptr)(uint8_t group, uint8_t channel, uint8_t program, bool bankValid, uint8_t bank, uint8_t index))

MIDI 1.0 Program Change Messages the ```bankValid``` is always false.

### inline void setRpnPerNote(void (*fptr)(uint8_t group, uint8_t channel, uint8_t noteNumber, uint8_t index, uint32_t value))
### inline void setNrpnPerNote(void (*fptr)(uint8_t group, uint8_t channel, uint8_t noteNumber, uint8_t index, uint32_t value))
###  inline void setPerNoteManage(void (*fptr)(uint8_t group, uint8_t channel, uint8_t noteNumber, bool detach, bool reset))
### inline void setPerNotePB(void (*fptr)(uint8_t group, uint8_t channel, uint8_t noteNumber,  uint32_t value))

## Common System Message Handlers

### inline void setTimingCode(void (\*fptr)(uint8_t group,uint8_t timeCode))
### inline void setSongSelect(void (\*fptr)(uint8_t group,uint8_t song))
### inline void setSongPositionPointer(void (*fptr)(uint8_t group,uint16_t position))
### inline void setTuneRequest(void (\*fptr)(uint8_t group))
### inline void setTimingClock(void (\*fptr)(uint8_t group))
### inline void setSeqStart(void (\*fptr)(uint8_t group))
### inline void setSeqCont(void (\*fptr)(uint8_t group))
### inline void setSeqStop(void (\*fptr)(uint8_t group))
### inline void setActiveSense(void (\*fptr)(uint8_t group))
### inline void setSystemReset(void (\*fptr)(uint8_t group))



## Common SysEx Handlers

###  inline void setRawSysEx(processUMPSysex)
```c++ 
midiCIProcessor midiciMain1;

void processUMPSysex(uint8_t group, uint8_t *sysex ,uint8_t length, uint8_t state){
    //Example of Processing UMP into MIDI-CI processor
    if(state==1 && sysex[0] == S7UNIVERSAL_NRT && sysex[2] == S7MIDICI){
        if(group==0) midiciMain1.startSysex7(group, sysex[1]);
    }
    for (int i = 0; i < length; i++) {
        if(group==0){
            midiciMain1.processMIDICI(sysex[i]);
        }
    }
    if(state==3){
        if(group==0) midiciMain1.endSysex7();
    }
}
```

## UMP Stream Messages

### inline void setMidiEndpoint(midiEndpointCallback)

__Example Processing of Endpoint Get:__
```c++ 
void midiEndpointCallback(uint8_t majVer, uint8_t minVer, uint8_t filter){
    //Upon Recieving the filter it is important to return the information requested
    if(filter & 0x1){ //Endpoint Info Notification
        std::array<uint32_t, 4> ump = mtFMidiEndpointInfoNotify(3, false, true, false, false);
        sendUMP(ump.data(),4);
    }

    if(filter & 0x2) {
        std::array<uint32_t, 4> ump = mtFMidiEndpointDeviceInfoNotify({DEVICE_MFRID}, {DEVICE_FAMID}, {DEVICE_MODELID}, {DEVICE_VERSIONID});
        sendUMP( ump.data(), 4);
    }

    if(filter & 0x4) {
        uint8_t friendlyNameLength = strlen(DEVICE_MIDIENPOINTNAME);
        for(uint8_t offset=0; offset<friendlyNameLength; offset+=14) {
            std::array<uint32_t, 4> ump = mtFMidiEndpointTextNotify(MIDIENDPOINT_NAME_NOTIFICATION, offset, (uint8_t *) DEVICE_MIDIENPOINTNAME,friendlyNameLength);
            sendUMP(ump.data(),4);
        }
    }
    
    if(filter & 0x8) {
        int8_t piiLength = strlen(PRODUCT_INSTANCE_ID);

        for(uint8_t offset=0; offset<piiLength; offset+=14) {
            std::array<uint32_t, 4> ump = mtFMidiEndpointTextNotify(PRODUCT_INSTANCE_ID, offset, (uint8_t *) buff,piiLength);
            sendUMP(ump.data(),4);
        }
    }
    
    if(filter & 0x10){
        std::array<uint32_t, 4> ump = mtFNotifyProtocol(0x1,false,false);
        sendUMP(ump.data(),4);
    }
} 
```
### inline void setMidiEndpointNameNotify(void (*fptr)(uint8_t form, uint8_t nameLength, uint8_t* name))
### inline void setMidiEndpointProdIdNotify(void (*fptr)(uint8_t form, uint8_t nameLength, uint8_t* name))
### inline void setMidiEndpointInfoNotify(void (*fptr)(uint8_t majVer, uint8_t minVer, uint8_t numOfFuncBlocks, bool m2, bool m1, bool rxjr, bool txjr))
### inline void setMidiEndpointDeviceInfoNotify(void (*fptr)(std::array<uint8_t, 3> manuId, std::array<uint8_t, 2> familyId, std::array<uint8_t, 2> modelId, std::array<uint8_t, 4> version))
### inline void setJRProtocolReq(void (*fptr)(uint8_t protocol, bool jrrx, bool jrtx))
### inline void setJRProtocolNotify(void (*fptr)(uint8_t protocol, bool jrrx, bool jrtx))
### inline void setFunctionBlock(void (*fptr)(uint8_t filter, uint8_t fbIdx))
### inline void setFunctionBlockNotify(void (*fptr)(uint8_t fbIdx, bool active, uint8_t direction, uint8_t firstGroup, uint8_t groupLength, bool midiCIValid, uint8_t midiCIVersion, uint8_t isMIDI1, uint8_t maxS8Streams))
### inline void setFunctionBlockNotify(void (*fptr)(uint8_t fbIdx, uint8_t form, uint8_t nameLength, uint8_t* name))
