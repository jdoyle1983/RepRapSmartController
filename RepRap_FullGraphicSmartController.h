#ifndef REPRAPFULLGRAPHICSMARTCONTROLLER_H
#define REPRAPFULLGRAPHICSMARTCONTROLLER_H

/* 
 *  These are set to the defaults
 *  RAMPS V1.4
 *  RepRapDiscount / RepRap
 *  Full Graphic Smart Controller
 */

#ifndef RR_CFG_Encoder_DirectionPin1
  #define RR_CFG_Encoder_DirectionPin1 31
#endif

#ifndef RR_CFG_Encoder_DirectionPin2
  #define RR_CFG_Encoder_DirectionPin2 33
#endif

#ifndef RR_CFG_Encoder_ButtonPin
  #define RR_CFG_Encoder_ButtonPin 35
#endif


#ifndef RR_CFG_Encoder_Threshold
  #define RR_CFG_Encoder_Threshold 2
#endif

#ifndef RR_CFG_Encoder_Timeout
  #define RR_CFG_Encoder_Timeout 5000
#endif

#ifndef RR_CFG_Encoder_LongThreshold
  #define RR_CFG_Encoder_LongThreshold 30000
#endif


#ifndef RR_CFG_StopButton_Pin
  #define RR_CFG_StopButton_Pin 41
#endif

#ifndef RR_CFG_StopButton_LongThreshold
  #define RR_CFG_StopButton_LongThreshold 30000
#endif


#ifndef RR_CFG_Buzzer_Pin
  #define RR_CFG_Buzzer_Pin 37
#endif

#ifndef RR_CFG_Buzzer_ShortLength
  #define RR_CFG_Buzzer_ShortLength 1000
#endif

#ifndef RR_CFG_Buzzer_LongLength
  #define RR_CFG_Buzzer_LongLength 5000
#endif


#ifndef RR_CFG_Sd_CsPin
  #define RR_CFG_Sd_CsPin 53
#endif

#ifndef RR_CFG_Sd_DetectPin
  #define RR_CFG_Sd_DetectPin 49
#endif


#ifndef RR_CFG_Lcd_ClkPin
  #define RR_CFG_Lcd_ClkPin 23
#endif

#ifndef RR_CFG_Lcd_DataPin
  #define RR_CFG_Lcd_DataPin 17
#endif

#ifndef RR_CFG_Lcd_CsPin
  #define RR_CFG_Lcd_CsPin 16
#endif

#define REPRAP_ROTARY_Stopped 0
#define REPRAP_ROTARY_Clockwise 1
#define REPRAP_ROTARY_CounterClockwise 2
#define REPRAP_ROTARY_Pressed 3
#define REPRAP_ROTARY_Released 4
#define REPRAP_ROTARY_LongReleased 5
#define REPRAP_STOP_Pressed 6
#define REPRAP_STOP_Released 7
#define REPRAP_STOP_LongReleased 8
#define REPRAP_SD_INSERTED 9
#define REPRAP_SD_REMOVED 10
#define REPRAP_BUZZER_ON 11
#define REPRAP_BUZZER_OFF 12

#define RR_SD_CARD_PRESENT LOW
#define RR_SD_CARD_NOTPRESENT HIGH

#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>
#include <SdFat.h>

class RepRap_FullGraphicsSmartController
{
  public:
    RepRap_FullGraphicsSmartController(void (* repRapEventHandler)(short repRapEvent, bool hasChanged));
    void Pulse();
    
    void Buzzer_On();
    void Buzzer_Off();
    void Buzzer_Beep(short Buzzer_BeepTime);
    void Buzzer_ShortBeep();
    void Buzzer_LongBeep();
    
    U8G2_ST7920_128X64_1_SW_SPI* GetDisplay();
    
    short GetSdCardState();
    SdFat* GetSdFat();
    
  private:
  
    void (* RepRapEventHandler)(short repRapEvent, bool hasChanged);
  
    short RotaryEncoder_Pin1;
    short RotaryEncoder_Pin2;
    short RotaryEncoder_ButtonPin;
    short RotaryEncoder_ButtonState;
    long RotaryEncoder_TriggerThreshold;
    long RotaryEncoder_TimeoutThreshold;
    short RotaryEncoder_CurrentState;
    short RotaryEncoder_LastState;
    short RotaryEncoder_LastEventState;
    long RotaryEncoder_StateCounter;
    long RotaryEncoder_CheckCounter;
    long RotaryEncoder_LongPressThreshold;
    long RotaryEncoder_LongPressCounter;
    void Pulse_RotaryEncoder();
    
    short StopButton_Pin;
    short StopButton_State;
    long StopButton_LongPressThreshold;
    long StopButton_LongPressCounter;
    void Pulse_StopButton();
    
    short Buzzer_Pin;
    short Buzzer_Length;
    short Buzzer_State;
    short Buzzer_StateCounter;
    void Pulse_Buzzer();
    
    U8G2_ST7920_128X64_1_SW_SPI* LCD_Display;
    
    short SDCard_CsPin;
    short SDCard_DetectPin;
    short SDCard_DetectState;
    void Pulse_SDCard();
    SdFat* SDCard_SdFat;
};

#endif
