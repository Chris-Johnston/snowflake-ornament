# Snowflake-Ornament

A snowflake shaped Xmas ornament with blinking LEDs.

![Image of the snowflake ornament.](images/snowflake.png)

This uses an ATTiny85 to drive 6 blue LEDs which are multiplexed.
There's a button on the front which is used to toggle display modes
and sleep.

When sleeping, the ornament uses about 200 microamps of current, little enough to run
off of a coin cell battery for about 1000 hours! While running, it uses about 2 milliamps,
which could last for about 120 hours.

![Image of the back of the ornament.](images/back.png)

The part was designed using KiCad. The shape of the PCB was generated with
[svg2mod]. The [outline of the board was modified from Wikipedia (Public Domain)](https://en.wikipedia.org/wiki/File:Snow_flake.svg), I added a hexagon in the
center to make room for the button, ATTiny85, and battery.

[svg2mod]: https://github.com/svg2mod/svg2mod

## Requirements

I use [damellis/attiny] for ATTiny85 support in the Arduino editor (via the VSCode extension).

[damellis/attiny]: https://github.com/damellis/attiny