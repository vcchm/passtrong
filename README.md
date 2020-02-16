# PasStrong bluetooth password strength dongle

The PassStrong BlueTooth Dongle uses a 14$ Esp32 dongle to generate, securely store (with limitations, see hereunder) and types strong passwords.

# 0) Hardware

Uses an ESP32-based M5StickC dongle (https://www.aliexpress.com/wholesale?catId=0&initiative_id=SB_20200216013739&SearchText=m5stickc ). You will need to install M5StickC libraries, as well as BLEkeybaord library (i used a modified version that allows you to change the kind of device used, but you can go without it).

# 1) What it does

After connection to your PC/mobile phone (as a bluetooth keyboard), enters a long and complex password impossible to remember but stronger than your 8 letters password.

The dongle is protected by a pincode (length is set to 4 digits, but can be augmented), and the password is stored crypted in the EEPROM using a key provided in the code (quite easy

The installation is quite straightforward if you are used to arduino & esp32. if not, just google for examples, there are many of them on the internet.

# 2) Instructions

Based on M5Stick ESP32 dongle (includes a screen and a battery).

a) Settings

The large button is the "mode" button. The small button is the "action" button
When you power the M5Stick, it tries to connect to a bluetooth device and shows the "Initializing" screen until connected to a bluetooth device.
When it is connected you see the main screen with 3 locks.

  i) Press the "action" button for the M5Stick to type the password (you will see a "keyboard" screen while it does so). If the pincode is not correct, nothing will happen.
  ii) Press the "mode" button once to enter the settings menu
    Pin setting : The ">" sign should be before "Pin:"
     - press the "action" button to increase each digit of the pincode  
	 - press the "mode" button to move to the next digit, and once more to move to next section
    Pin changing : The ">" sign should be before "Change:"
	 - If the correct pin has been entered in normal mode, pressing the "action" button enters the "new pin" mode : you should go back to the preceding setting to enter a new pin code
	 - if a pin was entered and he device was already in the "new pin" mode (or if it is the first time you set the pin) pressing "action" will setup the new pin and move to the next setting
	Keyboard language setting:
     - Press "action" to shift from aazrty to qwerty and back
	Password regenerating
	 - Press "action" twice to erase old password and to output the old password and to generate a new one. Nb : after you do this, there is no way to restore the old password
	 - Press mode again to come back to the main menu

# 3) Security concerns and future developments

In case you lose the pincode, you can wipe the Eprom and reuse the device using the sketch provided in the Utils directory.

In a next version, I will implement a pincode on the bluetooth connection. The password is stored in the EEPROM, but crypted using AES (thanks to CNLohr for the nice routine) and a 32 bytes key you provide in the code (it is recommended to flash a different key with all the versions of the key - else it is easy to retrieve the crypted password from the EEPROM by flashing a new version of the software, then to decrypt it).

If the dongle is found, it is possible to retrieve the key by accessing the flasmemory of the chip, and then to decrypt the password which is stored in the EEPROM, but it requires some hardware expertise.

If the dongle is found unlit, then the pincode is needed to unlock the key. 
