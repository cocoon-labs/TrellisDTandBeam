// Run this function upon switching to "modes" screen
void modesSetup() {
  wipe();
  populateModes();
  trellis.writeDisplay();
}

void populateModes() {
  for (uint8_t i = 0; i < nModePresets; i++) {
    setXY(7 - i/8, i%8);
  }
}

void modesLoop() {
  if (trellis.readSwitches()) {
    // go through every mode preset
    for (uint8_t i=0; i<nModePresets; i++) {
      // if it was pressed, send mode preset ID and exit loop
      if (justPressedXY(7 - i/8, i%8)) {
        sendMode(i);
        break;
      }
    }
  }
}

void sendMode(uint8_t m) {
  Serial.write(MODESEL);
  Serial.write(m);
}
