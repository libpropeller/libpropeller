#include "../concurrent_buffer/concurrent_buffer.h"

const int ConcurrentBuffer::kSize;
volatile char ConcurrentBuffer::buffer_[kSize];
volatile int ConcurrentBuffer::head_ = 0;
int ConcurrentBuffer::lock_ = -1;
unsigned int ConcurrentBuffer::timeout_;
