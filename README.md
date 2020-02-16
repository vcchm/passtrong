# PassStrong bluetooth password protection

The PassStrong BlueTooth Dongle uses a 14$ Esp32 dongle to generate, securely store (with limitations, see hereunder) and types strong passwords.

# 0) Hardware

Uses an ESP32-based M5StickC dongle (https://www.aliexpress.com/wholesale?catId=0&initiative_id=SB_20200216013739&SearchText=m5stickc ). You will need to install M5StickC libraries, as well as BLEkeybaord library (i used a modified version that allows you to change the kind of device used, but you can go without it).

# 1) What it does

After connection to your PC/mobile phone (as a bluetooth keyboard), enters a long and complex password impossible to remember but stronger than your 8 letters password.

The dongle is protected by a pincode (length is set to 4 digits, but can be augmented), and the password is stored crypted in the EEPROM using a key provided in the code (quite easy

The installation is quite straightforward if you are used to arduino & esp32. if not, just google for examples, there are many of them on the internet.

# 2) Instructions

For the first setup or after the Eprom is wiped using the code provided in the Utils directory, you will need to define the pincode and to regenerate the password.

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
	 - Press "action" twice to erase old password.
	This will output the old password and to generate a new one. Nb : after this, there is no way to restore the old password. You should note it somewhere if you need to change your password on a site
	 - Press mode again to come back to the main menu

# 3) Security concerns and future developments

In case you lose the pincode, you can wipe the Eprom and reuse the device using the sketch provided in the Utils directory.

As this is more a proof of concept than a finished product, there are a couple of security limitations:
   -in a next version, I will implement a pincode on the bluetooth connection
   -the password is stored in the EEPROM, crypted using AES (thanks to CNLohr for the nice routine) and a 32 bytes generated using AFACTOR and BFACTOR provided in the code. It is strongly recommended to use different values for theses parameters for each of the dongles. If you do not do it, it is easy to retrieve the crypted password from the EEPROM by flashing a new version of the software, then to decrypt it
   -if the dongle is found, it is possible to retrieve the key by accessing the flashmemory of the chip, and then to decrypt the password which is stored in the EEPROM, but it requires some hardware skills.
 
 If the dongle is found unlit, then the pincode is needed to unlock the key. 
