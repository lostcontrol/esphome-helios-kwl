# ESPHome / Helios KWL

ESPHome integration for Helios KWL via RS485.

![ESP32 BEC RS485](docs/esp32-bec-rs485.jpg)

Of course, it all makes sense if integrated with the amazing Home Assistant:

![In Home Assistant](docs/hs-helios-kwl.png)

## Introduction

I've had a Helios KWL EC 500 R since 2012. Back then, I looked into getting the KNX module to hook it up to my home automation, but the price was ridiculous. So, after all this time, I decided to just do it myself. I got an RS485/UART adapter, sniffed the data packets, and actually found the protocol documentation online. I'm powering an ESP32 from the KWL's 24V using a BEC.

Newer Helios units have Ethernet built-in, making integration much easier. But if you've got an older model, this is for you.

I've only programmed the features I use and need, but you could definitely add more.

## Wiring diagram

![Wiring diagram](docs/wiring.png)

## Installation

Check out the `helios-kwl.yaml` file. This is an ESPHome [external component](https://esphome.io/components/external_components) you can easily add to your setup. The best way to get it is using Git:

```yaml
external_components:
  - source:
      type: git
      url: https://github.com/lostcontrol/esphome-helios-kwl/
      ref: 2025.3.1
```

## Disclaimer

As usual, I decline all responsibility if you break your Helios KWL (or anything else actually).

## Shopping list

These are examples of items required for this project. You will also need (silicone) wires and heat shrink tubing.

* RS485 adapter: https://www.aliexpress.com/item/32889414931.html
* Wemos D1 Mini ESP32: https://www.aliexpress.com/item/32815738795.html (select D1 MINI ESP32)
* BEC 5V (input 24V): https://www.aliexpress.com/item/1865193146.html

## Links

* Example datagrams: [helios-kwl.txt](docs/helios-kwl.txt)
* Protocol: https://wiki.fhem.de/w/images/7/7e/Digit_protocol_english_RS485.pdf
