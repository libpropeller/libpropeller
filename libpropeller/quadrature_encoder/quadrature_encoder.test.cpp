#include "unity.h"
#include "quadrature_encoder.h"
#include "c++-alloc.h"


const int basePin = 9;
QuadratureEncoder * sut;




void setUp(void){
    sut = new QuadratureEncoder();
    printf("Started in cog: %i", sut->Start(9, 2));
    waitcnt(CLKFREQ/10 + CNT);
}

void tearDown(void){
    sut->Stop();
    delete sut;
    sut = NULL;
}


void test_Empty(void){
    while(true){
        printf("\nPosition: %i", sut->GetReading(1));
        waitcnt(CLKFREQ/10 + CNT);
    }
}