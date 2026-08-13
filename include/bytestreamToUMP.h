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

#ifndef BSUMP_H
#define BSUMP_H
#include <cstdint>

#define BSTOUMP_BUFFER 4

#include "utils.h"

class bytestreamToUMP{

	private:
		uint8_t d0=0;
		uint8_t d1=255;
		
		uint8_t sysex7State = 0;
		uint8_t sysex7Pos = 0;
		
		uint8_t sysex[6] = {0,0,0,0,0,0};
	    uint32_t umpMess[BSTOUMP_BUFFER] = {0,0,0,0};
	    
	    //Channel Based Data
		uint8_t bankMSB[16];
		uint8_t bankLSB[16];
		bool rpnMode[16];
		uint8_t rpnMsbValue[16];
		uint8_t rpnMsb[16];
		uint8_t rpnLsb[16];
		int readIndex = 0;
		int writeIndex = 0;
		int bufferLength = 0;
	    	
		void bsToUMP(uint8_t b0, uint8_t b1, uint8_t b2){
		  uint8_t status = b0 & 0xF0;

		   if(b0 >= MIDI1_MSGS::TIMING_CODE){
			  umpMess[writeIndex] = ((MIDI1_MSGS::UMP_SYSTEM << 4) + defaultGroup + 0L) << 24;
			  umpMess[writeIndex] +=  (b0 + 0L) << 16;
			  umpMess[writeIndex] +=  b1  << 8;
			  umpMess[writeIndex] +=  b2;
		   	  increaseWrite();
		   }else if(status>=MIDI1_MSGS::NOTE_OFF && status<=MIDI1_MSGS::PITCH_BEND){
			  umpMess[writeIndex] = ((MIDI1_MSGS::UMP_M1CVM << 4) + defaultGroup + 0L) << 24;
			  umpMess[writeIndex] +=  (b0 + 0L) << 16;
			  umpMess[writeIndex] +=  b1  << 8;
			  umpMess[writeIndex] +=  b2;
		   	  increaseWrite();
		  }
		}

		void increaseWrite(){
			bufferLength++;
			writeIndex++;
			if (writeIndex == BSTOUMP_BUFFER) {
				writeIndex = 0;
			}
		}

	public:
		uint8_t defaultGroup = 0;
		bool enableRunningStatus = true;
		
		bytestreamToUMP(){
			clearAll();
		}
		
		bool availableUMP(){
			return bufferLength;
		}

        void clearAll(){
            using M2Utils::clear;
            clear(bankMSB, 255, sizeof(bankMSB));
            clear(bankLSB, 255, sizeof(bankLSB));
            clear(rpnMsbValue, 255, sizeof(rpnMsbValue));
            clear(rpnMsb, 255, sizeof(rpnMsb));
            clear(rpnLsb, 255, sizeof(rpnLsb));
			d0=0;
			d1=255;
        }

        void resetBuffer(){
            d1 = 255;
            for (int i = 0; i < BSTOUMP_BUFFER; i++) {
                umpMess[i] = 0;
            }
            readIndex = 0;
            writeIndex = 0;
            bufferLength = 0;
        }
		
		uint32_t readUMP(){
			uint32_t mess = umpMess[readIndex];
			bufferLength--;	 //	Decrease buffer size after reading
			readIndex++;
			if (readIndex == BSTOUMP_BUFFER) {
				readIndex = 0;
			}
			return mess;
		}

		void dumpSysex7State(bool reset) {
			if (sysex7State > 0 && sysex7Pos > 0) {
				//Then dump current bytes
				umpMess[writeIndex] = ((MIDI1_MSGS::UMP_SYSEX7 << 4) + defaultGroup + 0L) << 24;
				umpMess[writeIndex] +=  (sysex7State + 0L) << 20;
				umpMess[writeIndex] +=  ((sysex7Pos + 0L) << 16);
				umpMess[writeIndex] += (sysex[0] << 8) + sysex[1];
				increaseWrite();
				umpMess[writeIndex] = ((sysex[2] + 0L) << 24) + ((sysex[3] + 0L)<< 16) + (sysex[4] << 8) + sysex[5] + 0L;
				increaseWrite();
				M2Utils::clear(sysex, 0, sizeof(sysex));
				if (sysex7State==1)sysex7State=2;
			}

			if (reset)sysex7State = 1;
			sysex7Pos = 0;
		}
		
		void bytestreamParse(uint8_t midi1Byte){
			if (midi1Byte == MIDI1_MSGS::TUNEREQUEST
                || midi1Byte ==  MIDI1_MSGS::TIMINGCLOCK
                || midi1Byte ==  MIDI1_MSGS::SEQSTART
                || midi1Byte ==  MIDI1_MSGS::SEQCONT
                || midi1Byte ==  MIDI1_MSGS::SEQSTOP
                || midi1Byte ==  MIDI1_MSGS::ACTIVESENSE
                || midi1Byte ==  MIDI1_MSGS::SYSTEMRESET
                ) {
				bsToUMP(midi1Byte,0,0);
				return;
			}

			if (midi1Byte & MIDI1_MSGS::NOTE_OFF) { // Status byte received
				if (sysex7State>=1 && midi1Byte != SYSEX_STOP){
					dumpSysex7State(true);
					sysex7State = 0;
				}
				d0 = midi1Byte;
				d1 = 255;
				if (midi1Byte == MIDI1_MSGS::SYSEX_START){
					dumpSysex7State(true);
				}
                else if (midi1Byte == MIDI1_MSGS::SYSEX_STOP){
                	if (sysex7State == 0) {
                		//This is a bad Sysex End Byte - received before a 0xF0
                		return;
                	}
                    umpMess[writeIndex] = ((UMP_SYSEX7 << 4) + defaultGroup + 0L) << 24;
                    umpMess[writeIndex] +=  ((sysex7State == 1?0:3) + 0L) << 20;
                    umpMess[writeIndex] +=  ((sysex7Pos + 0L) << 16) ;
                    umpMess[writeIndex] += (sysex[0] << 8) + sysex[1];
                    increaseWrite();
                    umpMess[writeIndex] = ((sysex[2] + 0L) << 24) + ((sysex[3] + 0L)<< 16) + (sysex[4] << 8) + sysex[5];
                    increaseWrite();
                    sysex7State = 0;
                	sysex7Pos = 0;
                    M2Utils::clear(sysex, 0, sizeof(sysex));
                }
			} else if(sysex7State >= 1){
				if(sysex7Pos%6 == 0 && sysex7Pos !=0){
                    umpMess[writeIndex] = ((MIDI1_MSGS::UMP_SYSEX7 << 4) + defaultGroup + 0L) << 24;
					umpMess[writeIndex] +=  (sysex7State + 0L) << 20;
					umpMess[writeIndex] +=  6L << 16;
					umpMess[writeIndex] += (sysex[0] << 8) + sysex[1];
					increaseWrite();
					umpMess[writeIndex] = ((sysex[2] + 0L) << 24) + ((sysex[3] + 0L)<< 16) + (sysex[4] << 8) + sysex[5] + 0L;
					increaseWrite();
					M2Utils::clear(sysex, 0, sizeof(sysex));
					sysex7State=2;
					sysex7Pos=0;
				}
                sysex[sysex7Pos++] = midi1Byte;
			}
            else if (d1 != 255) { // Second byte
                bsToUMP(d0, d1, midi1Byte);
                d1 = 255;
            	if (!(enableRunningStatus && d0 < MIDI1_MSGS::SYSEX_START)){
            		d0 = 0;
            	}
            }
            else if (d0){ // status byte set
                if (
                        (d0 & 0xF0) == MIDI1_MSGS::PROGRAM_CHANGE
                        || (d0 & 0xF0) == MIDI1_MSGS::CHANNEL_PRESSURE
                        || d0 == MIDI1_MSGS::TIMING_CODE
                        || d0 == MIDI1_MSGS::SONG_SELECT
                        ) {
                    bsToUMP(d0, midi1Byte, 0);
                	if (!(enableRunningStatus && d0 < MIDI1_MSGS::SYSEX_START)){
                		d0 = 0;
                	}
                } else if (d0 == 0xF4 || d0 == 0xF5 || d0 == 0xFD || d0==0xF9) {
                    resetBuffer();
                } else if (d0 < MIDI1_MSGS::SYSEX_START || d0 == MIDI1_MSGS::SPP) { // First data byte
                    d1=midi1Byte;
                }
            }
		}
};

#endif
