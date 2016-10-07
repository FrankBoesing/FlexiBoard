
Posted details here from :: https://forum.pjrc.com/threads/36869-T3-x-Flexiboard(-)?p=117701&viewfull=1#post117701

HOW TO GET ESP PROGRAMMED OTA WITH A very functional WEBSERVER THROUGH ARDUINO::
I'd say start with IDE 1.6.12 it works for me.

Get Server code from here: https://github.com/nailbuster/myWebServer
It says to get these two things to your sketchbook/libraries folder:
- TimeLib for ntp here: https://github.com/PaulStoffregen/Time
- Arduino Json here: https://github.com/bblanchon/ArduinoJson 

Get the ESP Arduino code from here: https://github.com/esp8266/Arduino

Use Boards Manager and the JSON from there: 
Boards manager link: http://arduino.esp8266.com/stable/pa...com_index.json 

By default OTA is OFF! - it won't show this used "ArduinoOTA"
Find this code on your machine where you put it in libraries and uncomment this line >> //#define ALLOW_IDEOTA
https://github.com/nailbuster/myWebServer/blob/master/myWebServer.h#L48
- OTA is "unsafe" so it is off by default :: - [there is a new scheme to password protect OTA updates - otherwise anyone who can see the WiFi name can program it.] :: https://github.com/esp8266/Arduino/blob/master/doc/ota_updates/readme.md#password-protection
- OTA requires PYTHON_2.7 to be installed ON THE PATH :: https://github.com/esp8266/Arduino/blob/master/doc/ota_updates/readme.md#application-example


If you do that and compile "sketchbook...\libraries\myWebServer\examples\myWe bServerSample" it should show those libraries like this::

- Using library ESP8266WiFi at version 1.0 in folder: C:\Users\YOU\AppData\Local\Arduino15\packages\esp8 266\hardware\esp8266\2.3.0\libraries\ESP8266WiFi 
- Using library ESP8266WebServer at version 1.0 in folder: C:\Users\YOU\AppData\Local\Arduino15\packages\esp8 266\hardware\esp8266\2.3.0\libraries\ESP8266WebSer ver 
- Using library ESP8266mDNS in folder: C:\Users\YOU\AppData\Local\Arduino15\packages\esp8 266\hardware\esp8266\2.3.0\libraries\ESP8266mDNS (legacy)
- Using library DNSServer at version 1.1.0 in folder: C:\Users\YOU\AppData\Local\Arduino15\packages\esp8 266\hardware\esp8266\2.3.0\libraries\DNSServer 
- Using library ArduinoOTA at version 1.0 in folder: C:\Users\YOU\AppData\Local\Arduino15\packages\esp8 266\hardware\esp8266\2.3.0\libraries\ArduinoOTA 
- Using library ArduinoJson at version 5.1.0 in folder: i:\tcode\libraries\ArduinoJson 
- Using library myWebServer at version 1.0.1 in folder: i:\tcode\libraries\myWebServer 
- Using library Time at version 1.5 in folder: i:\tcode\libraries\Time 

The first time upload you'll have to push over serially of course. 
From then on - if you don't add faulting code you can do OTA updates about 5 times faster than Serial. As noted I set up an ESP read pin that is set to indicate ABORT USER CODE (by Teensy or wire) and just allow ther server code to run and do OTA. It is easy to touch something (IN ERROR) and HANG an ESP because it is doing DISK and WIFI stuff - if you repower with that bit set - it will wait for a fresh OTA upload. I think this is on my GitHub too. << I could post my sketches but they are hacked to accommodate 4 different ESP's and their different configurations to the Teensy/onehorse_FC_3.2 units - after 6 months I don't under stand them again myself yet. And they would not match the Flexiboard - but I might post something as it was quite fun and usable.>>

>> For starters I would suggest using 4MB with 3 MB SPIFFS - 1MB for code is good, 3MB for SPIFFS is better. My sketch is 311KB. Using of near HALF of 1MB still allows OTA FLASH - you can change later if you want that much code. This WEBSERVER library runs all HTML pages for the server from SPIFFS - You can Read Write other data there too when you don't need to put it in FLASH - the upload is smaller faster.


Get your Server IP address and his :: 192.168.1. _??_ /wifisetup.html, there is a .gz file there it runs from, you can NAME the device and that will show on the IDE then.
<<Yes the server has code to perform GZ decompress to some degree on files to run them! Details on the server github - I asked a question in an ISSUE>>


- WARNING - WARNING - WARNING !!!!
- I just wiped my 3MB SPIFFS DATA - when you enter the IDE ALWAYS CHECK the TOOLS :: SPIFFS size and other params - the IDE will often not remember some of those CRITICAL things properly across board changes!!!!!
- I just found this out the hard way for the first time - so that lets me show the details above because my ESP is TRASHED - 512K Code and 64K SPIFFS is the DEFAULT !!!!!
- WARNING - WARNING - WARNING !!!!
So I lost my SPIFFS data - my network configuration setting and I cannot OTA upload because 512MB cannot fit two copies of the 311K sketch !!!!
I'll have to get the details right to T_3.2 proxy upload again from my hacked sketch ...


ONE MORE MISSING PIECE :: 
- You can just transfer files over FTP once the server is running - THAT IS WAY COOL!!
- You may want SPIFFS IMAGE data on your device 
My github refers to this :: you make a folder image and then upload the data to it. 
https://github.com/esp8266/arduino-esp8266fs-plugin/releases/tag/0.2.0
