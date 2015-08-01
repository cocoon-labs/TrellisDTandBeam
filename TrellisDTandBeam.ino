#include <Wire.h>
#include "Adafruit_Trellis.h"

#define numKeys 64

// mode options
#define SCREENSAVER 0
#define MODES 1
#define COLORS 2
#define ARCADE 3
#define SLEEP 4

// screensaver mode options
#define GOL 0
#define RINGS 1

// serial tags
#define SWITCHMODE 0
#define MODESEL 1
#define COLORSEL 2
#define BEAMVAL 3

// Connect Trellis Vin to 5V and Ground to ground.
// Connect the INT wire to pin #A2 (can change later!)
#define INTPIN A2
// Connect I2C SDA pin to your Arduino SDA line
// Connect I2C SCL pin to your Arduino SCL line

uint16_t beamPin = A0;

Adafruit_Trellis matrix0 = Adafruit_Trellis();
Adafruit_Trellis matrix1 = Adafruit_Trellis();
Adafruit_Trellis matrix2 = Adafruit_Trellis();
Adafruit_Trellis matrix3 = Adafruit_Trellis();

Adafruit_TrellisSet trellis =  Adafruit_TrellisSet(&matrix0, &matrix1, &matrix2, &matrix3);

uint8_t mode = SCREENSAVER;

uint8_t ssMode = RINGS;
uint8_t ssModeCount = 2;
uint8_t ssDelay = 100; // 100ms delay is minimum

uint8_t nColorPresets = 0;
uint8_t nModePresets = 0;

uint8_t randFactor = 100;

uint16_t beamState = 0;
boolean stateChanged = false;
uint8_t tolerance = 5;


void setup() {
  Serial.begin(9600);

  // INT pin requires a pullup
  pinMode(INTPIN, INPUT);
  digitalWrite(INTPIN, HIGH);
  randomSeed(analogRead(0));
  
  // begin() with the addresses of each panel in order
  trellis.begin(0x70, 0x71, 0x72, 0x73);

  wipe();
  setupMode();
}

void setupMode() {
  switch(mode) {
    case SCREENSAVER:
      wipe();
      break;
    case MODES:
      modesSetup();
      break;
    case COLORS:
      colorsSetup();
      break;
    case ARCADE:
      //arcadeSetup();
      wipe();
      break;
    case SLEEP:
      wipe();
      break;
  }
}

void loop() {
  delay(30); // 30ms delay is required, dont remove me!
  listenForPi();
  
  if (checkBeam()) {
    sendBeamVal();
  }
  
  switch(mode) {
    case SCREENSAVER:
      ssLoop();
      break;
    case MODES:
      modesLoop();
      break;
    case COLORS:
      colorsLoop();
      break;
    case ARCADE:
      ssLoop();
      break;
    case SLEEP:
      // do nothing
      break;
  }
}

void ssLoop() {
  ssRandomize();
  switch(ssMode) {
    case GOL:
      golLoop();
      break;
    case RINGS:
      ringsLoop();
      break;
  }
}

void ssRandomize() {
  if (random(randFactor) == 0) {
    uint8_t newSSMode = random(ssModeCount);
    if (ssMode != newSSMode) {
      ssMode = newSSMode;
      wipe();
    }
  }
  if (random(randFactor) == 0) {
    ssDelay = random(400) + 100;
  }
}

uint8_t unmorph(uint8_t i) {
  uint8_t newI;
  if (i < 16) {
    uint8_t r = i / 4;
    newI = i + (r * 4);
  } else if (i < 32) {
    i -= 16;
    uint8_t r = i / 4;
    newI = i + 4 + (r * 4);
  } else if (i < 48) {
    i -= 32;
    uint8_t r = i / 4;
    newI = i + 36 + (r * 4);
  } else {
    i -= 48;
    uint8_t r = i / 4;
    newI = i + 32 + (r * 4);
  }
  return newI;
}

uint8_t morph(uint8_t i) {
  uint8_t newI;
  uint8_t r = i / 8;
  uint8_t c = i % 8;
  if (i < 32) {
    if (c < 4) {
      newI = (4 * r) + c;
    } else {
      c -= 4;
      newI = (4 * r) + c + 16;
    }    
  } else {
    if (c < 4) {
      r -= 4;
      newI = (4 * r) + c + 48;
    } else {
      r -= 4;
      c -= 4;
      newI = (4 * r) + c + 32;
    }
  }
  return newI;
}

uint8_t morphXY(uint8_t x, uint8_t y) {
  return morph((8 * y) + x);
}

void setXY(uint8_t x, uint8_t y) {
  trellis.setLED(morphXY(x, y));
}

void clrXY(uint8_t x, uint8_t y) {
  trellis.clrLED(morphXY(x, y));
}

boolean justPressedXY(uint8_t x, uint8_t y) {
  return trellis.justPressed(morphXY(x, y));
}

void toggle(uint8_t placeVal) {
  if (trellis.isLED(placeVal))
    trellis.clrLED(placeVal);
  else
    trellis.setLED(placeVal);
}

void wipe() {
  for (uint8_t i=0; i<numKeys; i++) {
    trellis.clrLED(i);
  }
  trellis.writeDisplay();
}

void setAllOff() {
  for (uint8_t i=0; i<numKeys; i++) {
    trellis.clrLED(i);
  }
}

void drawRows() {
  for (uint8_t y=0; y<8; y++) {
    for (uint8_t x=0; x<8; x++) {
      trellis.setLED(morphXY(x, y));
      trellis.writeDisplay();
      delay(50);
    }
  }
}

void setRow(uint8_t y) {
  for (uint8_t x=0; x<8; x++) {
    trellis.setLED(morphXY(x, y));
  }
}

void setCol(uint8_t x) {
  for (uint8_t y=0; y<8; y++) {
    trellis.setLED(morphXY(x, y));
  }
}

void setRowPart(uint8_t y, uint8_t minim, uint8_t maxim) {
  for (uint8_t x=minim; x<=maxim; x++) {
    trellis.setLED(morphXY(x, y));
  }
}

void setColPart(uint8_t x, uint8_t minim, uint8_t maxim) {
  for (uint8_t y=minim; y<=maxim; y++) {
    trellis.setLED(morphXY(x, y));
  }
}

void toggleRow(uint8_t y) {
  for (uint8_t x=0; x<8; x++) {
    toggle(morphXY(x, y));
  }
}

void toggleCol(uint8_t x) {
  for (uint8_t y=0; y<8; y++) {
    toggle(morphXY(x, y));
  }
}

boolean isEmpty() {
  for (uint8_t i=0; i<numKeys; i++) {
    if (trellis.isLED(i)) {
      return false;
    }
  }
  return true;
}

boolean toggleModeLoop() {
  if (trellis.readSwitches()) {
    // go through every button
    for (uint8_t i=0; i<numKeys; i++) {
      // if it was pressed...
      if (trellis.justPressed(i)) {
	// Alternate the LED
	if (trellis.isLED(i))
	  trellis.clrLED(i);
	else
	  trellis.setLED(i);
      } 
    }
    // tell the trellis to set the LEDs we requested
    trellis.writeDisplay();
  }
}

void bitAnd() {
  if (Serial.available()) {
    byte data[8];
    for (int i = 0; i < 8; i++) {
      data[i] = Serial.read();
    }
    for (uint8_t i=0; i<numKeys; i++) {
      if ((data[i/8] & (1 << ((i%8)-1))) != 0)
	trellis.setLED(i);
    }
  }
}

void bitXAnd() {
  if (Serial.available() >= 8) {
    byte data[8];
    readNBytes(data, 8);
    for (uint8_t i=0; i<numKeys; i++) {
      if ((data[i/8] & (1 << ((i%8)))) != 0)
	trellis.setLED(i);
      else
        trellis.clrLED(i);
    }
  }
}

void readNBytes(byte *data, uint8_t n) {
  if (Serial.available() >= n) {
    for (int i = n - 1; i >= 0; i--) {
      *(data + i) = Serial.read();
    }
  }
}

void listenForPi() {
  if (Serial.available() >= 2) {
    parseSerial();
  }
}

void parseSerial() {
  byte serialTag = Serial.read();
  switch(serialTag) {
    case SWITCHMODE:
      mode = Serial.read();
      if (mode == MODES) {
        nModePresets = Serial.read();
      } else if (mode == COLORS) {
        nColorPresets = Serial.read();
      }
      setupMode();
      break;
    default:
      Serial.print("Unknown serial tag: ");
      Serial.println(serialTag);
      break;
  }
}

boolean checkBeam() {
  boolean result = false;
  int reading = analogRead(beamPin);
  result = abs(reading - beamState) >= tolerance;
  if (result) beamState = reading;
  stateChanged = result;
  return result;
}

void sendBeamVal() {
  Serial.write(BEAMVAL);
  byte val = (byte) (beamState >> 2);
  Serial.write(val);
}
