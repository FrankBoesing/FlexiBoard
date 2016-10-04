# FlexiBoard
##Board for Teensy 3.2, 3.5, 3.6

https://forum.pjrc.com/threads/36869-T3-x-Flexiboard(-)?p=117415#post117415

Link to board @ PCBs.io: https://pcbs.io/search?query=flexiboard

### Some Pictures
![](pictures/top.svg.m.png?raw=true)
![](pictures/bottom.svg.m.png?raw=true)
![](pictures/board.png?raw=true)
![](pictures/spreadsheet.png?raw=true)
![](pictures/schematic.png?raw=true)

## FLASH ESP FIRMWARE

### Flash size 32Mbit: 512KB+512KB
    boot_v1.2+.bin              0x00000
    user1.1024.new.2.bin        0x01000
    esp_init_data_default.bin   0x3fc000 (optional)
    blank.bin                   0x7e000 & 0x3fe000


## Flash Firmware

1. Extract the Firmware and Flash-Tool in extras/ESP8266/Firmware.zip

2. Start ESP8266Flasher.exe and configure it :

![](extras/ESP8266/pictures/flash_step1.png?raw=true)

![](extras/ESP8266/pictures/flash_step2.png?raw=true)

- Load FlexiBoard_FlashESP8266.ino to the Teensy

Select the COM-Port:
![](extras/ESP8266/pictures/flash_step3.png?raw=true)

- Reset the Teensy and click the "Flash" button

![](extras/ESP8266/pictures/flash_step4.png?raw=true)


After that, you can run FlexiBoard_TalkToESP8266.ino and enter the following command into the console to update the ESP8266-Firmware:

## Update Firmware
1.Make sure TE(terminal equipment) is in sta or sta+ap mode

    AT+CWMODE=3
    OK

2.Make sure TE got ip address

    AT+CWJAP="YourSSID","YourWLANPassword"
    OK
    
    AT+CIFSR
    192.168.1.134

3.Let's update

    AT+CIUPDATE
    +CIPUPDATE:1    found server
    +CIPUPDATE:2    connect server
    +CIPUPDATE:3    got edition
    +CIPUPDATE:4    start start
    
    OK

