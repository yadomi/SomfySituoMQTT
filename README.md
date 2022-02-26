SomfySituoMQTT
==============


## What is this about ?

This repository aim to provides codes sample and documentions to replicate an experiment I made to control Somfy IO roller shutter with an ESP8266 and a Situo 5 remote, with MQTT support.

## Requirements

- Some Somfy IO roller shutter
- a Situo IO remote. I'm using the Situo 5 
- an ESP8266, I'm using the d1 mini
- an MQTT Broker

## Hardware setup

The main goal is to solder wires from each buttons of the remote to any GPIO of the esp8266. 
Depending of the version of your remote, the physcal button of the remote can be different. 
Previous remote were using membrane style/rubber dome swith but recents one use miniature tactile switch. 

In both case, you have to solder a wire on the **VCC side of the switch**.

| version | where to solder |
|-|-|
| membrane | On the outer ring of the switch  |
| tactile switch | On the corresponding test probe point (on the other side of the PCB, see image below) |

### Wiring

The original remote use a 3V battery and the Wemo D1 have a 3.3V output. You can be adventurous and connect it directly as it will probably 
be fine since the IC of the remote is only powered when pressing a button. Or you can use a [voltage divier](https://en.wikipedia.org/wiki/Voltage_divider) with value of R1 = 100 Ohm and R2 = 10 Ohm, for 3V. 

![wiring](https://user-images.githubusercontent.com/7341187/155855025-fde316c5-0c64-43db-9921-8322b5ac17c6.png)

The PROG button is not connected in the schema but you can connect it as well.


## Code setup

Set your wifi password here:

https://github.com/yadomi/SomfySituoMQTT/blob/f04af5b4010ef12d059bee928fa6030ffb6ee80a/sketch.ino#L6-L8

Then you broker information here:
https://github.com/yadomi/SomfySituoMQTT/blob/f04af5b4010ef12d059bee928fa6030ffb6ee80a/sketch.ino#L10-L14

You can also edit the base mqtt topic on L14.

## Usage

When powering the D1, make sure the remote is on the first channel before sending any command as this is used to keep track of the current channel.

The MQTT command has this shape:

```
somfy/channel/<number>/<up|down|my>
```

So, if you want to open the roller shutter on channel 3, send:

```
somfy/channel/3/up
```


## FAQ

- Q: Why not using the Tahoma ? 
  - A: The Somfy "Conectivity Kit" is 69€ but is cloud based and I don't want that. The Tahoma Switch has a local API but is 199€. This cost less than 50€ if you buy the remote second hand or less if you already have one. 
- Q: Will it work with Somfy RTS ? 
  - A: Yes, should work as well. However the RTS protocol is simpler that IO-Homecontrol and can be emulated with a 433Mhz transitter so you don't need to sacrifice a remote. Take a look at  https://github.com/Legion2/Somfy_Remote_Lib and https://github.com/etimou/SomfyRTS 
- Q: This is nice but this won't give me feedback status return ? 
  - A: True, this won't give you much than the remote. I'm using cheap zigbee door sensors on the roller blinds to know the status
