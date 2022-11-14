# Profile Configuration handling in midi2Processor
This enables processing of MIDI-CI Profiles. 

### void sendProfileListRequest(uint8_t group, uint32_t remoteMuid, uint8_t ciVer, uint8_t destination)
Send a Request to get list of Profiles. This will generate the SysEx data that is then set to the function provided by ```setRawSysEx```, which is then sent based on how the applications wishes to send out data.

Use a destination of 0x7F to get all Profiles across all channels.
Enabled and Disabled Profiles are sent to the ```setRecvProfileEnabled``` and ```setRecvProfileDisabled``` respectively.

```c++
midi2Processor MIDI2 (0,2,2);
//Send a Request to Group 1 to to the remote MUID using MIDI CI ver 1 to the whole Port.
//This should return the Profiles on ALL channels and any port-wide Profiles.
MIDI2.sendProfileListRequest(0, remoteMuid, 1, 0x7F);
```



### void sendProfileListResponse(uint8_t group, uint32_t remoteMuid, uint8_t ciVer, uint8_t destination, uint8_t profilesEnabledLen, uint8_t\* profilesEnabled, uint8_t profilesDisabledLen , uint8_t\* profilesDisabled)
Send a Profile List Response Message. This will generate the SysEx data that is then set to the function provided by ```setRawSysEx```, which is then sent based on how the applications wishes to send out data.

```profilesEnabledLen``` and ```profilesDisabledLen``` represent how many Profiles. ```profilesEnabled``` and ```profilesDisabled``` arguments should be 5 times the length of ```profilesEnabledLen``` and ```profilesDisabledLen``` respectively.

```c++
void profileInquiry(uint8_t group, uint32_t remoteMUID, uint8_t destination){  
  uint8_t profileNone[0] = {};
  
  // If a Profile Inquiry is Recieved where destination = 0x7F, you should also return 
  // the Profiles on each channel. In this example Destination of 0 = channel 1, so
  // the Profile is also returned for Channel 1 or destination = 0x7F
  if(destination == 0 || destination == 0x7F){
    uint8_t profileDrawBar[5] = {0x7E, 0x40, 0x01, 0x01};
    MIDI2.sendProfileListResponse(group, remoteMUID, 1, 0, 1, profileDrawBar, 0, profileNone);
  }

  if(destination == 0x7F){
   MIDI2.sendProfileListResponse(group, remoteMUID, 1, 0x7F, 0, profileNone, 0, profileNone);
  }
}

midi2Processor MIDI2 (0,2,2);
...
MIDI2.setRecvProfileInquiry(profileInquiry);
```





### void sendProfileOn(uint8_t group, uint32_t remoteMuid, uint8_t ciVer, uint8_t destination, uint8_t\* profile)
This will generate the SysEx data that is then set to the function provided by ```setRawSysEx```, which is then sent based on how the applications wishes to send out data.

```profile``` is always 5 bytes.

### void sendProfileOff(uint8_t group, uint32_t remoteMuid, uint8_t ciVer, uint8_t destination, uint8_t\* profile)

This will generate the SysEx data that is then set to the function provided by ```setRawSysEx```, which is then sent based on how the applications wishes to send out data.

### void sendProfileEnabled(uint8_t group, uint32_t remoteMuid, uint8_t ciVer, uint8_t destination, uint8_t\* profile)

This will generate the SysEx data that is then set to the function provided by ```setRawSysEx```, which is then sent based on how the applications wishes to send out data.

### void sendProfileDisabled(uint8_t group, uint32_t remoteMuid, uint8_t ciVer, uint8_t destination, uint8_t\* profile)

This will generate the SysEx data that is then set to the function provided by ```setRawSysEx```, which is then sent based on how the applications wishes to send out data.

### inline void setRecvProfileInquiry(void (\*fptr)(uint8_t group, uint32_t remoteMuid, uint8_t destination))
```sendProfileListResponse``` should be used in Response to this Message.

### inline void setRecvProfileEnabled(void (\*fptr)(uint8_t group, uint32_t remoteMuid, uint8_t destination, uint8_t\* profile))
### inline void setRecvProfileDisabled(void (\*fptr)(uint8_t group, uint32_t remoteMuid, uint8_t destination, uint8_t\* profile))
### inline void setRecvProfileOn(void (\*fptr)(uint8_t group, uint32_t remoteMuid, uint8_t destination, uint8_t\* profile))
### inline void setRecvProfileOff(void (\*fptr)(uint8_t group, uint32_t remoteMuid, uint8_t destination, uint8_t\* profile))