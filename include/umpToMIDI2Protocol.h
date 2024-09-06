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

#ifndef UMP2MIDI2_H
#define UMP2MIDI2_H


#include <cstdint>
#include "utils.h"

#define UMPTOPROTO1_BUFFER 4

struct ccToRPN{
    ccToRPN() : type(0), MSB(255), LSB(255),  valueMSB(255) {}
    uint8_t type;
    uint8_t MSB;
    uint8_t LSB;
    uint8_t valueMSB;
};

struct ccToPC{
    ccToPC() : valueMSB(255), valueLSB(255) {}
    uint8_t valueMSB;
    uint8_t valueLSB;
};


class umpToMIDI2Protocol
{
private:
    uint8_t mType = 0;
    uint32_t ump64word1 = 0;
    uint8_t UMPPos = 0;
    uint32_t umpMess[UMPTOPROTO1_BUFFER] = {0, 0, 0, 0};

    ccToRPN cTR[256];
    ccToPC cTP[256];

    void checkRPNOnChannel(uint8_t grChannel)
    {
        if (cTR[grChannel].valueMSB < 128 && cTR[grChannel].MSB < 128 && cTR[grChannel].LSB < 128)
        {
            //So sometimes CC 38 is optional grrrr... handle this
            uint8_t group = grChannel >> 4;
            uint8_t channel = grChannel & 0xF;
            uint32_t out1 = ((0x04 << 4) + group) << 24;
            out1 += cTR[grChannel].type == RPN ? 0b0010 << 20 : 0b0011 << 20;
            out1 += channel << 16;
            out1 += (cTR[grChannel].MSB << 8) + cTR[grChannel].LSB;
            uint32_t val = (cTR[grChannel].valueMSB << 7);
            uint32_t out2 = M2Utils::scaleUp(val, 14, 32);
            cTR[grChannel].valueMSB = 255;
            umpMess[writeIndex] = out1;
            increaseWrite();
            umpMess[writeIndex] = out2;
            increaseWrite();

        }
    }

    void increaseWrite()
    {
        bufferLength++;
        writeIndex++;
        if (writeIndex == UMPTOPROTO1_BUFFER)
        {
            writeIndex = 0;
        }
    }

    int readIndex = 0;
    int writeIndex = 0;
    int bufferLength = 0;

public:
    bool availableUMP()
    {
        return bufferLength;
    }

    uint32_t readUMP()
    {
        uint32_t mess = umpMess[readIndex];
        bufferLength--; //	Decrease buffer size after reading
        readIndex++;
        if (readIndex == UMPTOPROTO1_BUFFER)
        {
            readIndex = 0;
        }

        return mess;
    }

    void UMPStreamParse(uint32_t UMP)
    {
        switch (UMPPos)
        {
        case 0:
            {
                //First UMP Packet
                //First part of a UMP Message
                mType = UMP >> 28;
                switch (mType)
                {
                case UMP_UTILITY: //32 bits Utility Messages
                case 0x6: //32 Reserved
                case 0x7: //32 Reserved
                case UMP_SYSTEM:
                    {
                        //32 bits System Real Time and System Common Messages (except System Exclusive)
                        umpMess[writeIndex] = UMP;
                        increaseWrite();
                        break;
                    }
                case UMP_M1CVM:
                    {
                        //32 Bits MIDI 1.0 Channel Voice Messages
                        //Do Convert here!
                        uint8_t group = (UMP >> 24) & 0xF;
                        uint8_t status = (UMP >> 16) & 0xF0;
                        uint8_t channel = (UMP >> 16) & 0xF;
                        uint8_t val1 = (UMP >> 8) & 0x7F;
                        uint8_t val2 = UMP & 0x7F;
                        uint32_t out1 = 0;
                        uint32_t out2 = 0;
                        //convert note on zero velocity to noteoff
                        if (status == 0x90 && val2 == 0)
                        {
                            status = 0x80;
                            val2 = 0x40;
                        }
                        uint8_t grChannel = (group << 4) + channel;
                        switch (status)
                        {
                        case 0x80: //note off
                        case 0x90: //note on
                            out1 = ((0x04 << 4) + group) << 24;
                            out1 += (status + channel) << 16;
                            out1 += val1 << 8;
                            out2 += (M2Utils::scaleUp(val2, 7, 16) << 16);
                            umpMess[writeIndex] = out1;
                            increaseWrite();
                            umpMess[writeIndex] = out2;
                            increaseWrite();
                            break;
                        case 0xA0: //poly aftertouch
                            out1 = ((0x04 << 4) + group) << 24;
                            out1 += (status + channel) << 16;
                            out1 += val1 << 8;
                            out2 += M2Utils::scaleUp(val2, 7, 32);
                            umpMess[writeIndex] = out1;
                            increaseWrite();
                            umpMess[writeIndex] = out2;
                            increaseWrite();
                            break;
                        case 0xB0: //cc
                            if(val1 !=38)
                            {
                                checkRPNOnChannel(grChannel);
                            }
                            switch (val1)
                            {
                                case 0:
                                    cTP[grChannel].valueMSB = val2;
                                    break;
                                case 32:
                                    cTP[grChannel].valueLSB = val2;
                                    break;
                                case 101:
                                    cTR[grChannel].type = RPN;
                                    cTR[grChannel].valueMSB = 255;
                                    cTR[grChannel].MSB = val2;
                                    break;
                                case 100:
                                    cTR[grChannel].type = RPN;
                                    cTR[grChannel].valueMSB = 255;
                                    cTR[grChannel].LSB = val2;
                                    break;
                                case 99:
                                    cTR[grChannel].type = NRPN;
                                    cTR[grChannel].valueMSB = 255;
                                    cTR[grChannel].MSB = val2;
                                    break;
                                case 98:
                                    cTR[grChannel].type = NRPN;
                                    cTR[grChannel].valueMSB = 255;
                                    cTR[grChannel].LSB = val2;
                                    break;
                                case 6:
                                    cTR[grChannel].valueMSB = val2;
                                    if(cTR[grChannel].MSB == 0x00 &&
                                        cTR[grChannel].LSB >= 0x02 && cTR[grChannel].LSB <= 0x06
                                        )
                                    { //Force an RPN Out
                                        checkRPNOnChannel(grChannel);
                                    }
                                    break;
                                case 38:
                                    if(cTR[grChannel].valueMSB != 255)
                                    {
                                        out1 = ((0x04 << 4) + group) << 24;
                                        out1 += cTR[grChannel].type == RPN ? 0b0010 << 20 : 0b0011 << 20;
                                        out1 += channel << 16;
                                        out1 += (cTR[grChannel].MSB << 8) + cTR[grChannel].LSB;
                                        uint32_t val = (cTR[grChannel].valueMSB << 7) + val2;
                                        out2 = M2Utils::scaleUp(val, 14, 32);
                                        cTR[grChannel].valueMSB = 255;
                                        umpMess[writeIndex] = out1;
                                        increaseWrite();
                                        umpMess[writeIndex] = out2;
                                        increaseWrite();
                                    }
                                    break;
                                default:
                                    out1 = ((0x04 << 4) + group) << 24;
                                    out1 += (status + channel) << 16;
                                    out1 += val1 << 8;
                                    out2 += M2Utils::scaleUp(val2, 7, 32);
                                    umpMess[writeIndex] = out1;
                                    increaseWrite();
                                    umpMess[writeIndex] = out2;
                                    increaseWrite();
                                    break;
                            }

                            break;
                        case 0xC0: //Program change
                            out1 = ((0x04 << 4) + group) << 24;
                            out1 += (status + channel) << 16;
                            if (cTP[grChannel].valueMSB != 255 && cTP[grChannel].valueLSB != 255)
                            {
                                out1 += 1;
                                out2 += (cTP[grChannel].valueMSB << 8) + cTP[grChannel].valueLSB;
                                cTP[grChannel].valueMSB = 255;
                                cTP[grChannel].valueLSB = 255;
                            }
                            out2 += val1 << 24;
                            umpMess[writeIndex] = out1;
                            increaseWrite();
                            umpMess[writeIndex] = out2;
                            increaseWrite();
                            break;
                        case 0xD0: //Channel Pressure
                            out1 = ((0x04 << 4) + group) << 24;
                            out1 += (status + channel) << 16;
                            out2 += M2Utils::scaleUp(val1, 7, 32);
                            umpMess[writeIndex] = out1;
                            increaseWrite();
                            umpMess[writeIndex] = out2;
                            increaseWrite();
                            break;
                        case 0xE0: //Pitch bend
                            out1 = ((0x04 << 4) + group) << 24;
                            out1 += (status + channel) << 16;
                            uint8_t pb = (val2 << 7) + val1;
                            out2 += M2Utils::scaleUp(pb, 14, 32);
                            umpMess[writeIndex] = out1;
                            increaseWrite();
                            umpMess[writeIndex] = out2;
                            increaseWrite();
                            break;

                        }
                        break;
                    }
                default:
                    umpMess[writeIndex] = UMP;
                    increaseWrite();
                    UMPPos++;
                    break;
                }
                break;
            }
        case 1:
            {
                //64Bit+ Messages only
                switch (mType)
                {
                case 0x8: //64 Reserved
                case 0x9: //64 Reserved
                case 0xA: //64 Reserved
                case UMP_M2CVM:
                case UMP_SYSEX7: //64 bits Data Messages (including System Exclusive) part 2
                    umpMess[writeIndex] = UMP;
                    increaseWrite();
                    UMPPos = 0;
                    break;
                default:
                    umpMess[writeIndex] = UMP;
                    increaseWrite();
                    UMPPos++;
                    break;
                }
                break;
            }
        case 2:
            {
                switch (mType)
                {
                case 0xB: //96 Reserved
                case 0xC: //96 Reserved
                    umpMess[writeIndex] = UMP;
                    increaseWrite();
                    UMPPos = 0;
                    break;
                default:
                    umpMess[writeIndex] = UMP;
                    increaseWrite();
                    UMPPos++;
                    break;
                }
                break;
            }
        case 3:
            {
                umpMess[writeIndex] = UMP;
                increaseWrite();
                UMPPos = 0;
                break;
            }

        }
    }
};

#endif
