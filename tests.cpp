//
// Created by andrew on 3/05/24.
//

#include "include/bytestreamToUMP.h"
#include "include/umpToBytestream.h"
#include "include/umpToMIDI1Protocol.h"
#include "include/umpMessageCreate.h"
#include <cstdio>

#include "umpToMIDI2Protocol.h"

bytestreamToUMP BS2UMP;
umpToBytestream UMP2BS;
umpToMIDI1Protocol UMP2M1;
umpToMIDI2Protocol UMP2M2;

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
        BS2UMP.bytestreamParse(bytes[i]);
        while(BS2UMP.availableUMP()){
            uint32_t ump = BS2UMP.readUMP();
            //ump contains a ump 32 bit value. UMP messages that have 64bit will produce 2 UMP words
            passFail (ump, testCheck[testCounter++]);

        }
    }
    printf(" length :");passFail (outlength, testCounter);
    printf("\n");
}

void testRun_umpToBs(const char* heading, uint8_t *testBytes, uint32_t * umps, int umplength, int outlength)
{
    va_list args;
    vprintf (heading, args);

    int testCounter = 0;

    for(int i=0; i<umplength; i++){
        UMP2BS.UMPStreamParse(umps[i]);
        while(UMP2BS.availableBS()){
            uint8_t byte = UMP2BS.readBS();
            //ump contains a ump 32 bit value. UMP messages that have 64bit will produce 2 UMP words
            passFail (byte, testBytes[testCounter++]);

        }
    }
    printf(" length :");passFail (outlength, testCounter);
    printf("\n");
}

void testRun_umpToM1(const char* heading, uint32_t * in, int inlength, uint32_t * out, int outlength)
{
    va_list args;
    vprintf (heading, args);

    int testCounter = 0;


    for(int i=0; i<inlength; i++){
        UMP2M1.UMPStreamParse(in[i]);
        while(UMP2M1.availableUMP()){
            uint32_t newUmp = UMP2M1.readUMP();
            //ump contains a ump 32 bit value. UMP messages that have 64bit will produce 2 UMP words
            passFail (newUmp, out[testCounter++]);
        }
    }
    printf(" length :");passFail (outlength, testCounter);
    printf("\n");
}

void testRun_umpToM2(const char* heading, uint32_t * in, int inlength, uint32_t * out, int outlength)
{
    va_list args;
    vprintf (heading, args);

    int testCounter = 0;


    for(int i=0; i<inlength; i++){
        UMP2M2.UMPStreamParse(in[i]);
        while(UMP2M2.availableUMP()){
            uint32_t newUmp = UMP2M2.readUMP();
            //ump contains a ump 32 bit value. UMP messages that have 64bit will produce 2 UMP words
            passFail (newUmp, out[testCounter++]);
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
    uint8_t bytes1[] = {0x81, 0x60, 0x50, 0x70, 0x70};
    uint32_t tests1[] = {0x20816050, 0x20817070};
    testRun_bsToUmp(" Test 1 Note On w/running status: ", bytes1, 5, tests1,2);

    uint8_t bytes2[] = {0xF8};
    uint32_t tests2[] = {0x10f80000};
    testRun_bsToUmp(" Test 2 System Message 1 byte: ", bytes2, 1, tests2,1);

    uint8_t bytes3[] = {0xC6, 0x40};
    uint32_t tests3[] = {0x20c64000};
    testRun_bsToUmp(" Test 3 PC 2 bytes : ", bytes3, 2, tests3,1);

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

    //Let's Send bad UMP Data
    uint32_t tests5_bad[] = {0x10F47F7F};
    uint8_t bytes5_bad[] = {0xF4, 0x7F, 0x7F,0x00,0x00};
    testRun_bsToUmp(" Test 9 Bad System Message : ", bytes5_bad, 5, tests5_bad,0);
    testRun_bsToUmp(" Test 10 Retest MT2 Note On w/running status: ", bytes1, 5, tests1,2);

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

    //******** UMP ByteSteam  ***************
    printf("UMP to ByteSteam \n");
    uint8_t bytes5[] = {0x81, 0x60, 0x50, 0x81, 0x70, 0x70};
    uint32_t tests5[] = {0x20816050, 0x20817070};
    testRun_umpToBs(" Test 5 Note On: ", bytes5, tests5, 2,6);
    testRun_umpToBs(" Test 6 System Message 1 byte: ", bytes2,  tests2, 1,1);
    testRun_umpToBs(" Test 7 PC 2 bytes : ", bytes3,  tests3, 1,2);
    testRun_umpToBs(" Test 8 Sysex : ", bytes4,  tests4, 10,32);


    //UMP2BS.debug = true;
    testRun_umpToBs(" Test 9 Bad Data : ", bytes5_bad,  tests5_bad, 1,0);
    testRun_umpToBs(" ReTest 5 Note On: ", bytes5, tests5, 2,6);

    //***** UMP2M1 *************
    printf("UMP to MIDI 1 Protocol \n");
    uint32_t in[] = {0x20816050, 0x20817070};
    testRun_umpToM1(" Test MIDI 1 : ", in,  2, in, 2);

    testRun_umpToM1(" Test SysEx : ", tests4,  10, tests4, 10);
    testRun_umpToM1(" Test System Msg : ", tests2,  1, tests2, 1);

    uint32_t in2[] = {0x40904000, 0xc1040000};
    uint32_t out2[] = {0x20904060};
    testRun_umpToM1(" Test MT4 : ", in2,  2, out2, 1);

    // ******** UMP2M2 ***********
    printf("UMP to MIDI 2 Protocol \n");
    testRun_umpToM2(" Test MIDI 2 : ", in2,  2, in2, 2);
    testRun_umpToM2(" Test MT2 Note On : ", out2,  1, in2, 2);

    uint32_t inMt2_PC1[] = {0x20B00001,0x20B0200A,0x20C04000};
    uint32_t outMt4_PC1[] = {0x40c00001,0x4000010A};
    testRun_umpToM2(" Test 6 MT 2 PC With Bank : ", inMt2_PC1, 3, outMt4_PC1,2);

    uint32_t inMt2_PC2[] = {0x20C64100};
    uint32_t outMt4_PC2[] = {0x40c60000,0x41000000};
    testRun_umpToM2(" Test 6 MT 2 PC No Bank : ", inMt2_PC2, 1, outMt4_PC2,2);

    uint32_t inMt2_RPN1[] = {0x20B66500, 0x20B66406, 0x20B60608};
    uint32_t outMt4_RPN1[] = {0x40260006,0x10000000};
    testRun_umpToM2(" Test 6 MT 2 RPN 0x6 no 38 : ", inMt2_RPN1, 3, outMt4_RPN1,2);

    uint32_t inMt2_RPN2[] = {0x20B66500, 0x20B66406, 0x20B60608, 0x20B62600};
    testRun_umpToM2(" Test 6 MT 2 RPN 0x6 with 38 : ", inMt2_RPN2, 4, outMt4_RPN1,2);

    uint32_t inMt2_RPN3[] = {0x20B66520, 0x20B66406, 0x20B60608, 0x20B60609, 0x20B607EE};
    uint32_t outMt2_RPN3[] = {0x40262006,0x10000000,0x40262006,0x12000000,0x40b60700,0xdd75d75d};
    testRun_umpToM2(" Test 6 MT 2 RPN 0x2006 no 38 twice followed by a Volume CC : ",
        inMt2_RPN3, 5, outMt2_RPN3,6);

    uint32_t inMt2_RPN4[] = {0x20B66520, 0x20B66406, 0x20B60608, 0x20B62601, 0x20B60609 , 0x20B62602};
    uint32_t outMt2_RPN4[] = {0x40262006,0x10040000,0x40262006,0x12080000};
    testRun_umpToM2(" Test 6 MT 2 RPN 0x2006 with 38 twice  : ",
        inMt2_RPN4, 6, outMt2_RPN4,4);

    //***** UMP Meesage Create *************
    printf("UMP Message Create \n");
    uint32_t inUmp1[] = {UMPMessage::mt0NOOP()};
    uint32_t outUmp1[] = {0x00000000};
    testRun_umpToump(" UMP NOOP : ", inUmp1,  1, outUmp1);

    uint32_t inUmp2[] = {UMPMessage::mt1TimingClock(8)};
    uint32_t outUmp2[] = {0x18f80000};
    testRun_umpToump(" UMP Timing Clock : ", inUmp2,  1, outUmp2);

    ///****************************
    printf("Tests Passed: %d    Failed : %d\n",testPassed, testFailed);

}