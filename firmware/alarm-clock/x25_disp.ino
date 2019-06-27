TM1637Display dispDrive(PIN_CLK, PIN_DATA);

const uint8_t DISP_AP[] = {
  SEG_A | SEG_B | SEG_C | SEG_E | SEG_F | SEG_G,
  SEG_A | SEG_B | SEG_E | SEG_F | SEG_G,
  0, 0
};

const uint8_t DISP_CON[] = {
  SEG_A | SEG_D | SEG_E | SEG_F,
  SEG_C | SEG_D | SEG_E | SEG_G,
  SEG_C | SEG_E | SEG_G
  , 0
};

const uint8_t DISP_OTA[] = {
  SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,
  SEG_D | SEG_E | SEG_F | SEG_G,
  SEG_A | SEG_B | SEG_C | SEG_E | SEG_F | SEG_G,
  0
};

const uint8_t DISP_NOTIME[] = {
  SEG_G, SEG_G, SEG_G, SEG_G
};

const uint8_t DISP_ALARM[] = {
  SEG_A | SEG_B | SEG_C | SEG_E | SEG_F | SEG_G,
  SEG_D | SEG_E | SEG_F,
  0,
  0
};

const uint8_t DISP_RESET[] = {
  SEG_E | SEG_G,
  SEG_A | SEG_C | SEG_D | SEG_F | SEG_G,
  SEG_D | SEG_E | SEG_F | SEG_G,
  0
};



uint16_t dispTime;
uint8_t dispDots;
uint8_t dispMask;

void dispSetup() {
  dispDrive.setBrightness(0xf, 1);
  dispTime = 0;
  dispDots = 0;
  dispMask = 0xf;
  dispDrive.clear();
}

void dispShowBuffer() {
  uint8_t buf[4];
  uint16_t t = dispTime;
  buf[3] = dispMask & 0b0001 ? (dispDrive.encodeDigit(t % 10) | (dispDots & 0b0001 ? 0b10000000 : 0)) : 0;
  t /= 10;
  buf[2] = dispMask & 0b0010 ? (dispDrive.encodeDigit(t % 10) | (dispDots & 0b0010 ? 0b10000000 : 0)) : 0;
  t /= 10;
  buf[1] = dispMask & 0b0100 ? (dispDrive.encodeDigit(t % 10) | (dispDots & 0b0100 ? 0b10000000 : 0)) : 0;
  t /= 10;
  buf[0] = dispMask & 0b1000 ? (dispDrive.encodeDigit(t % 10) | (dispDots & 0b1000 ? 0b10000000 : 0)) : 0;
  t /= 10;

  dispDrive.setSegments(buf);
}

void dispShowStatic(const uint8_t* data) {
  dispDrive.setSegments(data);
}

void dispShowStatic2(const uint8_t* data, const uint8_t c) {
  uint8_t buf[4];
  buf[0] = data[0];
  buf[1] = data[1];
  buf[2] = data[2];
  buf[3] = c;
  dispDrive.setSegments(buf);
}

uint8_t dispDigit(const uint8_t digit) {
  return dispDrive.encodeDigit(digit);
}
