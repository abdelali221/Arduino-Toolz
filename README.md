# Arduino-Toolz
 A Toolbox for the Arduino!

# Features :

 - Clock (Not available on V2)
 - Terminal
 - Analog/Digital PIN Tools
 - Module Testers (IR/UltraSonic/DHT11/Keypad) (Some are not available on V2, and/or broken on V1)
 - EEPROM Tool
 - Hot-Plug a LCD Display

 # How to Use :

Just upload the Project to your Arduino after downloading the required Libraries, then you'll be done!

 REQUIREMENTS :
 - An Arduino (This program was tested with an Arduino UNO)
 - A I2C LCD display (Needed)

After uploading the project you should get "Waiting for a command" on the LCD. The program is controllable via Serial with commands. (V1)
On V2 otherwise, you'll get this : "$>" on both the lcd screen and the terminal, it's the command prompt.

# The commands : 

 - Terminal
 - Clock (Not available on V2)
 - Analog
 - Digital
 - IR (Not available on V2)
 - Sensor (For DHT11) 
 - UltraR (For the UltraSonic Sensor)
 - Keypad (Not available on V2)
 - LCD (LCD tools) (Not available in V1)
 - EEPROM tool
 - Rave (pattern for LCD TEST) (Not available on V1)
 - Help
 
# Changelog :

 - b 0.9 (fixed bugs/added features)
 - b 0.9.1 (the command system was remade, now it's more intuitive)
 - b 0.9.2 (fixed bugs/changed serialread stuff)
 - b 0.9.3 (major serial command system update)
 - b 0.9.4 (bug fixes/updated serial system)
 - 1.0 (FIRST RELEASE! ADDED CONTENT/FIXED BUGS/OPTIMIZATION)
 - 2.0 (MAJOR UPDATE : ENTIRELY REWRITTEN/MASSIVE IMPROVEMENTS)
 - 2.1 (added UltraR to V2/changed mind about other functions because of Dynamic Memory limitations/improved user-friendliness)
 
# Notes :

	DON'T USE THE Arduino-ToolzV1 AS IT'S BUGGY AND DEPRECATED NOW, Use ArduinoToolzV2 instead.
