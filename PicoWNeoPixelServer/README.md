[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](LICENSE)

# PicoWNeoPixelServer: Control NeoPixels(WS2812, etc.) from a Raspberry Pi Pico W

This project turns your Raspberry Pi Pico W into a Wi-Fi access-point. Connecting to it from your computer, tablet or phone will automatically redirect you to a page allowing to control each individual NeoPixel in your strand. It was developed specifically to be consumed by a project for GameCube, and as such, the included web app reflects that fact. While this is certainly not appropriate for most applications, the example provided by this should be sufficient to create a similar NeoPixel project for any application!

Should you require further specialization/customization, the [PicoHTTPServer project](https://github.com/sysprogs/PicoHTTPServer) has created an extremely useful example illustrating how to directly control each pin of your Pico W! Otherwise, an application-agnostic version of this app is still considered long-term.

### Captive Portal
To make sure that users are displayed the appropriate page whenever a login attempt occurs, the project uses a Captive Portal. This is a relatively complex operation(which is explained in [full detail here](https://github.com/sysprogs/PicoHTTPServer)), which should dependably trigger a sign-in request on the user's device and display the appropriate page upon a login attempt.


### WiFi Settings

The Wifi SSID and Password can easily be configured by modifying the CMakeLists.txt file, altering the `set` commands shown below:
````
# Set the appropriate compiler options
set(WIFI_SSID GC_RGB) #Choose any valid name for network
#set(WIFI_PASSWORD 12345678) #Setting no password leaves the network open
````
Because the purpose of this project is to allow users to access the page as easily as possible, the default configuration is not password-protected. Uncomment the line `set(WIFI_PASSWORD 12345678)` and replace the `12345678` with your desired password, if you wish to password-protect your network.

If you intend on using [VisualGDB](https://visualgdb.com/) to build the app, then you'll also need to change the appropriate options in the [PicoHTTPServer.vgdbproj file](PicoHTTPServer/PicoHTTPServer.vgdbproj):
````
<Configuration>
  <Board>pico_w</Board>
  <SSID>GC_RGB</SSID>
  <Password />
</Configuration>
````

If you would like to display a custom URL, the settings to do so are contained in the [server_settings.c](PicoHTTPServer/server_settings.c) file. The default address is `gc.rgb`, and the address will be displayed in the format `hostname.domain_name`
````
.hostname = "gc",
.domain_name = "rgb",
````

For a more detailed explanation of how the HTTP server functions, please read [the original documentation for PicoHTTPServer](https://github.com/sysprogs/PicoHTTPServer).

### Simple File System

In order to support images, styles or multiple pages, the HTTP server includes a tool packing the served content into a single file (along with the content type for each file). The file is then embedded into the image, and is programmed together with the rest of the firmware. You can easily add more files to the web server by simply putting them into the [www](www) directory and rebuilding the project with CMake.

You can dramatically reduce the FLASH utilization by the web server content by pre-compressing the files with gzip and returning the `Content-Encoding: gzip` header for the affected files. The decompression will happen on the browser side, without the need to include decompression code in the firmware.

## Building the App

The easiest way to build the sources on Windows is to install [VisualGDB](https://visualgdb.com/) and open the `PicoHTTPServer.sln` file in Visual Studio. VisualGDB will automatically install the necessary toolchain/SDK and will manage build and debugging for you.

You can build the project manually by running the [build-all.sh](build-all.sh) file. Make sure you have CMake and GNU Make installed, and that you have the ARM GCC (arm-none-eabi) in the PATH.

Once the project is built, boot your Pico W in bootloader mode. Then, copy the **PicoHTTPServer.uf2** file to it. The Pico W will automatically restart and create a network with the name and password chosen in the [CMakeLists.txt](/PicoHTTPServer/CMakeLists.txt) file.

## Modifying the App

The project that this repo is based on is more generally-applicable for the Raspberry Pico W's pins, and I **highly** recommend that you take a look at [the original repo](https://github.com/sysprogs/PicoHTTPServer), especially if you desire to make changes unrelated to NeoPixels. There is [a tutorial included](https://visualgdb.com/tutorials/raspberry/pico_w/http/) in that project that explains step-by-step how to make changes.
