// Run this function upon switching to "colors" screen
void colorsSetup() {
  wipe();
  populateColors();
  trellis.writeDisplay();
}

void populateColors() {
  for (uint8_t i = 0; i < nColorPresets; i++) {
    setXY(i/8, i%8);
  }
}

void colorsLoop() { 
  if (trellis.readSwitches()) {
    // go through every color preset
    for (uint8_t i=0; i<nColorPresets; i++) {
      // if it was pressed, send color preset ID and exit loop
      if (justPressedXY(i/8, i%8)) {
        sendColor(i);
        break;
      }
    }
  }
}

void sendColor(uint8_t c) {
  Serial.write(COLORSEL);
  Serial.write(c);
}
