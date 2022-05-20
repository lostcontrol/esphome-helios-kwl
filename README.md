# ESPHome / Helios KWL

ESPHome integration for Helios KWL via RS485.

![ESP32 BEC RS485](docs/esp32-bec-rs485.jpg)

Of course, it all makes sense if integrated with the amazing Home Assistant:

![In Home Assistant](docs/hs-helios-kwl.png)

## Introduction

I own since 2012 a Helios KWL EC 500 R. Back then, I checked for the KNX module that would allow me
to integrate the device into my home automation bus. However, the price was just crazy high. So
after so many years, I finally decided to hack it myself. I ordered a RS485 / UART adapter, sniffed
a few packets and actually found the documentation of the protocol online. I'm using a BEC to power
the ESP32 from the 24V of the KWL.

Newer version of the Helios product now have directly an Ethernet port and can be integrated more
easily. However, if you own an older model, this integration is for you.

I only implemented the features I use and find useful but more could be added.

## Links

* RS485 adapter: https://www.aliexpress.com/item/32889414931.html
* Example datagrams: [helios-kwl.txt](docs/helios-kwl.txt)
* Protocol: https://wiki.fhem.de/w/images/7/7e/Digit_protocol_english_RS485.pdf
