#include "RepRap_FullGraphicSmartController.h"

#define BUZZER_STATE_SILENT 0
#define BUZZER_STATE_ON 1
#define BUZZER_STATE_OFF 2

RepRap_FullGraphicsSmartController::RepRap_FullGraphicsSmartController(void (* repRapEventHandler)(short repRapEvent, bool hasChanged))
{
  RotaryEncoder_Pin1 = RR_CFG_Encoder_DirectionPin1;
  RotaryEncoder_Pin2 = RR_CFG_Encoder_DirectionPin2;
  RotaryEncoder_ButtonPin = RR_CFG_Encoder_ButtonPin;
  RotaryEncoder_ButtonState = HIGH;
  RotaryEncoder_TriggerThreshold = RR_CFG_Encoder_Threshold;
  RotaryEncoder_TimeoutThreshold = RR_CFG_Encoder_Timeout;
  RotaryEncoder_CurrentState = 3;
  RotaryEncoder_LastState = 3;
  RotaryEncoder_LastEventState = REPRAP_ROTARY_Stopped;
  RotaryEncoder_StateCounter = 0;
  RotaryEncoder_CheckCounter = 0;
  RotaryEncoder_LongPressThreshold = RR_CFG_Encoder_LongThreshold;
  RotaryEncoder_LongPressCounter = 0;
  
  StopButton_Pin = RR_CFG_StopButton_Pin;
  StopButton_State = HIGH;
  StopButton_LongPressThreshold = RR_CFG_StopButton_LongThreshold;
  StopButton_LongPressCounter = 0;
  
  Buzzer_Pin = RR_CFG_Buzzer_Pin;
  Buzzer_Length = 0;
  Buzzer_State = BUZZER_STATE_SILENT;
  Buzzer_StateCounter = 0;
  
  LCD_Display = NULL;
  
  SDCard_CsPin = RR_CFG_Sd_CsPin;
  SDCard_DetectPin = RR_CFG_Sd_DetectPin;
  SDCard_DetectState = 0;
  
  RepRapEventHandler = repRapEventHandler;
  
  //If they didn't bother to pass an event handler, don't bother setting up
  if(RepRapEventHandler != NULL) {
    pinMode(RotaryEncoder_Pin1, INPUT_PULLUP);
    pinMode(RotaryEncoder_Pin2, INPUT_PULLUP);
    pinMode(RotaryEncoder_ButtonPin, INPUT_PULLUP);
    
    pinMode(StopButton_Pin, INPUT_PULLUP);
    
    pinMode(Buzzer_Pin, OUTPUT);
    digitalWrite(Buzzer_Pin, LOW);
    
    pinMode(SDCard_DetectPin, INPUT_PULLUP);
    SDCard_DetectState = digitalRead(SDCard_DetectPin);
    
    if(SDCard_DetectState == RR_SD_CARD_PRESENT) {
      RepRapEventHandler(REPRAP_SD_INSERTED, true);
    }
    
    SDCard_SdFat = new SdFat();
    SDCard_SdFat->begin(RR_CFG_Sd_CsPin);
    
    LCD_Display = new U8G2_ST7920_128X64_1_SW_SPI(U8G2_R0, RR_CFG_Lcd_ClkPin, RR_CFG_Lcd_DataPin, RR_CFG_Lcd_CsPin);
    LCD_Display->begin();
  }
};

void RepRap_FullGraphicsSmartController::Pulse()
{
  Pulse_RotaryEncoder();
  Pulse_StopButton();
  Pulse_Buzzer();
  Pulse_SDCard();
};

void RepRap_FullGraphicsSmartController::Buzzer_On()
{
  Buzzer_State = BUZZER_STATE_ON;
  digitalWrite(Buzzer_Pin, HIGH);
  Buzzer_StateCounter = 0;
  Buzzer_Length = 0;
  
  if(RepRapEventHandler != NULL) {
    RepRapEventHandler(REPRAP_BUZZER_ON, true);
  }
};

void RepRap_FullGraphicsSmartController::Buzzer_Off()
{
  Buzzer_State = BUZZER_STATE_SILENT;
  digitalWrite(Buzzer_Pin, LOW);
  Buzzer_StateCounter = 0;
  Buzzer_Length = 0;
  
  if(RepRapEventHandler != NULL) {
    RepRapEventHandler(REPRAP_BUZZER_OFF, true);
  }
};

void RepRap_FullGraphicsSmartController::Buzzer_Beep(short Buzzer_BeepTime)
{
  if(Buzzer_State == BUZZER_STATE_SILENT) {
    Buzzer_On();
    Buzzer_State = BUZZER_STATE_OFF;
    digitalWrite(Buzzer_Pin, HIGH);
    Buzzer_StateCounter = 0;
    Buzzer_Length = Buzzer_BeepTime;
  }
};

void RepRap_FullGraphicsSmartController::Buzzer_ShortBeep()
{
  Buzzer_Beep(RR_CFG_Buzzer_ShortLength);
};

void RepRap_FullGraphicsSmartController::Buzzer_LongBeep()
{
  Buzzer_Beep(RR_CFG_Buzzer_LongLength);
};

U8G2_ST7920_128X64_1_SW_SPI* RepRap_FullGraphicsSmartController::GetDisplay()
{
  return LCD_Display;
};

short RepRap_FullGraphicsSmartController::GetSdCardState()
{
  return SDCard_DetectState;
};

SdFat* RepRap_FullGraphicsSmartController::GetSdFat()
{
  return SDCard_SdFat;
};

void RepRap_FullGraphicsSmartController::Pulse_RotaryEncoder()
{
  if(RepRapEventHandler == NULL)
    return;
    
    short RotaryEncoder_ButtonCurrentState = digitalRead(RotaryEncoder_ButtonPin);
    if(RotaryEncoder_ButtonCurrentState == LOW) {
      RotaryEncoder_LongPressCounter++;
    }
    if(RotaryEncoder_ButtonState == HIGH && RotaryEncoder_ButtonCurrentState == LOW) {
    RotaryEncoder_ButtonState = RotaryEncoder_ButtonCurrentState;
    RepRapEventHandler(REPRAP_ROTARY_Pressed, true);
    } else if(RotaryEncoder_ButtonState == LOW && RotaryEncoder_ButtonCurrentState == HIGH) {
    RotaryEncoder_ButtonState = RotaryEncoder_ButtonCurrentState;
    if(RotaryEncoder_LongPressCounter > RotaryEncoder_LongPressThreshold) {
      RepRapEventHandler(REPRAP_ROTARY_LongReleased, true);
    } else {
      RepRapEventHandler(REPRAP_ROTARY_Released, true);
    }
    RotaryEncoder_LongPressCounter = 0;
    }
    
    short MostSignifigantBit = digitalRead(RotaryEncoder_Pin1);
    short LeastSignifigantBit = digitalRead(RotaryEncoder_Pin2);

    RotaryEncoder_CurrentState = (MostSignifigantBit << 1) | LeastSignifigantBit;

    int EncoderSum = (RotaryEncoder_LastState << 2) | RotaryEncoder_CurrentState;

    if(EncoderSum == 0b1101 || EncoderSum == 0b0100 || EncoderSum == 0b0010 || EncoderSum == 0b1011) 
    RotaryEncoder_StateCounter ++;
    if(EncoderSum == 0b1110 || EncoderSum == 0b0111 || EncoderSum == 0b0001 || EncoderSum == 0b1000) 
    RotaryEncoder_StateCounter --;

    RotaryEncoder_LastState = RotaryEncoder_CurrentState;

    if(RotaryEncoder_StateCounter > RotaryEncoder_TriggerThreshold) {
    RepRapEventHandler(REPRAP_ROTARY_Clockwise, RotaryEncoder_LastEventState != REPRAP_ROTARY_Clockwise);
    RotaryEncoder_StateCounter = 0;
    RotaryEncoder_CheckCounter = 0;
    RotaryEncoder_LastEventState = REPRAP_ROTARY_Clockwise;
    } else if(RotaryEncoder_StateCounter < (-1 * RotaryEncoder_TriggerThreshold)) {
    RepRapEventHandler(REPRAP_ROTARY_CounterClockwise, RotaryEncoder_LastEventState != REPRAP_ROTARY_CounterClockwise);
    RotaryEncoder_StateCounter = 0;
    RotaryEncoder_CheckCounter = 0;
    RotaryEncoder_LastEventState = REPRAP_ROTARY_CounterClockwise;
    } else {
    if(RotaryEncoder_CheckCounter > RotaryEncoder_TimeoutThreshold) {
      RepRapEventHandler(REPRAP_ROTARY_Stopped, RotaryEncoder_LastEventState != REPRAP_ROTARY_Stopped);
      RotaryEncoder_StateCounter = 0;
      RotaryEncoder_CheckCounter = 0;
      RotaryEncoder_LastEventState = REPRAP_ROTARY_Stopped;
    }
    RotaryEncoder_CheckCounter++;
    }
};

void RepRap_FullGraphicsSmartController::Pulse_StopButton()
{
  if(RepRapEventHandler == NULL)
    return;
    
  short StopButton_CurrentState = digitalRead(StopButton_Pin);
  if(StopButton_CurrentState == LOW) {
    StopButton_LongPressCounter++;
  }
  if(StopButton_State == HIGH && StopButton_CurrentState == LOW) {
    StopButton_State = StopButton_CurrentState;
    RepRapEventHandler(REPRAP_STOP_Pressed, true);
  } else if(StopButton_State == LOW && StopButton_CurrentState == HIGH) {
    StopButton_State = StopButton_CurrentState;
    if(StopButton_LongPressCounter > StopButton_LongPressThreshold) {
      RepRapEventHandler(REPRAP_STOP_LongReleased, true);
    } else {
      RepRapEventHandler(REPRAP_STOP_Released, true);
    }
    StopButton_LongPressCounter = 0;
  }
};

void RepRap_FullGraphicsSmartController::Pulse_Buzzer()
{
  if(Buzzer_State == BUZZER_STATE_OFF) {
    Buzzer_StateCounter++;
    if(Buzzer_StateCounter > Buzzer_Length) {
      Buzzer_Off();
    }
  }
};

void RepRap_FullGraphicsSmartController::Pulse_SDCard()
{
  short SDCard_CurrentDetectState = digitalRead(SDCard_DetectPin);
  if(SDCard_CurrentDetectState != SDCard_DetectState) {
    SDCard_DetectState = SDCard_CurrentDetectState;
    
    if(RepRapEventHandler != NULL) {
      if(SDCard_DetectState == RR_SD_CARD_PRESENT) {
        RepRapEventHandler(REPRAP_SD_INSERTED, true);
      } else {
        RepRapEventHandler(REPRAP_SD_REMOVED, true);
      }
    }
  }
}
