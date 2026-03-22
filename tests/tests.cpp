//
// Created by andrew on 3/05/24.
//

#include "../include/bytestreamToUMP.h"
#include "../include/umpToBytestream.h"
#include "../include/umpToMIDI1Protocol.h"
#include "../include/umpMessageCreate.h"
#include "../include/umpProcessor.h"
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

    //***** Flex Data (MT=0xD) *************
    printf("Flex Data MT=0xD Create \n");

    auto tempo = UMPMessage::mtDFlexTempo(0, 0, 0, 500000000);
    uint32_t inTempo[] = {tempo[0], tempo[1], tempo[2], tempo[3]};
    uint32_t outTempo[] = {0xd0000000, 0x1dcd6500, 0x00000000, 0x00000000};
    testRun_umpToump(" mtDFlexTempo 120bpm : ", inTempo, 4, outTempo);

    auto keysig = UMPMessage::mtDFlexKeySig(0, 0, 0, 2, 0);
    uint32_t inKeySig[] = {keysig[0], keysig[1], keysig[2], keysig[3]};
    uint32_t outKeySig[] = {0xd0000005, 0x02000000, 0x00000000, 0x00000000};
    testRun_umpToump(" mtDFlexKeySig D major (2 sharps) : ", inKeySig, 4, outKeySig);

    auto timesig = UMPMessage::mtDFlexTimeSig(0, 0, 0, 4, 2, 8);
    uint32_t inTimeSig[] = {timesig[0], timesig[1], timesig[2], timesig[3]};
    uint32_t outTimeSig[] = {0xd0000001, 0x04020800, 0x00000000, 0x00000000};
    testRun_umpToump(" mtDFlexTimeSig 4/4 : ", inTimeSig, 4, outTimeSig);

    auto chord = UMPMessage::mtDFlexChord(0, 0, 0, 0,0,1, 0,0,0,0, 0,0,0,0, 0,0,0, 0,0,0,0);
    uint32_t inChord[] = {chord[0], chord[1], chord[2], chord[3]};
    uint32_t outChord[] = {0xd0000006, 0x00010000, 0x00000000, 0x00000000};
    testRun_umpToump(" mtDFlexChord C major no bass : ", inChord, 4, outChord);

    // Flex Data with addrs=1 (Group) — verifies bits 21-20
    auto keysigGrp = UMPMessage::mtDFlexKeySig(0, 1, 0, 2, 0);
    uint32_t inKeySigGrp[] = {keysigGrp[0], keysigGrp[1], keysigGrp[2], keysigGrp[3]};
    uint32_t outKeySigGrp[] = {0xd0100005, 0x02000000, 0x00000000, 0x00000000};
    testRun_umpToump(" mtDFlexKeySig addrs=Group : ", inKeySigGrp, 4, outKeySigGrp);

    // Flex Data with form=1 (Start) — verifies bits 23-22
    uint8_t perfText[] = {'H', 'i'};
    auto perf = UMPMessage::mtDFlexPerformance(1, 1, 0, 5, 0, perfText, 2);
    uint32_t inPerf[] = {perf[0], perf[1], perf[2], perf[3]};
    uint32_t outPerf[] = {0xd1450100, 0x48690000, 0x00000000, 0x00000000};
    testRun_umpToump(" mtDFlexPerformance form=Start ch=5 : ", inPerf, 4, outPerf);

    // Flex Data with form=3 (End), addrs=1, ch=9 — exercises all bit fields
    uint8_t lyricText[] = {'!', '!'};
    auto lyric = UMPMessage::mtDFlexLyric(2, 3, 1, 9, 0, lyricText, 2);
    uint32_t inLyric[] = {lyric[0], lyric[1], lyric[2], lyric[3]};
    uint32_t outLyric[] = {0xd2d90200, 0x21210000, 0x00000000, 0x00000000};
    testRun_umpToump(" mtDFlexLyric form=End addrs=Group ch=9 : ", inLyric, 4, outLyric);

    //***** Flex Data Roundtrip (create -> processUMP -> callback) *************
    printf("Flex Data Roundtrip \n");

    umpProcessor proc;

    // Roundtrip: FlexTempo
    uint32_t rtTempoVal = 0;
    proc.setFlexTempo([&](struct umpFlexData mess, uint32_t num10nsPQN){
        rtTempoVal = num10nsPQN;
    });
    auto rtTempo = UMPMessage::mtDFlexTempo(0, 0, 0, 500000000);
    for(int i=0; i<4; i++) proc.processUMP(rtTempo[i]);
    passFail(rtTempoVal, 500000000);
    printf(" FlexTempo roundtrip\n");

    // Roundtrip: FlexKeySig
    uint8_t rtSharpFlats = 0, rtTonic = 0;
    proc.setFlexKeySig([&](struct umpFlexData mess, uint8_t sf, uint8_t t){
        rtSharpFlats = sf;
        rtTonic = t;
    });
    auto rtKeySig = UMPMessage::mtDFlexKeySig(0, 0, 0, 3, 5);
    for(int i=0; i<4; i++) proc.processUMP(rtKeySig[i]);
    passFail(rtSharpFlats, 3);
    passFail(rtTonic, 5);
    printf(" FlexKeySig roundtrip\n");

    // Roundtrip: FlexTimeSig
    uint8_t rtNum = 0, rtDenom = 0, rtN32 = 0;
    proc.setFlexTimeSig([&](struct umpFlexData mess, uint8_t n, uint8_t d, uint8_t n32){
        rtNum = n; rtDenom = d; rtN32 = n32;
    });
    auto rtTimeSig = UMPMessage::mtDFlexTimeSig(0, 0, 0, 6, 3, 16);
    for(int i=0; i<4; i++) proc.processUMP(rtTimeSig[i]);
    passFail(rtNum, 6);
    passFail(rtDenom, 3);
    passFail(rtN32, 16);
    printf(" FlexTimeSig roundtrip\n");

    // Roundtrip: FlexChord — exercises all 18 chord parameters + verifies baAlt2Deg fix
    uint8_t rtChShrpFlt=0, rtChTonic=0, rtChType=0;
    uint8_t rtChAlt1T=0, rtChAlt1D=0, rtChAlt2T=0, rtChAlt2D=0;
    uint8_t rtChAlt3T=0, rtChAlt3D=0, rtChAlt4T=0, rtChAlt4D=0;
    uint8_t rtBaShrpFlt=0, rtBaTonic=0, rtBaType=0;
    uint8_t rtBaAlt1T=0, rtBaAlt1D=0, rtBaAlt2T=0, rtBaAlt2D=0;
    proc.setFlexChord([&](struct umpFlexData mess,
        uint8_t a, uint8_t b, uint8_t c,
        uint8_t d, uint8_t e, uint8_t f, uint8_t g,
        uint8_t h, uint8_t i, uint8_t j, uint8_t k,
        uint8_t l, uint8_t m, uint8_t n,
        uint8_t o, uint8_t p, uint8_t q, uint8_t r){
        rtChShrpFlt=a; rtChTonic=b; rtChType=c;
        rtChAlt1T=d; rtChAlt1D=e; rtChAlt2T=f; rtChAlt2D=g;
        rtChAlt3T=h; rtChAlt3D=i; rtChAlt4T=j; rtChAlt4D=k;
        rtBaShrpFlt=l; rtBaTonic=m; rtBaType=n;
        rtBaAlt1T=o; rtBaAlt1D=p; rtBaAlt2T=q; rtBaAlt2D=r;
    });
    // Use distinct non-zero values for every parameter to catch any mix-up
    auto rtChord = UMPMessage::mtDFlexChord(0, 0, 0,
        2, 3, 0x15,       // chShrpFlt=2, chTonic=3(Eb), chType=0x15
        1, 2, 3, 4,       // alt1-2
        5, 6, 7, 8,       // alt3-4
        9, 10, 0x20,      // baShrpFlt=9, baTonic=10, baType=0x20
        11, 12, 13, 14);  // baAlt1-2
    for(int i=0; i<4; i++) proc.processUMP(rtChord[i]);
    passFail(rtChShrpFlt, 2); passFail(rtChTonic, 3); passFail(rtChType, 0x15);
    passFail(rtChAlt1T, 1); passFail(rtChAlt1D, 2);
    passFail(rtChAlt2T, 3); passFail(rtChAlt2D, 4);
    passFail(rtChAlt3T, 5); passFail(rtChAlt3D, 6);
    passFail(rtChAlt4T, 7); passFail(rtChAlt4D, 8);
    passFail(rtBaShrpFlt, 9); passFail(rtBaTonic, 10); passFail(rtBaType, 0x20);
    passFail(rtBaAlt1T, 11); passFail(rtBaAlt1D, 12);
    passFail(rtBaAlt2T, 13); passFail(rtBaAlt2D, 14);
    printf(" FlexChord roundtrip (18 params)\n");

    // Roundtrip: FlexPerformance with form verification
    uint8_t rtPerfForm = 0xFF;
    uint8_t rtPerfData[12] = {};
    uint8_t rtPerfLen = 0;
    proc.setFlexPerformance([&](struct umpFlexData mess, uint8_t* data, uint8_t len){
        rtPerfForm = mess.form;
        rtPerfLen = len;
        for(int i=0; i<len && i<12; i++) rtPerfData[i] = data[i];
    });
    uint8_t rtPerfText[] = {'T','e','s','t'};
    auto rtPerf = UMPMessage::mtDFlexPerformance(0, 0, 0, 0, 0, rtPerfText, 4);
    for(int i=0; i<4; i++) proc.processUMP(rtPerf[i]);
    passFail(rtPerfForm, 0);
    passFail(rtPerfLen, 4);
    passFail(rtPerfData[0], 'T');
    passFail(rtPerfData[1], 'e');
    passFail(rtPerfData[2], 's');
    passFail(rtPerfData[3], 't');
    printf(" FlexPerformance roundtrip\n");

    //***** Per-Note Expression *************
    printf("Per-Note Expression Create \n");

    auto pnpb = UMPMessage::mt4PerNotePitchBend(0, 0, 60, 0x80000000);
    uint32_t inPNPB[] = {pnpb[0], pnpb[1]};
    uint32_t outPNPB[] = {0x40603c00, 0x80000000};
    testRun_umpToump(" mt4PerNotePitchBend note=60 center : ", inPNPB, 2, outPNPB);

    auto pncc = UMPMessage::mt4PerNoteCC(0, 0, 60, 74, 0xFFFFFFFF);
    uint32_t inPNCC[] = {pncc[0], pncc[1]};
    uint32_t outPNCC[] = {0x40103c4a, 0xffffffff};
    testRun_umpToump(" mt4PerNoteCC note=60 CC74 max : ", inPNCC, 2, outPNCC);

    //***** Per-Note Roundtrip *************
    printf("Per-Note Roundtrip \n");

    uint8_t rtPNNote = 0;
    uint32_t rtPNValue = 0;
    uint8_t rtPNStatus = 0;
    proc.setCVM([&](struct umpCVM mess){
        rtPNNote = mess.note;
        rtPNValue = mess.value;
        rtPNStatus = mess.status;
    });

    auto rtPNPB = UMPMessage::mt4PerNotePitchBend(0, 0, 60, 0x80000000);
    for(int i=0; i<2; i++) proc.processUMP(rtPNPB[i]);
    passFail(rtPNNote, 60);
    passFail(rtPNValue, 0x80000000);
    passFail(rtPNStatus, PITCH_BEND_PERNOTE);
    printf(" PerNotePitchBend roundtrip\n");

    auto rtPNCC = UMPMessage::mt4PerNoteCC(0, 3, 48, 74, 0xABCD1234);
    for(int i=0; i<2; i++) proc.processUMP(rtPNCC[i]);
    passFail(rtPNNote, 48);
    passFail(rtPNValue, 0xABCD1234);
    printf(" PerNoteCC roundtrip\n");

    // mt4PerNoteRPN: group=1, ch=2, note=64, index=5 (RPN bank 0), value=0x12345678
    // word0: MT=4 group=1 status=0x00(RPN_PERNOTE) ch=2 note=64 index=5
    //        0x41024005
    auto pnrpn = UMPMessage::mt4PerNoteRPN(1, 2, 64, 5, 0x12345678);
    uint32_t inPNRPN[] = {pnrpn[0], pnrpn[1]};
    uint32_t outPNRPN[] = {0x41024005, 0x12345678};
    testRun_umpToump(" mt4PerNoteRPN note=64 idx=5 : ", inPNRPN, 2, outPNRPN);

    // mt4PerNoteManage: group=0, ch=0, note=60, optionFlags=3 (detach+reset)
    // word0: MT=4 group=0 status=0xF0(PERNOTE_MANAGE) ch=0 note=60 flags=3
    //        0x40F03c03
    auto pnman = UMPMessage::mt4PerNoteManage(0, 0, 60, 3);
    uint32_t inPNMan[] = {pnman[0], pnman[1]};
    uint32_t outPNMan[] = {0x40f03c03, 0x00000000};
    testRun_umpToump(" mt4PerNoteManage note=60 flags=3 : ", inPNMan, 2, outPNMan);

    //***** Flex Metronome Roundtrip *************
    printf("Flex Metronome Roundtrip \n");

    uint8_t rtMetClk = 0, rtMetAcc1 = 0, rtMetAcc2 = 0, rtMetAcc3 = 0;
    uint8_t rtMetSub1 = 0, rtMetSub2 = 0;
    proc.setFlexMetronome([&](struct umpFlexData mess, uint8_t numClkpPriCli,
                              uint8_t bAccP1, uint8_t bAccP2, uint8_t bAccP3,
                              uint8_t numSubDivCli1, uint8_t numSubDivCli2){
        (void) mess;
        rtMetClk  = numClkpPriCli;
        rtMetAcc1 = bAccP1;
        rtMetAcc2 = bAccP2;
        rtMetAcc3 = bAccP3;
        rtMetSub1 = numSubDivCli1;
        rtMetSub2 = numSubDivCli2;
    });

    auto rtMet = UMPMessage::mtDFlexMetronome(0, 0, 0, 24, 2, 1, 0, 4, 3);
    for(int i=0; i<4; i++) proc.processUMP(rtMet[i]);
    passFail(rtMetClk,  24);
    passFail(rtMetAcc1, 2);
    passFail(rtMetAcc2, 1);
    passFail(rtMetAcc3, 0);
    passFail(rtMetSub1, 4);
    passFail(rtMetSub2, 3);
    printf(" FlexMetronome roundtrip\n");

    ///****************************
    printf("Tests Passed: %d    Failed : %d\n",testPassed, testFailed);

}