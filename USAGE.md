# Usage Guide

The NeoPicoBoot web interface is designed to be as straightforward to use as possible. This guide explains the features and their usage. If you would like to request additional patterns or animations, please submit a feature request to this repository.

## Network Name and Password

NeoPicoBoot uses the Captive Portal method broadcasting a network as a dedicated WiFi Access Point. By default, this network will have the default SSID of `GC_RGB` and no password.

These values can be altered in [the CMakeLists.txt file in the PicoWNeoPixelServer/PicoHTTPServer directory](https://github.com/paulmreese/NeoPicoBoot/blob/686e2f957be58c4621f378fb9fc77a336fcbb2db/PicoWNeoPixelServer/PicoHTTPServer/CMakeLists.txt#L35-L36). You can substitute any valid name for the network SSID you prefer. To add a password, just uncomment `set(WIFI_PASSWORD 12345678)` and replace `12345678` with that password. You'll need to recompile the firmware if you change these settings. The remainder of this guide will refer to the default settings.

## Connecting

When you start up the Pico W after installing the firmware(or when you startup your GameCube with the mod installed), a startup animation sequence will display on the NeoPixels. This happens after the PicoBoot code has successfully run. The display occurs as the server is initialized. You can now use any WiFi-enabled device to access the `GC_RGB` network, which will initiate the Captive Portal process. For most modern phones, you'll be given the option to `Sign into network`(or similar wording) as a notification. On many devices the control page will automatically appear upon your attempted connection.

### Troubleshooting Connection

Some devices may notice that this network offers no internet access and balk at any connection attempts. In this case there is often an option to `Connect Anyways` within the network-specific settings. This may require that the device remembers the network for next usage. Please submit any connection issues you encounter to this repo.

## Control

The controls are presented as a responsive web app, and as the result of this approach will differ between different systems. If you encounter any issues interacting with the controls, please let me know!

<p align=center>
  <img src="/assets/Web_App_GUI_small.jpg" alt="A screen shot of the GameCube RGB interface of NeoPicoBoot">
</p>

For any individual LED on the system, you can click or tap the corresponding location to bring up a color picker to set a new color. The color will fade from the current color to the chosen color.

For patterns of animation affecting all NeoPixels, you'll first need to select the relevant animation from the Effect drop-down. This will activate any options that would be used by the particular animation. Next, you'll need to set the desired colors, repetitions, speed, and/or brightness. Once you've chosen your values, click `Go`!
