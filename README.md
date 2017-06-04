## Code for Cal Poly Space Systems Upboard

The Upboard is a custom arduino zero based board used on CPSS Rockets.

### Building

You can either use the Arduino IDE or [Ardunio Makefile](https://github.com/sudar/Arduino-Makefile)

#### With IDE

Open the main `cpss.ino` file and build.

:warning: If you edit any included files the editor must be restarted to pick up and changes.

#### With Makefile

Set `$ARDMK_DIR` to the root directory of Arduino Makefile (`/usr/share/arduino` with ubuntu package).

```
$ export ARDMK_DIR=/usr/share/arduino
$ make
$ make upload
```

#### Required External Libraries
To use interrupt based I/O you must install ASFcore and Adafruit_ZeroTimer

