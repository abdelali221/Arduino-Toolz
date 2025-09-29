# Arduino-Toolz
 A Toolbox for the Arduino!

# Features :

 - Terminal (Using the LCD Display if you have one)
 - Analog/Digital PIN Tools (Will be removed since I'm working on a separate tool) 
 - Module Testers (IR/UltraSonic/DHT11/Keypad)
 - EEPROM Tool

 # How to Use :

Just upload the Project to your Arduino after downloading the required Libraries, then you'll be done!

 REQUIREMENTS :
 - An Arduino
 - A I2C LCD display (Optional)

The program uses commands that are sent using a Terminal (Not the Serial Monitor).
You should use a software like Putty or Tera Term.

# The commands : 

 - Terminal
 - Analog-Tool
 - Digital-Tool (Removed, check https://github.com/abdelali221/digital-toolz)
 - Sensor (DHT11) 
 - UltraR (For the UltraSonic Sensor)
 - LCD (LCD tools)
 - EEPROM (EEPROM tools)
 - Rave (pattern for LCD TEST)
 
# Changelog :

 - b 0.9 (fixed bugs/added features)
 - b 0.9.1 (the command system was remade, now it's more intuitive)
 - b 0.9.2 (fixed bugs/changed serialread stuff)
 - b 0.9.3 (major serial command system update)
 - b 0.9.4 (bug fixes/updated serial system)
 - 1.0 (First Stable Release)
 - 2.0 (Code was entirely rewritten)
 - 2.1 (Added UltraR to V2/Improved user-friendliness)
 - 2.2 (Major update : Decreased RAM usage to a THIRD of what it used to be/ Fused the Serial-Only with the LCD version)
 - 2.3 (Improved LCD displays compatibility/ Fixed some minor bugs)
 
# Notes :
	Please do not use Arduino-ToolzV1 nor Arduino-ToolzV2_OLD since they are outdated and/or broken, Use ArduinoToolzV2 instead.
