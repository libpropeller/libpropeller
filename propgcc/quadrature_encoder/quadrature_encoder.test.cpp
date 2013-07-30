#include "unity.h"
#include "quadrature_encoder.h"
#include "c++-alloc.h"


const int basePin = 9;
quadrature_encoder * sut;


int data[10];


void setUp(void){
    sut = new quadrature_encoder();
    printf("Started in cog: %i", sut->Start(9, 1, 1, (int32_t)&data));
    waitcnt(CLKFREQ/10 + CNT);
}

void tearDown(void){
    sut->Stop();
    delete sut;
    sut = NULL;
}


void test_Empty(void){
    while(true){
        printf("\nPosition: %i", data[0]);
        waitcnt(CLKFREQ/10 + CNT);
    }
}