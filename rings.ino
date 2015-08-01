#define IN 0
#define OUT 1

uint8_t dir = OUT;
uint8_t ringPos = 0; // 0 is innermost ring index

void ringsSetup() {
  wipe();
  setFirstPos();
  setRing(ringPos);
  trellis.writeDisplay();
}

void setFirstPos() {
  if (dir == OUT) {
    ringPos = 0;
  } else {
    ringPos = 3;
  }
}

void ringsLoop() {
  if (isEmpty()) {
    ringsSetup();
  }
  delay(ssDelay);
  
  ringsRandomize();
  if (trellis.readSwitches()) {
    for (uint8_t i=0; i<numKeys; i++) {
      // if it was pressed...
      if (trellis.justPressed(i)) {
        toggleDir();
        break;
      }
    }
  }
  
  if (dir == OUT) {
    ringPos = (ringPos + 1) % 4;
  } else {
    ringPos = (ringPos - 1) % 4;
  }
  setAllOff();
  setRing(ringPos);
  trellis.writeDisplay();
}

void setRing(uint8_t r) {
  r = r % 4;
  setRowPart(3 - r, 3 - r, 4 + r);
  setRowPart(4 + r, 3 - r, 4 + r);
  setColPart(3 - r, 3 - r, 4 + r);
  setColPart(4 + r, 3 - r, 4 + r);
}

void ringsRandomize() {
  if (random(randFactor) == 0) {
    toggleDir();
  }
}

void toggleDir() {
  if (dir == OUT) {
    dir = IN;
  } else {
    dir = OUT;
  }
}

