#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>
#include <SdFat.h>
#include "RepRap_FullGraphicSmartController.h"

char displayLine1[80] = "\0";
char displayLine2[80] = "\0";
char displayLine3[80] = "\0";
char displayLine4[80] = "\0";
char displayLine5[80] = "\0";
char displayLine6[80] = "\0";

/*
 * Function Declarations
 */


RepRap_FullGraphicsSmartController* controller;
void repRapEventHandler(short repRapEvent, bool hasChanged);

bool lcdNeedsUpdate = true;
int actionCount = 0;

File myFile;

char* readLine(File* srcFile) {
  char* buf = new char[500];
  bool el = false;
  short i = 0;
  while(myFile.available() && !el) {
    char r = myFile.read();
    if(r == '\n') {
      buf[i] = '\0';
      el = true;
    } else {
      buf[i] = r;
      i++;
    }
  }

  return buf;
}

void setup() {

  controller = new RepRap_FullGraphicsSmartController(&repRapEventHandler);

  myFile = controller->GetSdFat()->open("jdtest.txt", FILE_WRITE);
  if(myFile) {
    myFile.println("Test Line 1");
    myFile.println("Test Line 2");
    myFile.close();
  }

  myFile = controller->GetSdFat()->open("jdtest.txt");
  if(myFile) {
    char* line1 = readLine(&myFile);
    char* line2 = readLine(&myFile);
    sprintf(displayLine1, "%s", line1);
    sprintf(displayLine2, "%s", line2);
    delete line1;
    delete line2;
    myFile.close();
  }
}

void loop() {
  if(lcdNeedsUpdate) {
    controller->GetDisplay()->firstPage();
    do {
      //sprintf(displayLine1, "MSB: %i  LSB: %i", encKnobMsb, encKnobLsb);
      //sprintf(displayLine2, "STATE: %i / %i", currentEncKnobBitState, lastEncKnobBitState);
      sprintf(displayLine3, "CNT: %i", actionCount);
      
      controller->GetDisplay()->setFont(u8g2_font_6x10_tf);
      controller->GetDisplay()->drawStr(0,7, displayLine1);
      controller->GetDisplay()->drawStr(0, 15, displayLine2);
      controller->GetDisplay()->drawStr(0, 23, displayLine3);
      controller->GetDisplay()->drawStr(0, 31, displayLine4);
      controller->GetDisplay()->drawStr(0, 39, displayLine5);
      controller->GetDisplay()->drawStr(0, 47, displayLine6);
      
    } while ( controller->GetDisplay()->nextPage() );

    lcdNeedsUpdate = false;
  }

  controller->Pulse();
}

void repRapEventHandler(short repRapEvent, bool hasChanged) {
  switch(repRapEvent) {
    case REPRAP_ROTARY_Stopped: {
      if(hasChanged) {
        sprintf(displayLine4, "Encoder Stopped");
        lcdNeedsUpdate = true;
        actionCount = 0;
      }
    } break;
    case REPRAP_ROTARY_Clockwise: {
      if(hasChanged) {
        sprintf(displayLine4, "Encoder Clockwise");
      }
      lcdNeedsUpdate = true;
      actionCount++;
    } break;
    case REPRAP_ROTARY_CounterClockwise: {
      if(hasChanged) {
        sprintf(displayLine4, "Encoder C-Clockwise");
      }
      lcdNeedsUpdate = true;
      actionCount--;
    } break;
    case REPRAP_ROTARY_Pressed: {
      controller->Buzzer_ShortBeep();
      sprintf(displayLine5, "Encoder Pressed");
      lcdNeedsUpdate = true;
    } break;
    case REPRAP_ROTARY_Released: {
      sprintf(displayLine5, "Encoder Release");
      lcdNeedsUpdate = true;
    } break;
    case REPRAP_ROTARY_LongReleased: {
      sprintf(displayLine5, "Encoder Long Release");
      lcdNeedsUpdate = true;
    } break;
    case REPRAP_STOP_Pressed: {
      controller->Buzzer_LongBeep();
      sprintf(displayLine5, "Stop Pressed");
      lcdNeedsUpdate = true;
    } break;
    case REPRAP_STOP_Released: {
      sprintf(displayLine5, "Stop Release");
      lcdNeedsUpdate = true;
    } break;
    case REPRAP_STOP_LongReleased: {
      sprintf(displayLine5, "Stop Long Release");
      lcdNeedsUpdate = true;
    } break;
    case REPRAP_BUZZER_ON: {
      sprintf(displayLine1, "Buzzer On");
      lcdNeedsUpdate = true;
    } break;
    case REPRAP_BUZZER_OFF: {
      sprintf(displayLine1, "");
      lcdNeedsUpdate = true;
    } break;
    case REPRAP_SD_INSERTED: {
      sprintf(displayLine2, "SD CARD PRESENT");
      lcdNeedsUpdate = true;
    } break;
    case REPRAP_SD_REMOVED: {
      sprintf(displayLine2, "");
      lcdNeedsUpdate = true;
    } break;
  }
}
