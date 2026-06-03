//
// Created by andrew on 3/05/24.
//

#include "../include/bytestreamToUMP.h"
#include <cstdio>

#include "umpToMIDI2Protocol.h"

bytestreamToUMP BS2UMP;


int testPassed = 0;
int testFailed = 0;

void passFail (uint32_t v1, uint32_t v2)
{
    if(v1 == v2)
    {
        printf(".");
        testPassed++;
    }else
    {
        printf(" fail %#08x != %#08x ", v1, v2);
        testFailed++;
    }
}

void testRun_bsToUmp(const char* heading, uint8_t *bytes, int btyelength, uint32_t * testCheck, int outlength)
{
    va_list args;
    vprintf (heading, args);

    int testCounter = 0;

    for(int i=0; i<btyelength; i++){
      if(bytes[i] == 0xFF){
        BS2UMP.dumpSysex7State(false);
        }else{
        BS2UMP.bytestreamParse(bytes[i]);
        }
        while(BS2UMP.availableUMP()){
            uint32_t ump = BS2UMP.readUMP();
            //ump contains a ump 32 bit value. UMP messages that have 64bit will produce 2 UMP words
            passFail (ump, testCheck[testCounter++]);

        }
    }
    printf(" length :");passFail (outlength, testCounter);
    printf("\n");
}





void testRun_umpToump(const char* heading, uint32_t * in, int inlength, uint32_t * out)
{
    va_list args;
    vprintf (heading, args);
    for(int i=0; i<inlength; i++){
        passFail (in[i], out[i]);
    }
    printf("\n");
}

int main(){
    printf("Starting Tests...\n");

    //******** ByteSteam to UMP ***************
    printf("ByteSteam to UMP \n");
    uint8_t bytes1[] = {0xf0, 0x11 ,0x22 ,0x33, 0xf0, 0x44, 0x55, 0xf7};
    uint32_t tests1[] = {0x30131122, 0x33000000, 0x30024455, 0x00000000};

    testRun_bsToUmp(" Bad Sysex 1: ", bytes1, 8, tests1,4);

    printf("ByteSteam to UMP \n");
    uint8_t bytes2[] = {0xf0, 0x10 ,0x11 ,0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0xFF/*this forces a dump*/, 0x18, 0x19, 0x1A, 0xf7};
    uint32_t tests2[] = {0x30161011, 0x12131415, 0x30221617, 0x00000000,0x30331819, 0x1A000000 };

    testRun_bsToUmp(" Sysex w/Dump: ", bytes2, 14, tests2,6);

     uint8_t bytes4[] = {0xF0, 0x7E, 0x7F, 0x0D, 0x70, 0x02, 0x4B, 0x60, 0x7A, 0x73, 0x7F, 0x7F, 0x7F, 0x7F, 0x7D,
        0x00 , 0x00, 0x00 , 0x00, 0x01 , 0x00, 0x00 , 0x00 , 0x03 , 0x00, 0x00, 0x00 , 0x10 , 0x00 , 0x00, 0x00, 0xF7};
    uint32_t tests4[] = {
        0x30167e7f, 0x0d70024b,
        0x3026607a, 0x737f7f7f,
        0x30267f7d, 0x00000000,
        0x30260100,0x00000300,
        0x30360000,0x10000000
    };
    testRun_bsToUmp(" Test 4 Sysex : ", bytes4, 32, tests4,10);



    uint8_t bytesSyesex[] = {
            0xf0, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e,0x1d, 0x1f, 0xf7,
            0xf0, 0x2a, 0x2b, 0x2c, 0x2d, 0x2f, 0x3a, 0x3b,
            0xf8,
            0x3c, 0x3d, 0x3e, 0x3f, 0xf7,
            0xf0, 0x4a, 0x4b, 0x4c, 0x4d, 0x4f, 0xf7,

            0xf0, 0x5a, 0x5b,
            0xf8,
            0x5c, 0x5d, 0xf7,
            0xf0, 0x6a, 0x6b, 0x6c, 0xf7,
            0xf0, 0x7a, 0x7b, 0xf7,
            0xf8,
            0xf0, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
            0xf8,
            0x1a, 0x1b, 0x1c, 0x1d,
            0xf8,
            0x1e,0x1d, 0x1f, 0xf7

    };

    uint32_t testsSysex2[] = {
            0x30160a0b, 0x0c0d0e0f,
            0x30261a1b, 0x1c1d1e1d,
            0x30311f00, 0x000000,
            0x30162a2b, 0x2c2d2f3a,
            0x10f80000,
            0x30353b3c, 0x3d3e3f00,
            0x30054a4b, 0x4c4d4f00,
            0x10f80000,
            0x30045a5b, 0x5c5d0000,
            0x30036a6b, 0x6c000000,
            0x30027a7b, 0x00000000,
            0x10f80000,
            0x10f80000, //This is slightly out of order because otherwise of the way the parser handles end of sysex
            0x30160a0b, 0x0c0d0e0f,
            0x10f80000,
            0x30261a1b, 0x1c1d1e1d,
            0x30311f00, 0x000000
    };

    testRun_bsToUmp(" Test 11 sysex 2 w/Timing Clock : ", bytesSyesex, 70, testsSysex2,29);



    ///****************************
    printf("Tests Passed: %d    Failed : %d\n",testPassed, testFailed);

}