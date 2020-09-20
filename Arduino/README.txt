There are (currently) 2 arduino versions of the Deej controller:-Deej_controller_EEPROM
                                                                -Deej_controller_no_EEPROM

The code is almost the same for the 2 versions the only difference is the use of EEPROM.
I've added EEPROM since I wanted when on reboot the Deej controller uses the value's from last time, so when you put an slider down,
the audio doesn't become insanely loud. The EEPROM is some sort of storage that doesn't lose it's data when the power drops down.
The only drawback of the feature is that the arduino's EEPROM has about 100 000 write cycle and from than it becomes unstable.
I've wrote the code in the way so the slider values only get update when you go back to the menu and the value has changed compared
to previous values. There are also I²C EEPROM modules where you can change the EEPROM IC when broken. I might add support for those in the future.
Note that I am not responsible for broken EEPROM or other broken hardware.
