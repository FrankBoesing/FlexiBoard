# FlexiBoard
Board for Teensy 3.2, 3.5, 3.6

[[https://github.com/FrankBoesing/FlexiBoard/tree/master/pictures/board.png]]

# FLASH ESP FIRMWARE
## download
 
### Flash size 32Mbit: 512KB+512KB
    boot_v1.2+.bin              0x00000
    user1.1024.new.2.bin        0x01000
    esp_init_data_default.bin   0x3fc000 (optional)
    blank.bin                   0x7e000 & 0x3fe000


# Update steps
1.Make sure TE(terminal equipment) is in sta or sta+ap mode

    AT+CWMODE=3
    OK

2.Make sure TE got ip address

    AT+CWJAP="ssid","12345678"
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

