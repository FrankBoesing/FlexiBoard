#Errata:
- Wrong location for Teensy 3.1/3.2 VBAT/DAC0 Pins (still possible to use them with a bit fiddling.. or use Teesy 3.5/3.6)
- Due to an error, pins 24-32 are not routed to the 48-Pin-connector :-( FIX: Patch them on the backside.
- Limited IR-Send power, but works for distances ~2m. For more power, add a (extern) transistor.
- The connector for the IR-Receiver is + - OUT, but most IR -Receivers use - + OUT Fix: Do not mount directly and use a cable
