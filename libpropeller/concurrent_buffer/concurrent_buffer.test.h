#include <stdio.h>
#include <propeller.h>

#include <string.h>

#include "unity.h"
#include "concurrent_buffer.h"

#include "pib.h"

///The system uses a single lock, and the static ConcurrentBuffer takes a second.
const int kFirstAvailableLock = 2;

//TODO(SRLM): When I used this class, it was locking up when used in multiple
// cogs. To solve this, I made head volatile, and that seemed to fix it. I also
// made buffer volatile just for safety. To Do: add a test that will fail if
// head or buffer is not volatile.

class UnityTests {
public:

    static void setUp(void) {
        ConcurrentBuffer::ResetHead();
        ConcurrentBuffer::Stop();
        ConcurrentBuffer::Start();
    }

    static void tearDown(void) {

    }

    // -----------------------------------------------------------------------------

    /**
     * @warning: this function must come first! Since lock allocation is static,
     * once the constructor has been called then this test will not work correctly.
     */
    static void test_CBUsesOnlyOneLock(void) {

        const int lockA = locknew();
        TEST_ASSERT_EQUAL_INT(kFirstAvailableLock, lockA);

        {
            ConcurrentBuffer::Start();
            ConcurrentBuffer::Start();
        }

        const int lockC = locknew();
        TEST_ASSERT_EQUAL_INT(kFirstAvailableLock + 1, lockC);

        ConcurrentBuffer::Start();

        const int lockD = locknew();
        TEST_ASSERT_EQUAL_INT(kFirstAvailableLock + 2, lockD);

        lockret(lockA);
        lockret(lockC);
        lockret(lockD);
    }

    static void test_CBResetHead(void) {
        ConcurrentBuffer::ResetHead();
        ConcurrentBuffer b;
        ConcurrentBuffer::Put('0');
        ConcurrentBuffer::ResetHead();
        ConcurrentBuffer::Put('1');
        TEST_ASSERT_EQUAL_HEX8('1', b.Get());
    }

    static void test_CBBasicSingleCharacterSingleThread(void) {
        ConcurrentBuffer b;
        ConcurrentBuffer::Put('a');

        TEST_ASSERT_EQUAL_HEX8('a', b.Get());
    }

    static void test_CBBasicMultipleCharactersSingleThread(void) {

        char data[] = "abcdef";
        int size = 6;
        ConcurrentBuffer b;
        ConcurrentBuffer::Put(data, size);

        char result[size];
        result[size] = 0; //Null terminate

        for (int i = 0; i < size; i++) {
            result[i] = b.Get();
        }

        TEST_ASSERT_EQUAL_STRING(data, result);
    }

    static void test_CBBasicPutTimeout(void) {
        int timeout = 1000; //1000us = 1 ms
        ConcurrentBuffer::Start(timeout);

        ConcurrentBuffer b;


        TEST_ASSERT_TRUE(ConcurrentBuffer::Put('a'));

        //Simulate a lockout condition:
        TEST_ASSERT_FALSE(lockset(ConcurrentBuffer::lock_));

        unsigned int startCNT = CNT;
        TEST_ASSERT_FALSE(ConcurrentBuffer::Put('b'));
        unsigned int endCNT = CNT;

        //Check to make sure it took less than 2 timeout periods
        TEST_ASSERT_TRUE((endCNT - startCNT) < 2 * (CLKFREQ / 1000000) * timeout);

        lockclr(ConcurrentBuffer::lock_);
    }

    static void test_CBAddLargeAmountsOfData(void) {
        char buffer[] = "abcdef";
        int size = 6;
        ConcurrentBuffer b;

        for (int i = 0; i < 100; ++i) {
            ConcurrentBuffer::Put(buffer, size);
            for (int j = 0; j < size; ++j) {
                TEST_ASSERT_EQUAL_HEX8(buffer[j], b.Get());
            }
        }
    }

    static void test_CBGetFreeBasic(void) {
        char buffer[] = "abcdef";
        int size = 6;

        ConcurrentBuffer b;

        TEST_ASSERT_EQUAL_INT(b.GetkSize(), b.GetFree());

        ConcurrentBuffer::Put(buffer, size);

        TEST_ASSERT_EQUAL_INT(b.GetkSize() - size, b.GetFree());

    }

    static void test_CBGetFreeWrapAround(void) {
        ConcurrentBuffer b;

        //Go to halfway through the buffer
        for (int i = 0; i < (b.GetkSize() / 2); ++i) {
            ConcurrentBuffer::Put('c');
            b.Get();
        }

        //No wrap around
        TEST_ASSERT_EQUAL_INT(b.GetkSize(), b.GetFree());


        //Go for 3/4 the buffer
        for (int i = 0; i < (3 * b.GetkSize() / 4); ++i) {
            ConcurrentBuffer::Put('c');
        }

        TEST_ASSERT_EQUAL_INT(b.GetkSize() - (3 * b.GetkSize() / 4), b.GetFree());
    }

    static void test_CBGetArrayHeadTailEqual(void) {
        ConcurrentBuffer b;
        volatile char * data = NULL;

        TEST_ASSERT_EQUAL_INT(0, b.Get(data));
    }

    static void test_CBGetArrayHeadGreaterThanTail(void) {
        ConcurrentBuffer b;
        volatile char * data = NULL;

        const char * input = "Big, long, String!";

        int input_size = strlen(input) + 1; //+1 for null terminator
        ConcurrentBuffer::Put(input, input_size);

        TEST_ASSERT_EQUAL_INT(input_size, b.Get(data));
        TEST_ASSERT_EQUAL_STRING(input, (char *) data);
    }

    static void test_CBGetArrayHeadLessThanTail(void) {
        //Tests across the loop around boundary of the buffer.

        ConcurrentBuffer b;

        int first_chunk_size = 10;
        const char * input_data = "What is Batman, but a masked vigilante?";
        const int input_size = strlen(input_data) + 1;

        volatile char * test_data_result = NULL;

        // Push some characters through the buffer.
        // +1 because the actual end of the buffer is at kBufferSize + 1.
        for (int i = 0; i < ConcurrentBuffer::GetkSize() + 1 - first_chunk_size; i++) {
            ConcurrentBuffer::Put('A');
            b.Get();
        }

        ConcurrentBuffer::Put(input_data, input_size);

        // Test first half of the result.
        TEST_ASSERT_EQUAL_INT(first_chunk_size, b.Get(test_data_result));
        TEST_ASSERT_EQUAL_MEMORY(input_data, test_data_result, first_chunk_size);

        // Test second half of the result.
        TEST_ASSERT_EQUAL_INT(input_size - first_chunk_size, b.Get(test_data_result));
        TEST_ASSERT_EQUAL_MEMORY(&input_data[first_chunk_size], test_data_result, input_size - first_chunk_size);


    }

    static void test_CBGetArrayReturnsAllAvailableElements(void) {
        ConcurrentBuffer b;

        const char * input = "A Lannister always pays his debts.";
        const int input_size = strlen(input) + 1;

        volatile char * data = NULL;

        ConcurrentBuffer::Put(input, input_size);
        b.Get(data);

        TEST_ASSERT_EQUAL_INT(0, b.Get(data));
    }

    static void test_CBPutArrayString(void) {
        ConcurrentBuffer buffer;
        const char * a = "Alabama, Arkansas";
        const char * b = "Baltimore, Bellingham";
        const int a_size = strlen(a) + 1;
        const int b_size = strlen(b) + 1;

        ConcurrentBuffer::PutWithString(a, a_size, b);
        volatile char * result;
        TEST_ASSERT_EQUAL_INT(a_size + b_size, buffer.Get(result));
        TEST_ASSERT_EQUAL_MEMORY(a, result, a_size);
        TEST_ASSERT_EQUAL_MEMORY(b, &(result[a_size]), b_size);

        TEST_ASSERT_EQUAL_INT(buffer.GetkSize(), buffer.GetFree());

    }

    static void test_CBPutArrayStringNonStandardTerminator(void) {
        ConcurrentBuffer buffer;
        const char * a = "Alabama, Arkansas";
        const char * b = "Baltimore, Bellingham";
        const int a_size = strlen(a) + 1;
        const int b_size = (int) strchr(b, ' ') - (int) b + 1; //Includes terminator

        ConcurrentBuffer::PutWithString(a, a_size, b, ' ');
        volatile char * result;

        TEST_ASSERT_EQUAL_INT(a_size + b_size, buffer.Get(result));
        TEST_ASSERT_EQUAL_MEMORY(a, result, a_size);
        TEST_ASSERT_EQUAL_MEMORY(b, &(result[a_size]), b_size);

        TEST_ASSERT_EQUAL_INT(buffer.GetkSize(), buffer.GetFree());
    }

    // -----------------------------------------------------------------------------
    // PIB (PutIntoBuffer) tests
    // -----------------------------------------------------------------------------

    static void test_PIB_3x2Simple(void) {
        ConcurrentBuffer buffer;

        int cnt = 0x7278FE86;
        char identifier = 'D';
        int a = 0xFFFF;
        int b = 0x0000;
        int c = 0x0787;

        PIB::_3x2(identifier, cnt, a, b, c);

        TEST_ASSERT_EQUAL_HEX8(identifier, buffer.Get());

        TEST_ASSERT_EQUAL_HEX32(cnt,
                buffer.Get() | buffer.Get() << 8 | buffer.Get() << 16 | buffer.Get() << 24);

        TEST_ASSERT_EQUAL_HEX16(a, buffer.Get() | buffer.Get() << 8);
        TEST_ASSERT_EQUAL_HEX16(b, buffer.Get() | buffer.Get() << 8);
        TEST_ASSERT_EQUAL_HEX16(c, buffer.Get() | buffer.Get() << 8);
    }

    static void test_PIB_3x2ThrowsAwayUpperBytes(void) {
        ConcurrentBuffer buffer;

        int cnt = 0x90523EDC;
        char identifier = 'D';
        int a = 0x1FFFF;
        int b = 0x10000;
        int c = 0xFFFF0787;

        PIB::_3x2(identifier, cnt, a, b, c);

        TEST_ASSERT_EQUAL_HEX8(identifier, buffer.Get());

        TEST_ASSERT_EQUAL_HEX32(cnt,
                buffer.Get() | buffer.Get() << 8 | buffer.Get() << 16 | buffer.Get() << 24);

        TEST_ASSERT_EQUAL_HEX16(a, buffer.Get() | buffer.Get() << 8);
        TEST_ASSERT_EQUAL_HEX16(b, buffer.Get() | buffer.Get() << 8);
        TEST_ASSERT_EQUAL_HEX16(c, buffer.Get() | buffer.Get() << 8);
    }

    static void test_PIB_3x2DoesNotPutInExtraBytes(void) {
        ConcurrentBuffer buffer;

        int cnt = 0x7278FE86;
        char identifier = 'D';
        int a = 0xEEFFFF;
        int b = 0x200000;
        int c = 0x88970787;


        PIB::_3x2(identifier, cnt, a, b, c);

        TEST_ASSERT_EQUAL_HEX8(identifier, buffer.Get());

        TEST_ASSERT_EQUAL_HEX32(cnt,
                buffer.Get() | buffer.Get() << 8 | buffer.Get() << 16 | buffer.Get() << 24);

        TEST_ASSERT_EQUAL_HEX16(a, buffer.Get() | buffer.Get() << 8);
        TEST_ASSERT_EQUAL_HEX16(b, buffer.Get() | buffer.Get() << 8);
        TEST_ASSERT_EQUAL_HEX16(c, buffer.Get() | buffer.Get() << 8);

        volatile char * temp = NULL;
        TEST_ASSERT_EQUAL_INT(0, buffer.Get(temp));
    }

    static void test_PIB_3x4Simple(void) {
        ConcurrentBuffer buffer;

        int cnt = 0x7278FE86;
        char identifier = 'D';
        int a = 0xFFFFFFFF;
        int b = 0x0000;
        int c = 0xF4787;

        PIB::_3x4(identifier, cnt, a, b, c);

        TEST_ASSERT_EQUAL_HEX8(identifier, buffer.Get());

        TEST_ASSERT_EQUAL_HEX32(cnt,
                buffer.Get() | buffer.Get() << 8 | buffer.Get() << 16 | buffer.Get() << 24);

        TEST_ASSERT_EQUAL_HEX32(a,
                buffer.Get() | buffer.Get() << 8 | buffer.Get() << 16 | buffer.Get() << 24);

        TEST_ASSERT_EQUAL_HEX32(b,
                buffer.Get() | buffer.Get() << 8 | buffer.Get() << 16 | buffer.Get() << 24);

        TEST_ASSERT_EQUAL_HEX32(c,
                buffer.Get() | buffer.Get() << 8 | buffer.Get() << 16 | buffer.Get() << 24);
    }

    static void test_PIB_3x4DoesNotPutInExtraBytes(void) {
        ConcurrentBuffer buffer;

        int cnt = 0x8743470;
        char identifier = 'E';
        int a = 0x00ECB95;
        int b = 0x1970;
        int c = 0x0000;

        PIB::_3x4(identifier, cnt, a, b, c);

        TEST_ASSERT_EQUAL_HEX8(identifier, buffer.Get());

        TEST_ASSERT_EQUAL_HEX32(cnt,
                buffer.Get() | buffer.Get() << 8 | buffer.Get() << 16 | buffer.Get() << 24);

        TEST_ASSERT_EQUAL_HEX32(a,
                buffer.Get() | buffer.Get() << 8 | buffer.Get() << 16 | buffer.Get() << 24);

        TEST_ASSERT_EQUAL_HEX32(b,
                buffer.Get() | buffer.Get() << 8 | buffer.Get() << 16 | buffer.Get() << 24);

        TEST_ASSERT_EQUAL_HEX32(c,
                buffer.Get() | buffer.Get() << 8 | buffer.Get() << 16 | buffer.Get() << 24);

        volatile char * temp = NULL;
        TEST_ASSERT_EQUAL_INT(0, buffer.Get(temp));
    }

    static void test_PIB_2x4Simple(void) {
        ConcurrentBuffer buffer;

        int cnt = 0x7278FE86;
        char identifier = 'D';
        int a = 0xFFFFFFFF;
        int b = 0x0000;

        PIB::_2x4(identifier, cnt, a, b);

        TEST_ASSERT_EQUAL_HEX8(identifier, buffer.Get());

        TEST_ASSERT_EQUAL_HEX32(cnt,
                buffer.Get() | buffer.Get() << 8 | buffer.Get() << 16 | buffer.Get() << 24);

        TEST_ASSERT_EQUAL_HEX32(a,
                buffer.Get() | buffer.Get() << 8 | buffer.Get() << 16 | buffer.Get() << 24);

        TEST_ASSERT_EQUAL_HEX32(b,
                buffer.Get() | buffer.Get() << 8 | buffer.Get() << 16 | buffer.Get() << 24);
    }

    static void test_PIB_2x4DoesNotPutInExtraBytes(void) {
        ConcurrentBuffer buffer;

        int cnt = 0x0;
        char identifier = 'H';
        int a = 0x1;
        int b = 0xFFFFFFFF;

        PIB::_2x4(identifier, cnt, a, b);

        TEST_ASSERT_EQUAL_HEX8(identifier, buffer.Get());

        TEST_ASSERT_EQUAL_HEX32(cnt,
                buffer.Get() | buffer.Get() << 8 | buffer.Get() << 16 | buffer.Get() << 24);

        TEST_ASSERT_EQUAL_HEX32(a,
                buffer.Get() | buffer.Get() << 8 | buffer.Get() << 16 | buffer.Get() << 24);

        TEST_ASSERT_EQUAL_HEX32(b,
                buffer.Get() | buffer.Get() << 8 | buffer.Get() << 16 | buffer.Get() << 24);

        volatile char * temp = NULL;
        TEST_ASSERT_EQUAL_INT(0, buffer.Get(temp));
    }

    static void test_PIB_stringSimple(void) {
        ConcurrentBuffer buffer;

        int cnt = 0x7278FE86;
        char identifier = 'D';
        char string [] = "Hello, World!\0\n";

        PIB::_string(identifier, cnt, string, '\n');

        TEST_ASSERT_EQUAL_HEX8(identifier, buffer.Get());

        TEST_ASSERT_EQUAL_HEX32(cnt,
                buffer.Get() | buffer.Get() << 8 | buffer.Get() << 16 | buffer.Get() << 24);

        volatile char * result = NULL;
        buffer.Get(result);
        TEST_ASSERT_EQUAL_STRING(string, result);

        //No extra bytes:
        volatile char * temp = NULL;
        TEST_ASSERT_EQUAL_INT(0, buffer.Get(temp));

    }



};
