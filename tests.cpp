//
// Created by andrew on 3/05/24.
//

#include "include/bytestreamToUMP.h"
#include "include/umpToBytestream.h"
#include "include/umpToMIDI1Protocol.h"
#include "include/umpMessageCreate.h"
#include <cstdio>

bytestreamToUMP BS2UMP;
umpToBytestream UMP2BS;
umpToMIDI1Protocol UMP2M1;

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

void testRun_bsToUmp(const char* heading, uint8_t *bytes, int btyelength, uint32_t * testCheck)
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
    printf("\n");
}

void testRun_umpToBs(const char* heading, uint8_t *testBytes, uint32_t * umps, int umplength)
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
    testRun_bsToUmp(" Test 1 Note On w/running status: ", bytes1, 5, tests1);

    uint8_t bytes2[] = {0xF8};
    uint32_t tests2[] = {0x10f80000};
    testRun_bsToUmp(" Test 2 System Message 1 byte: ", bytes2, 1, tests2);

    uint8_t bytes3[] = {0xC6, 0x40};
    uint32_t tests3[] = {0x20c64000};
    testRun_bsToUmp(" Test 3 PC 2 bytes : ", bytes3, 2, tests3);

    uint8_t bytes4[] = {0xF0, 0x7E, 0x7F, 0x0D, 0x70, 0x02, 0x4B, 0x60, 0x7A, 0x73, 0x7F, 0x7F, 0x7F, 0x7F, 0x7D,
        0x00 , 0x00, 0x00 , 0x00, 0x01 , 0x00, 0x00 , 0x00 , 0x03 , 0x00, 0x00, 0x00 , 0x10 , 0x00 , 0x00, 0x00, 0xF7};
    uint32_t tests4[] = {
        0x30167e7f, 0x0d70024b,
        0x3026607a, 0x737f7f7f,
        0x30267f7d, 0x00000000,
        0x30260100,0x00000300,
        0x30360000,0x10000000
    };
    testRun_bsToUmp(" Test 4 Sysex : ", bytes4, 32, tests4);


    //******** UMP ByteSteam  ***************
    printf("UMP to ByteSteam \n");
    uint8_t bytes5[] = {0x81, 0x60, 0x50, 0x81, 0x70, 0x70};
    uint32_t tests5[] = {0x20816050, 0x20817070};
    testRun_umpToBs(" Test 5 Note On: ", bytes5, tests5, 2);
    testRun_umpToBs(" Test 6 System Message 1 byte: ", bytes2,  tests2, 1);
    testRun_umpToBs(" Test 7 PC 2 bytes : ", bytes3,  tests3, 1);
    testRun_umpToBs(" Test 8 Sysex : ", bytes4,  tests4, 10);

    //***** UMP2M1 *************
    printf("UMP to MIDI 1 Protocol \n");
    uint32_t in[] = {0x20816050, 0x20817070};
    testRun_umpToM1(" Test MIDI 1 : ", in,  2, in, 2);

    testRun_umpToM1(" Test SysEx : ", tests4,  10, tests4, 10);
    testRun_umpToM1(" Test System Msg : ", tests2,  1, tests2, 1);

    uint32_t in2[] = {0x40904000, 0xc1040000};
    uint32_t out2[] = {0x20904060};
    testRun_umpToM1(" Test MT4 : ", in2,  2, out2, 1);

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