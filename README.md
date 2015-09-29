# WookieController

Tiny project to play around with Chibios on a STM32F4 Discovery board

# LCD testing variant
Using SSD1803A displays to communicate with.

## Hardware

* PC / Raspberry to run boblight e.g. in combination with [Kodi](kodi.tv)
* STM32F4 Discovery board

## Interface
On the STM32F4 the following GPIOs are used:
* PB12 is NSS
* PB13 is SCK
* PB14 is MISO
* PB15 is MOSI
* LED interface
  * Green LED blinking : Board running
