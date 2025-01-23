#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include "heltec-eink-modules.h"

extern EInkDisplay_VisionMasterE213 display;

void updateDisplay(String message) {
   display.clear(); 
   display.setCursor(0, 20);
   display.setTextSize(2); 
   display.setTextColor(0x00); // Set text color to black
   display.print(message);
   display.update(); 
}

#endif // DISPLAY_MANAGER_H
