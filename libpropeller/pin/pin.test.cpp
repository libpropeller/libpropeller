#include "unity.h"
#include "c++-alloc.h"
#include "pin.h"

#include "board/board_unit_tester.h"


Pin * a = NULL;
Pin * b = NULL;


void setUp(void){
    a = new Pin(Board::kPinTie1a);
    b = new Pin(Board::kPinTie1b);
}

void tearDown(void){
    delete a;
    delete b;
    a = b = NULL;
}

void test_HighLow(void){
    a->high();
    TEST_ASSERT_EQUAL_INT(1, b->input());
    
    a->low();
    TEST_ASSERT_EQUAL_INT(0, b->input());
}

void test_Toggle(void){
    a->high();
    int output = 1;
    for(int i = 0; i < 100; i++){
        a->toggle();
        output = output == 1 ? 0 : 1;
        TEST_ASSERT_EQUAL_INT(output, b->input());
    }
}

void test_isOutput(void){
    a->input();
    TEST_ASSERT_FALSE(a->isOutput());
    
    a->high();
    TEST_ASSERT_TRUE(a->isOutput());
}

void test_Ouput(void){
    a->output(1);
    TEST_ASSERT_EQUAL_INT(1, b->input());
    
    a->output(0);
    TEST_ASSERT_EQUAL_INT(0, b->input());
}