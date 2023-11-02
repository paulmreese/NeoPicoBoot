<img src="/assets/NeoPicoBoot.png" alt="NeoPicoBoot Logo. A cubic raspberry logo with an antenna glowing red, green and blue" align="left"/>


# NeoPicoBoot: NeoPixel + PicoBoot
This is an IPL replacement modchip for Nintendo GameCube. It's also a WiFi Access Point hosting responsive RGB control. It's open source, cheap, and easy to install.

## Features
* open source, easy to install
* uses $6 Raspberry Pi Pico W board
* RGB control for your controller ports and power LED
* easy-to-access NeoPixel web controls via Captive Portal
* boots any DOL app of your choice

<p align="center">
  <img src="/assets/RGB_Ports_and_Power_small.gif" alt="A zoomed-in view of a modded GameCube cycling a rainbow of colors in from the controller ports and power LED"/>
</p>

## Installation guide

The complete installation guide has been moved to [a separate document](/INSTALLATION.md).

Other than the NeoPixels being added, and **3V3** being swapped for **VSYS** from the 5V rail of the controller ports, the wiring works exactly like the original. So, if you need any additional help you can head over to [the PicoBoot wiki](https://github.com/webhdx/PicoBoot/wiki) for [the PicoBoot Installation guide](../../wiki/Installation-guide).

Additionally, you can check the PicoBoot Discord Server to get support and discuss new features:

[![](https://dcbadge.vercel.app/api/server/fEhyWRPCmb)](https://click.webhdx.dev/discord)

## 3D Printed Parts
Printing these `STL` files makes for a much simpler installation process, but neither is necessary.

* [PicoBoot Mount for the Nintendo Gamecube](https://www.thingiverse.com/thing:5482506) - This mounts the Pico W onto the controller port board. While other mounts exist, this one is convenient for 2 reasons:
    1. The wires can exit the motherboard towards the controller ports, without deforming the RF shielding.
    2. The NeoPixels can be closer to the Pico W itself, with shorter wires.
* [NeoPixel alignment bracket](/assets/NeoPixel_Bracket.stl) - This bracket helps to align all of the controller port NeoPixels with the correct spacing. It will only install below the controller ports, if you have a normally installed battery. This can be printed in PLA and hot-glued into place.

## Video guides and features overview

- [PicoBoot Modchip Will Unleash The POWER of Your Nintendo GAMECUBE! | Installation Guide and Overview](https://www.youtube.com/watch?v=qwL4ZSa0xMo) by [MachoNachoProductions](https://www.youtube.com/c/MachoNachoProductions)
- [This new Gamecube Modchip is a GAMECHANGER - PicoBoot](https://www.youtube.com/watch?v=lfMTLEM0yeQ) by [RockerGaming](https://www.youtube.com/c/RockerGaming)
- [$5 Gamecube IPL Modchip?! Picoboot Dol-001 + Dol-101 Installation / Setup / Showcase](https://www.youtube.com/watch?v=W_9-mSBMBJ4) by [ModzvilleUSA!](https://www.youtube.com/c/ModzvilleUSA)
- [PicoBoot GameCube custom mod chip - make and install your own chip with a Raspberry Pi Pico](https://youtu.be/rDrosSd-nDc) by [Joe Bleeps](https://www.youtube.com/@JoeBleeps)
- [RGB LED Mod Tutorial/ How to/ guide - Gamecube, Saturn, N64, SNES, and More!](https://youtu.be/1XUOI1c_viQ?si=Boch__JV7q4JWGpH) - This is a general RGB installation guide, but it does include GameCube

## Acknowledgements

Some parts of this project use GPL-2.0 licensed code from:
 * https://github.com/webhdx/PicoBoot
 * https://github.com/redolution/iplboot

Some parts of this project use MIT licensed code from:
 * https://github.com/sysprogs/PicoHTTPServer

Some parts of this project use LGPL-3.0 licensed code from:
 * https://github.com/martinkooij/pi-pico-adafruit-neopixels
 * https://github.com/adafruit/Adafruit_NeoPixel

## Hall of Fame

These are the people who made PicoBoot possible:
* [webhdx](https://github.com/webhdx) - All credit for the [PicoBoot](https://github.com/webhdx/PicoBoot) goes here. Consider being their [Sponsor](https://github.com/sponsors/webhdx)

* #gc-forever crew: [Extrems](https://github.com/Extrems), [novenary](https://github.com/9ary), [emu_kidid](https://github.com/emukidid) and others
* [tmbinc](https://github.com/tmbinc) - he started it all 🙏
* happy_bunny - [webhdx] started [webhdx's] research with his great writeup on [Shuriken Attack](https://www.retro-system.com/shuriken_attack.htm)
* PicoBoot beta testers: [seewood](https://github.com/seewood), [MethodOrMadness](https://github.com/MethodOrMadness), [renanbianchi](https://github.com/renanbianchi)
* content creators: [MachoNachoProductions](https://www.youtube.com/c/MachoNachoProductions), [RockerGaming](https://www.youtube.com/c/RockerGaming), [ModzvilleUSA!](https://www.youtube.com/c/ModzvilleUSA)
* people who sponsored [PicoBoot](https://github.com/webhdx/PicoBoot)
* every PicoBoot enjoyer - it's all about you after all 😉
