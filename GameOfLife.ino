uint8_t nextFrame[64];

void golSetup() {
  wipe();
  makeSomething();
  delay(1000);
}

/* Directions are ordered clockwise starting with 0 = NORTH */
uint8_t getNeighbor(uint8_t placeVal, uint8_t neighbor) {
  uint8_t i = unmorph(placeVal);
  uint8_t px = i % 8;
  uint8_t py = i / 8;
  uint8_t x = 0;
  uint8_t y = 0;
  
  switch (neighbor) {
    case 0:
      x = px;
      y = py - 1;
      break;
    case 1:
      x = px + 1;
      y = py - 1;
      break;
    case 2:
      x = px + 1;
      y = py;
      break;
    case 3:
      x = px + 1;
      y = py + 1;
      break;
    case 4:
      x = px;
      y = py + 1;
      break;
    case 5:
      x = px - 1;
      y = py + 1;
      break;
    case 6:
      x = px - 1;
      y = py;
      break;
    case 7:
      x = px - 1;
      y = py - 1;
      break;
    default:
      x = 0;
      y = 0;
  }
  if (x < 0) x = 7;
  if (x > 7) x = 0;
  if (y < 0) y = 7;
  if (y > 7) y = 0;
  
  return morphXY(x,y);
}

void makeSomething() {
  uint8_t options = 5;
  switch(random(options)) {
    case 0:
      makeGlider();
      break;
    case 1:
      makeOscillator();
      break;
    case 2:
      toggleRow(random(8));
      trellis.writeDisplay();
      break;
    case 3:
      toggleCol(random(8));
      trellis.writeDisplay();
      break;
    case 4:
      make42();
      break;
  }
}

void makeGlider() {
  uint8_t xOffset = random(8);
  uint8_t yOffset = random(8);
  setXY((xOffset + 2) % 8,(yOffset + 3) % 8);
  setXY((xOffset + 1) % 8,(yOffset + 2) % 8);
  setXY((xOffset + 3) % 8,(yOffset + 1) % 8);
  setXY((xOffset + 3) % 8,(yOffset + 2) % 8);
  setXY((xOffset + 3) % 8,(yOffset + 3) % 8);
  trellis.writeDisplay();
}

void makeOscillator() {
  uint8_t xOffset = random(8);
  uint8_t yOffset = random(8);
  setXY((xOffset + 6) % 8,(yOffset + 0) % 8);
  setXY((xOffset + 6) % 8,(yOffset + 6) % 8);
  setXY((xOffset + 6) % 8,(yOffset + 7) % 8);
  setXY((xOffset + 5) % 8,(yOffset + 0) % 8);
  setXY((xOffset + 5) % 8,(yOffset + 1) % 8);
  setXY((xOffset + 5) % 8,(yOffset + 2) % 8);
  trellis.writeDisplay();
}

void make42() {
  uint8_t offset = (random(2) == 0) ? 0 : 1;
  setXY(offset + 0,1);
  setXY(offset + 0,2);
  setXY(offset + 0,3);
  setXY(offset + 0,4);
  setXY(offset + 1,4);
  setXY(offset + 2,1);
  setXY(offset + 2,2);
  setXY(offset + 2,3);
  setXY(offset + 2,4);
  setXY(offset + 2,5);
  setXY(offset + 2,6);
  setXY(offset + 4,1);
  setXY(offset + 4,3);
  setXY(offset + 4,4);
  setXY(offset + 4,5);
  setXY(offset + 4,6);
  setXY(offset + 5,1);
  setXY(offset + 5,3);
  setXY(offset + 5,6);
  setXY(offset + 6,1);
  setXY(offset + 6,2);
  setXY(offset + 6,3);
  setXY(offset + 6,6);
  trellis.writeDisplay();
}

void liveOrDie(uint8_t placeVal) {  
  // Calculate whether to live or die the next round
  uint8_t neighbors = 0;
  for (uint8_t d=0; d<=7; d++) {
    if (trellis.isLED(getNeighbor(placeVal, d))) {
      neighbors++;
    }
  }
  
  if (neighbors == 3 && !trellis.isLED(placeVal)) {
    nextFrame[placeVal] = 1;
  }else if ((neighbors == 2 || neighbors == 3) && trellis.isLED(placeVal)) {
    nextFrame[placeVal] = 1;
  } else {
    nextFrame[placeVal] = 0;
  }
}

void golLoop() {
  if (isEmpty() || random(100) == 0) {
    makeSomething();
  }
  delay(ssDelay);
  
  // Clear out the next frame
  for(uint8_t c=0; c<64; c++) {
    nextFrame[c] = 0;
  }
  
  //compute the next step
  for (uint8_t i=0; i<numKeys; i++) {
    liveOrDie(i);
  }
  
  if (trellis.readSwitches()) {
    // go through every button
    for (uint8_t i=0; i<numKeys; i++) {
      // if it was pressed, add it to the list!
      if (trellis.justPressed(i)) {
        nextFrame[i] = 1;
      }
    }
  }
  
  // Update the map
  for (uint8_t i=0; i<numKeys; i++) {
    if(nextFrame[i] == 1) {
      trellis.setLED(i);
    } else {
      trellis.clrLED(i);
    }
  }
  
  // tell the trellis to set the LEDs we requested
  trellis.writeDisplay(); 
}

