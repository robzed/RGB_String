# RGB_String

A time of day display for an RGB string (that uses three analogue voltages to set the colour).

Assumes a RTC PCF8563, although code would be easy enough to change to use with another clock module, 
since it uses the Adafruit RTC module.

# Overview

Arduino Project to change a short LED RGB string, based on a RGB chain that has three analog 
outputs (one for each red line, green line, and blue line).

It adjusts based on time of day and day of the week.

# Serial Commands

115200 8N1 USB - press a key as follows:

1-7 = forces an override for the mode (to check colours)
Escape key - cancel override 
+ = advances the RTC by 1 minute
- = decrements the RTC by 1 minute


# LICENSE

(c) 2022 Rob Probin

Licensed under the MIT License

