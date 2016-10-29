#ifndef _FIFO_h_
#define _FIFO_h_

#include <FlexiBoard.h>

#if defined(__MK64FX512__) //Teensy 3.5
#define FIFO_SIZE 1024*120 
#elif defined(__MK66FX1M0__) //Teensy 3.6
#define FIFO_SIZE 1024*180 
#else
#error FIFO in RAM: Teensy 3.5 or 3.6 only
#endif


class Fifo
{
  public:
    Fifo(void) {
      size = FIFO_SIZE;
      reset();
    }
    void reset(void) {
       head = tail = count = 0;
    }
    inline size_t available(void) {
      return FIFO_SIZE - count;
    }
    inline size_t used(void) {
      return count;
    }    
    int write(uint8_t* buf, size_t length);
    int read(uint8_t* buf, size_t length);
   
  private:    
    size_t size, head, tail, count;
    uint8_t fifodata[FIFO_SIZE];
};

  
int Fifo::write(uint8_t* buf, size_t length)
{
  if (available() < length) return -1;
  
  count += length;  
  int len = min(length, size - head);
  memcpy(fifodata + head, buf, len);  
  head +=len;
  buf += len;
  len = length - len;
  if (len) {
    memcpy(fifodata, buf, len);
    head = len;
  }

  return length;
}

int Fifo::read(uint8_t* buf, size_t length)
{
  if (count < length) length=count;     
  count -= length;    
  int len = min(length, size - tail);
  memcpy(buf, fifodata + tail, len);  
  tail +=len;
  buf += len;
  len = length - len;
  if (len) {
    memcpy(buf, fifodata, len);
    tail = len;
  }  
//  if (count==0) tail=head=0;//optimization
  return length;
}

#endif
