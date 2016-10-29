#ifndef SRAM_FIFO_h_
#define SRAM_FIFO_h_

#include <FlexiBoard.h>
#include <SPI.h>
#include <SPIFIFO.h>

#define SRAM_SPISETTINGS SPISettings (25e6, MSBFIRST, SPI_MODE0)
#define SRAM_RDMR        5
#define SRAM_WRMR        1
#define SRAM_READ        3
#define SRAM_WRITE       2

#define LED 3

class SRAMFifo
{
  public:
    SRAMFifo(void) {
      init();
    }
    void reset(void) {
       head = tail = headChip = tailChip = count = 0;
    }
    int initOK(void) {
      return initok;
    }
    size_t available(void) {
      return RAM_SIZE_TOTAL - count;
    }
    size_t used(void) {
      return count;
    }    
    int write(uint8_t* buf, size_t length);
    int read(uint8_t* buf, size_t length);

    int check(void);

  private:
    void init(void);    
    size_t head, tail, count;
    volatile uint8_t* csreg[NUM_RAMS];
    uint8_t headChip, tailChip;
    char initok;
};

void SRAMFifo::init(void)
{
  reset();
  initok = true;
  head = tail = headChip = tailChip = count = 0;

  initMEM(); // set pinModes

  SPI.begin();
  SPI.setSCK(SCK);
  SPI.setMISO(MISO);
  SPI.setMOSI(MOSI);

  SPI.beginTransaction(SRAM_SPISETTINGS);

  for (unsigned i = 0; i < NUM_RAMS; i++) {
    csreg[i] = portOutputRegister(ramArrCS[i]);
  }

  for (unsigned i = 0; i < NUM_RAMS; i++) {    
    *csreg[i] = 0;//digitalWriteFast(ramArrCS[i], LOW);
    SPI.transfer(SRAM_WRMR);
    SPI.transfer(0x40); //sequential operation
    *csreg[i] = 1;//digitalWriteFast(ramArrCS[i], HIGH);    
  }

  //Verify
  for (unsigned i = 0; i < NUM_RAMS; i++) {    
    *csreg[i] = 0; //digitalWriteFast(ramArrCS[i], LOW);
    SPI.transfer(SRAM_RDMR);
    char r = SPI.transfer(0);
    if (r != 0x040 ) initok = false;    
    *csreg[i] = 1;//digitalWriteFast(ramArrCS[i], HIGH);
  }

  SPI.endTransaction();
}
  

  
int SRAMFifo::write(uint8_t* buf, size_t length)
{
  if (available() < length) return -1;
  if (length < 1) return 0;
   
  count += length;  
  SETLED(LED, HIGH);
  uint8_t pin = headChip;

  SPI.beginTransaction(SRAM_SPISETTINGS);
  *csreg[pin] = 0;//digitalWriteFast(ramArrCS[pin], LOW);      
  SPI.transfer16((SRAM_WRITE << 8) | (head >> 16));
  SPI.transfer16(head & 0xFFFF);
  
  for (size_t i = 0; i < length; i++) {

    //SPI.transfer(buf[i]);
    while (!(SPI0_SR & SPI_SR_TCF));
    SPI0_POPR;
    SPI0_SR = SPI_SR_TCF;
    SPI0_PUSHR = buf[i];
        
    head++;
    
    if (head >= RAM_SIZE) {     
      head = 0;
      headChip++;
      if (headChip >= NUM_RAMS) headChip = 0;   
      while (!(SPI0_SR & SPI_SR_TCF));      
      SPI0_POPR;      
      *csreg[pin] = 1;//digitalWriteFast(ramArrCS[pin], HIGH);
      pin = headChip;
      *csreg[pin] = 0;//digitalWriteFast(ramArrCS[pin], LOW);      
      SPI.transfer16(SRAM_WRITE << 8);
      SPI.transfer16(0);
    } 
    
  }
  while (!(SPI0_SR & SPI_SR_TCF));
  SPI0_POPR;
  *csreg[pin] = 1;//digitalWriteFast(ramArrCS[pin], HIGH);  
  SPI.endTransaction();
 
  SETLED(LED, LOW);
  return 0;
}

int SRAMFifo::read(uint8_t* buf, size_t length)
{
  if (count < length) return -1;
  if (length < 1) return 0;

  SETLED(LED, HIGH);
  count -= length;
  uint8_t pin = tailChip;
   
  SPI.beginTransaction(SRAM_SPISETTINGS);
  *csreg[pin] = 0;//digitalWriteFast(ramArrCS[pin], LOW);
  SPI.transfer16((SRAM_READ << 8) | (tail >> 16));
  SPI.transfer16(tail & 0xFFFF);

  size_t i = 0;
  
  while ( i < length ) {
  
    //buf[i] = SPI.transfer(0);
    SPI0_SR = SPI_SR_TCF;
    SPI0_PUSHR = 0;
    
    tail++;
    if (tail >= RAM_SIZE) {
      tail = 0;            
      tailChip++;
      if (tailChip >= NUM_RAMS) tailChip = 0;   
      while (!(SPI0_SR & SPI_SR_TCF)) ;    
      buf[i] = SPI0_POPR;      
      *csreg[pin] = 1;//digitalWriteFast(ramArrCS[pin], HIGH);      
      pin = tailChip;
      *csreg[pin] = 0;//digitalWriteFast(ramArrCS[pin], LOW);
      SPI.transfer16(SRAM_READ << 8);
      SPI.transfer16(0);

    } else {
      while (!(SPI0_SR & SPI_SR_TCF)) ; 
      buf[i] = SPI0_POPR;
    }
    i++;
  }
  *csreg[pin] = 1;//digitalWriteFast(ramArrCS[pin], HIGH);
  SPI.endTransaction();  
  
  SETLED(LED, LOW);
  return length;
}

int SRAMFifo::check(void) {
  uint8_t buf0[512];
  uint8_t buf1[512];
  size_t count = 0;
  
  unsigned long time = micros();
  Serial.print("Ram Test...");
  do {
    size_t num = random(sizeof(buf0) - 1);
    for (size_t i = 0; i < num; i++) buf0[i] = random(255);
    write(buf0, num);
    read(buf1, num);
    if (memcmp(buf0, buf1, num) != 0) while (1) {
        Serial.println("SRAM ERROR");
        delay(500);
      }
    count += num;
  } while (count < RAM_SIZE_TOTAL);
  time = micros() - time;
  reset();
//  Serial.printf("SRAM: %d Bytes\n", available());
  return time;
}
#endif
