# RepRapSmartController
Library to allow basic access to RepRap Full Graphic Smart Controller via the RAMPS V1.4 Board.

This was built as I wanted to use the controller, but not for building a 3D printer, but rather to build other projects requiring raw access to the components.

This library is an event driven system, where the following events are captured:

Stop Button Pressed
Stop Button Short Released (Short Push)
Stop Button Long Released (Long Push)

Encoder Direction Clockwise
Encoder Direction Counter Clockwise
Encoder Direction Stopped
Encoder Button Pressed
Encoder Button Short Released (Short Push)
Encoder Button Long Released (Long Push)

Buzzer On
Buzzer Off

SD Card Inserted
SD Card Removed

Currently, the following methods can be used:

Buzzer On (Turn on buzzer, leave it on)
Buzzer Off (Turn off buzzer, regardless of any beeps in process)
Short Beep
Long Beep

Get U8G2 LCD Object
Get SdFat Card Reader Object
