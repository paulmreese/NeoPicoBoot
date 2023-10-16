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
  <img src="/assets/RGB_Ports_and_Power_small.gif" alt="A zoomed-in view of a modded GameCube cycling a rainbow of colors in from the controller ports and power LED">
</p>

## Installation guide

### Pico W
I've created a [visual installation guide](/assets/NeoPicoBoot%20Wiring%20diagram.jpg) to show the default wiring. This should be enough information to guide someone through a basic installation. This wiring incorporates [a method mentioned in an issue on the original PicoBoot Repo](https://github.com/webhdx/PicoBoot/issues/75#issuecomment-1539225503), which wires power into the **VSYS** pin of the Pico W instead of the **3V3** pin.

According to the [Raspberry Pi Pico W Datasheet](https://datasheets.raspberrypi.com/picow/pico-w-datasheet.pdf):
> VSYS is the main system input voltage, which can vary in the allowed range 1.8V to 5.5V, and is used by the on-board SMPS to generate the 3.3V for the RP2040 and its GPIO.

> 3V3 is the main 3.3V supply to RP2040 and its I/O, generated by the on-board SMPS. This pin can be used to power external circuitry (maximum output current will depend on RP2040 load and VSYS voltage; it is recommended to keep the load on this pin under 300mA).

### RGB
As for the NeoPixels(WS2812, etc), this [video shows exactly how to mod your controller port board to add the NeoPixels](https://youtu.be/1XUOI1c_viQ?si=Boch__JV7q4JWGpH)
<p align="center">
  <img src="/assets/NeoPixel_Locations_Highlighted.jpg" alt="View of the GameCube controller port PCB from below, with the center retaining pins removed(highlighted in red)."/>
  <i>GameCube controller ports from below, with retaining pins removed</i>
</p>

* [1:04](https://youtu.be/1XUOI1c_viQ?si=35ieH2EPR0lPXH_Q&t=64) - Remove the pins from each port. These pins can be removed from **either** the top *or* the bottom of each controller port. These LEDs can be very bright, so choose based on where you keep your GameCube. I personally use tweezers to snap the bottom ones so that I'm not competing with the battery for space.
* [3:25](https://youtu.be/1XUOI1c_viQ?si=mzvSUIralLa4HMga&t=205) - Create a strand of 5 NeoPixels. Make sure to leave a little bit of slack in the wires between each NeoPixel, especially to the power led and Pico W. Check continuity on the positive and negative wires regularly!
<p align="center">
  <img src="/assets/LED_and_Battery_Controller_Board.jpg" alt="View of the GameCube controller port PCB from the back, highlighting the pins of the power LED and Battery"/>
  <i>Battery and power LED solder points</i>
</p>

* [5:50](https://youtu.be/1XUOI1c_viQ?si=wtnrG3PIRv7RvrTk&t=350) - Remove stock LED. Optionally, also replace CR2032 Clock Battery. I would highly recommend replacing this battery with a separate battery holder and fresh battery.
* [6:25](https://youtu.be/1XUOI1c_viQ?si=p0bYnIFZQqc9hqtQ&t=385) - Hot glue controller port NeoPixels into place. Make sure they all point the same angle! You can remove the hot glue with rubbing alcohol. I've included [a 3D model of a basic alignment bracket as an STL file](/assets/NeoPixel_Bracket.stl) if you have access to a 3D printer. This bracket will only install below the controller ports, if you have a normally installed battery.
<p align="center">
  <img src="/assets/Finished_Controller_Board.jpg" alt="View of the GameCube controller port PCB from the front, with the sanded/filed-away area highlighted. A new battery holder with a new battery is shown."/>
  <i>The highlighted area has been sanded away. Note the new battery holder</i>
</p>

* [8:00](https://youtu.be/1XUOI1c_viQ?si=zBVIya90WfhzKUA-&t=480) - Hot glue Power NeoPixel. In my experience, you often need to seat the power LED lower than shown in the video for it to align correctly with the original light pipe. I recommend seating to the bottom of the notch on the PCB. This can sometimes involve sanding the PCB to the side to fit a NeoPixel.


Other than the NeoPixels being added, and **3V3** being swapped for **VSYS** from the 5V rail of the controller ports, the wiring works exactly like the original. So, if you need any additional help you can head over to [the PicoBoot wiki](https://github.com/webhdx/PicoBoot/wiki) for [the PicoBoot Installation guide](../../wiki/Installation-guide).

Additionally, you can check the PicoBoot Discord Server to get support and discuss new features:

[![](https://dcbadge.vercel.app/api/server/fEhyWRPCmb)](https://click.webhdx.dev/discord)

## 3D Printing
Printing these `STL` files makes for a much simpler installation process, but neither is necessary.

* [PicoBoot Mount for the Nintendo Gamecube](https://www.thingiverse.com/thing:5482506) - This mounts the Pico W onto the controller port board. While other mounts exist, this one is convenient for 2 reasons:
    1. The wires can exit the motherboard towards the controller ports, without deforming the RF shielding.
    2. The NeoPixels can be closer to the Pico W itself, with shorter wires.
* [NeoPixel alignment bracket](/assets/NeoPixel_Bracket.stl) - This bracket helps to align all of the controller port NeoPixels with the correct spacing. It will only install below the controller ports, if you have a normally installed battery. This can be printed in PLA and hot-glued into place.


## Hall of Fame

I'd like to thank people who helped making PicoBoot possible:
* [webhdx](https://github.com/webhdx) - All credit for the [PicoBoot](https://github.com/webhdx/PicoBoot) goes here. Consider being their [Sponsor](https://github.com/sponsors/webhdx)

* #gc-forever crew: [Extrems](https://github.com/Extrems), [novenary](https://github.com/9ary), [emu_kidid](https://github.com/emukidid) and others
* [tmbinc](https://github.com/tmbinc) - he started it all 🙏
* happy_bunny - [webhdx] started [webhdx's] research with his great writeup on [Shuriken Attack](https://www.retro-system.com/shuriken_attack.htm)
* PicoBoot beta testers: [seewood](https://github.com/seewood), [MethodOrMadness](https://github.com/MethodOrMadness), [renanbianchi](https://github.com/renanbianchi)
* content creators: [MachoNachoProductions](https://www.youtube.com/c/MachoNachoProductions), [RockerGaming](https://www.youtube.com/c/RockerGaming), [ModzvilleUSA!](https://www.youtube.com/c/ModzvilleUSA)
* people who sponsored [PicoBoot](https://github.com/webhdx/PicoBoot)
* every PicoBoot enjoyer - it's all about you after all 😉

## Video guides and features overview

- [PicoBoot Modchip Will Unleash The POWER of Your Nintendo GAMECUBE! | Installation Guide and Overview](https://www.youtube.com/watch?v=qwL4ZSa0xMo) by [MachoNachoProductions](https://www.youtube.com/c/MachoNachoProductions)
- [This new Gamecube Modchip is a GAMECHANGER - PicoBoot](https://www.youtube.com/watch?v=lfMTLEM0yeQ) by [RockerGaming](https://www.youtube.com/c/RockerGaming)
- [$5 Gamecube IPL Modchip?! Picoboot Dol-001 + Dol-101 Installation / Setup / Showcase](https://www.youtube.com/watch?v=W_9-mSBMBJ4) by [ModzvilleUSA!](https://www.youtube.com/c/ModzvilleUSA)
- [PicoBoot GameCube custom mod chip - make and install your own chip with a Raspberry Pi Pico](https://youtu.be/rDrosSd-nDc) by [Joe Bleeps](https://www.youtube.com/@JoeBleeps)

## Acknowledgements

Some parts of this project use GPL-2.0 licensed code from:
 * https://github.com/redolution/iplboot
