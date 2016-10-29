
Minimal Internetradio
===

This is a minimal webradio. It is in no way complete.
In can play 44.1KHz - MP3 radio streams.

It works best, if you add (min) 2 pcs 23LC1024 RAMS (or more).
But i can be run with the Teensy 3.5/3.6 RAM only, too. 
In this case, 128KB internal RAM are used. This is a bit too less, so 
expect some problems, if your internet-connection is not good.

Currently, it connects to a German station - you may want to edit that, and use
a local station for a better connection.


Setup
===

For a better experience, edit _Serial1.c_ from the Teensy-core and set 
RX_BUFFER_SIZE     2048


Use the Example "TalkToESP" to setup the ESP:

First, restore the factory-settings:
- AT+RESTORE

Set station mode:
- AT+CWMODE_DEF=1

Store the connection to your WLAN
- AT+CWJAP_DEF ="SSID", "PASSWORD"

Additional libraries needed: 
https://github.com/FrankBoesing/Teensy-MP3

