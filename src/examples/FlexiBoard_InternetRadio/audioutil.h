
void setI2SFreq(int freq) {
  typedef struct {
    uint8_t mult;
    uint16_t div;
  } __attribute__((__packed__)) tmclk;

  const int numfreqs = 8;
  const int samplefreqs[numfreqs] = { 8000, 11025, 16000, 22050, 32000, 44100, 44117 , 48000 };

#if (F_PLL==16000000)
  const tmclk clkArr[] = {{16, 125}, {148, 839}, {32, 125}, {145, 411}, {64, 125}, {151, 214}, {12, 17}, {96, 125} };
#elif (F_PLL==72000000)
  const tmclk clkArr[] = {{32, 1125}, {49, 1250}, {64, 1125}, {49, 625}, {128, 1125}, {98, 625}, {8, 51}, {64, 375} };
#elif (F_PLL==96000000)
  const tmclk clkArr[] = {{8, 375}, {60, 2041}, {16, 375}, {120, 2041}, {32, 375}, {147, 1250}, {2, 17}, {16, 125} };
#elif (F_PLL==120000000)
  const tmclk clkArr[] = {{32, 1875}, {29, 1233}, {64, 1875}, {89, 1892}, {128, 1875}, {89, 946}, {8, 85}, {64, 625} };
#elif (F_PLL==144000000)
  const tmclk clkArr[] = {{16, 1125}, {40, 2041}, {32, 1125}, {49, 1250}, {64, 1125}, {49, 625}, {4, 51}, {32, 375} };
#elif (F_PLL==180000000)
  const tmclk clkArr[] = {{9, 791}, {31, 1977}, {37, 1626}, {62, 1977}, {73, 1604}, {107, 1706}, {16, 255}, {128, 1875} };
#elif (F_PLL==192000000)
  const tmclk clkArr[] = {{4, 375}, {30, 2041}, {8, 375}, {60, 2041}, {16, 375}, {120, 2041}, {1, 17}, {8, 125} };
#elif (F_PLL==216000000)
  const tmclk clkArr[] = {{17, 1793}, {17, 1301}, {34, 1793}, {49, 1875}, {49, 1292}, {98, 1875}, {8, 153}, {64, 1125} };
#elif (F_PLL==240000000)
  const tmclk clkArr[] = {{16, 1875}, {24, 2041}, {32, 1875}, {29, 1233}, {64, 1875}, {89, 1892}, {4, 85}, {32, 625} };
#endif

  for (int f = 0; f < numfreqs; f++) {
    if ( freq == samplefreqs[f] ) {
      while (I2S0_MCR & I2S_MCR_DUF) ;
      I2S0_MDR = I2S_MDR_FRACT((clkArr[f].mult - 1)) | I2S_MDR_DIVIDE((clkArr[f].div - 1));
      return;
    }
  }
}

