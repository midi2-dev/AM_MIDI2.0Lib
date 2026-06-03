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

#ifndef UTILS_H
#define UTILS_H

#pragma once


#include <cstdint>
#include <tuple>
#include <cstdio>


namespace MIDI1_MSGS {

constexpr uint8_t NOTE_OFF = 0x80;
constexpr uint8_t NOTE_ON = 0x90;
constexpr uint8_t KEY_PRESSURE = 0xA0;
constexpr uint8_t CC = 0xB0;
constexpr uint8_t RPN = 0x20;
constexpr uint8_t NRPN = 0x30;
constexpr uint8_t RPN_RELATIVE = 0x40;
constexpr uint8_t NRPN_RELATIVE = 0x50;
constexpr uint8_t PROGRAM_CHANGE = 0xC0;
constexpr uint8_t CHANNEL_PRESSURE = 0xD0;
constexpr uint8_t PITCH_BEND = 0xE0;
constexpr uint8_t PITCH_BEND_PERNOTE = 0x60;
constexpr uint8_t NRPN_PERNOTE = 0x10;
constexpr uint8_t RPN_PERNOTE = 0x00;
constexpr uint8_t PERNOTE_MANAGE = 0xF0;

constexpr uint8_t SYSEX_START = 0xF0;
constexpr uint8_t TIMING_CODE = 0xF1;
constexpr uint8_t SPP = 0xF2;
constexpr uint8_t SONG_SELECT = 0xF3;
constexpr uint8_t TUNEREQUEST = 0xF6;
constexpr uint8_t SYSEX_STOP = 0xF7;
constexpr uint8_t TIMINGCLOCK = 0xF8;
constexpr uint8_t SEQSTART = 0xFA;
constexpr uint8_t SEQCONT = 0xFB;
constexpr uint8_t SEQSTOP = 0xFC;
constexpr uint8_t ACTIVESENSE = 0xFE;
constexpr uint8_t SYSTEMRESET = 0xFF;

constexpr uint8_t UTILITY_NOOP = 0x0;
constexpr uint8_t UTILITY_JRCLOCK = 0x1;
constexpr uint8_t UTILITY_JRTS = 0x2;
constexpr uint8_t UTILITY_DELTACLOCKTICK = 0x3;
constexpr uint8_t UTILITY_DELTACLOCKSINCE = 0x4;

constexpr uint8_t FLEXDATA_COMMON = 0x00;
constexpr uint8_t FLEXDATA_COMMON_TEMPO = 0x00;
constexpr uint8_t FLEXDATA_COMMON_TIMESIG = 0x01;
constexpr uint8_t FLEXDATA_COMMON_METRONOME = 0x02;
constexpr uint8_t FLEXDATA_COMMON_KEYSIG = 0x05;
constexpr uint8_t FLEXDATA_COMMON_CHORD = 0x06;
constexpr uint8_t FLEXDATA_PERFORMANCE = 0x01;
constexpr uint8_t FLEXDATA_LYRIC = 0x02;

constexpr uint16_t MIDIENDPOINT = 0x000;
constexpr uint16_t MIDIENDPOINT_INFO_NOTIFICATION = 0x001;
constexpr uint16_t MIDIENDPOINT_DEVICEINFO_NOTIFICATION = 0x002;
constexpr uint16_t MIDIENDPOINT_NAME_NOTIFICATION = 0x003;
constexpr uint16_t MIDIENDPOINT_PRODID_NOTIFICATION = 0x004;
constexpr uint16_t MIDIENDPOINT_STREAMCONFIG_REQUEST = 0x005;
constexpr uint16_t MIDIENDPOINT_STREAMCONFIG_NOTIFICATION = 0x006;
constexpr uint16_t STARTOFSEQ = 0x020;
constexpr uint16_t ENDOFFILE = 0x021;

constexpr uint16_t FUNCTIONBLOCK = 0x010;
constexpr uint16_t FUNCTIONBLOCK_INFO_NOTFICATION = 0x011;
constexpr uint16_t FUNCTIONBLOCK_NAME_NOTIFICATION = 0x012;

constexpr uint16_t S7_BUFFERLEN = 36;

constexpr uint8_t S7UNIVERSAL_NRT = 0x7E;
constexpr uint8_t S7UNIVERSAL_RT = 0x7F;
constexpr uint8_t S7MIDICI = 0x0D;

constexpr uint8_t MIDICI_DISCOVERY = 0x70;
constexpr uint8_t MIDICI_DISCOVERYREPLY = 0x71;
constexpr uint8_t MIDICI_ENDPOINTINFO = 0x72;
constexpr uint8_t MIDICI_ENDPOINTINFO_REPLY = 0x73;
constexpr uint8_t MIDICI_INVALIDATEMUID = 0x7E;
constexpr uint8_t MIDICI_ACK = 0x7D;
constexpr uint8_t MIDICI_NAK = 0x7F;

constexpr uint8_t MIDICI_PROTOCOL_NEGOTIATION = 0x10;
constexpr uint8_t MIDICI_PROTOCOL_NEGOTIATION_REPLY = 0x11;
constexpr uint8_t MIDICI_PROTOCOL_SET = 0x12;
constexpr uint8_t MIDICI_PROTOCOL_TEST = 0x13;
constexpr uint8_t MIDICI_PROTOCOL_TEST_RESPONDER = 0x14;
constexpr uint8_t MIDICI_PROTOCOL_CONFIRM = 0x15;

constexpr uint8_t MIDICI_PROFILE_INQUIRY = 0x20;
constexpr uint8_t MIDICI_PROFILE_INQUIRYREPLY = 0x21;
constexpr uint8_t MIDICI_PROFILE_SETON = 0x22;
constexpr uint8_t MIDICI_PROFILE_SETOFF = 0x23;
constexpr uint8_t MIDICI_PROFILE_ENABLED = 0x24;
constexpr uint8_t MIDICI_PROFILE_DISABLED = 0x25;
constexpr uint8_t MIDICI_PROFILE_ADD = 0x26;
constexpr uint8_t MIDICI_PROFILE_REMOVE = 0x27;
constexpr uint8_t MIDICI_PROFILE_DETAILS_INQUIRY = 0x28;
constexpr uint8_t MIDICI_PROFILE_DETAILS_REPLY = 0x29;
constexpr uint8_t MIDICI_PROFILE_SPECIFIC_DATA = 0x2F;

constexpr uint8_t MIDICI_PE_CAPABILITY = 0x30;
constexpr uint8_t MIDICI_PE_CAPABILITYREPLY = 0x31;
constexpr uint8_t MIDICI_PE_GET = 0x34;
constexpr uint8_t MIDICI_PE_GETREPLY = 0x35;
constexpr uint8_t MIDICI_PE_SET = 0x36;
constexpr uint8_t MIDICI_PE_SETREPLY = 0x37;
constexpr uint8_t MIDICI_PE_SUB = 0x38;
constexpr uint8_t MIDICI_PE_SUBREPLY = 0x39;
constexpr uint8_t MIDICI_PE_NOTIFY = 0x3F;

constexpr uint16_t MIDICI_PE_STATUS_OK = 200;
constexpr uint16_t MIDICI_PE_STATUS_ACCEPTED = 202;
constexpr uint16_t MIDICI_PE_STATUS_RESOURCE_UNAVAILABLE = 341;
constexpr uint16_t MIDICI_PE_STATUS_BAD_DATA = 342;
constexpr uint16_t MIDICI_PE_STATUS_TOO_MANY_REQS = 343;
constexpr uint16_t MIDICI_PE_STATUS_BAD_REQ = 400;
constexpr uint16_t MIDICI_PE_STATUS_REQ_UNAUTHORIZED = 403;
constexpr uint16_t MIDICI_PE_STATUS_RESOURCE_UNSUPPORTED = 404;
constexpr uint16_t MIDICI_PE_STATUS_RESOURCE_NOT_ALLOWED = 405;
constexpr uint16_t MIDICI_PE_STATUS_PAYLOAD_TOO_LARGE = 413;
constexpr uint16_t MIDICI_PE_STATUS_UNSUPPORTED_MEDIA_TYPE = 415;
constexpr uint16_t MIDICI_PE_STATUS_INVALID_DATA_VERSION = 445;
constexpr uint16_t MIDICI_PE_STATUS_INTERNAL_DEVICE_ERROR = 500;

constexpr uint8_t MIDICI_PI_CAPABILITY = 0x40;
constexpr uint8_t MIDICI_PI_CAPABILITYREPLY = 0x41;
constexpr uint8_t MIDICI_PI_MM_REPORT = 0x42;
constexpr uint8_t MIDICI_PI_MM_REPORT_REPLY = 0x43;
constexpr uint8_t MIDICI_PI_MM_REPORT_END = 0x44;

constexpr uint8_t MIDICI_PE_COMMAND_START = 1;
constexpr uint8_t MIDICI_PE_COMMAND_END = 2;
constexpr uint8_t MIDICI_PE_COMMAND_PARTIAL = 3;
constexpr uint8_t MIDICI_PE_COMMAND_FULL = 4;
constexpr uint8_t MIDICI_PE_COMMAND_NOTIFY = 5;

constexpr uint8_t MIDICI_PE_ACTION_COPY = 1;
constexpr uint8_t MIDICI_PE_ACTION_MOVE = 2;
constexpr uint8_t MIDICI_PE_ACTION_DELETE = 3;
constexpr uint8_t MIDICI_PE_ACTION_CREATE_DIR = 4;

constexpr uint8_t MIDICI_PE_ASCII = 1;
constexpr uint8_t MIDICI_PE_MCODED7 = 2;
constexpr uint8_t MIDICI_PE_MCODED7ZLIB = 3;

constexpr uint8_t FUNCTION_BLOCK = 0x7F;
constexpr uint32_t M2_CI_BROADCAST = 0xFFFFFFF;

constexpr uint8_t UMP_VER_MAJOR = 1;
constexpr uint8_t UMP_VER_MINOR = 1;

constexpr uint8_t EXP_MIDICI_PE_EXPERIMENTAL_PATH = 1;

constexpr uint8_t UMP_UTILITY = 0x0;
constexpr uint8_t UMP_SYSTEM = 0x1;
constexpr uint8_t UMP_M1CVM = 0x2;
constexpr uint8_t UMP_SYSEX7 = 0x3;
constexpr uint8_t UMP_M2CVM = 0x4;
constexpr uint8_t UMP_DATA = 0x5;
constexpr uint8_t UMP_FLEX_DATA = 0xD;
constexpr uint8_t UMP_MIDI_ENDPOINT = 0xF;

}

namespace M2Utils {
 inline void clear(uint8_t * const dest, uint8_t const c, std::size_t const n) {
  for (auto i = std::size_t{0}; i < n; i++) {
   dest[i] = c;
  }
 }

 inline uint32_t scaleUp(uint32_t srcVal, uint8_t srcBits, uint8_t dstBits){
  //Handle value of 0 - skip processing
  if(srcVal == 0){
   return 0L;
  }

  //handle 1-bit (bool) scaling
  if(srcBits == 1){
   return (1 << dstBits) - 1L;
  }

  // simple bit shift
  uint8_t scaleBits = (dstBits - srcBits);
  uint32_t bitShiftedValue = (srcVal + 0L) << scaleBits;
  uint32_t srcCenter = 1 << (srcBits-1);
  if (srcVal <= srcCenter ) {
   return bitShiftedValue;
  }

  // expanded bit repeat scheme
  uint8_t repeatBits = srcBits - 1;
  auto repeatMask = (1 << repeatBits) - 1;
  uint32_t repeatValue = srcVal & repeatMask;
  if (scaleBits > repeatBits) {
   repeatValue <<= scaleBits - repeatBits;
  } else {
   repeatValue >>= repeatBits - scaleBits;
  }

  while (repeatValue != 0) {
   bitShiftedValue |= repeatValue;
   repeatValue >>= repeatBits;
  }
  return bitShiftedValue;
 }

 inline uint32_t scaleDown(uint32_t srcVal, uint8_t srcBits, uint8_t dstBits){
  // simple bit shift
  uint8_t scaleBits = (srcBits - dstBits);
  return srcVal >> scaleBits;
 }

 inline void hirezRepresentation(char * outputString, uint32_t srcVal, uint8_t srcBits, uint8_t decimalPlaces) {
  if (srcVal==0) {
   sprintf(outputString,"MIN");
   return;
  }

  if (srcVal == (1UL << (srcBits-1))+0L) {
   sprintf(outputString,"MID");
   return;
  }

  uint32_t maxval = 0xFFFFFFFF;
  if (srcBits!=32) {
   maxval = (1<< (srcBits))-1;
  }

  if (srcVal==maxval) {
   sprintf(outputString,"MAX");
   return;
  }

  uint8_t fractionalBits = srcBits - 7;
  uint32_t fractionalValue = srcVal - (srcVal>> fractionalBits <<fractionalBits );
  float hiRezValue = (float)(srcVal >> fractionalBits) + ( (float)fractionalValue / (float)(1<<fractionalBits));

  sprintf(outputString, "%.*f", decimalPlaces, hiRezValue );

 }

}

#endif
