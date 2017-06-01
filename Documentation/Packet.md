## Telemetry Packet Format

These telemetry packets are used to communicate rocket status with the ground station

There are several components that make up a complete packet:

`C<rtc><time><gps><acc><gryo><baro><pres>*<CS>`

#### Header

The header represents the start of the message and it is always the one byte ACSII `C`

#### RTC

RTC stores the number of milliseconds since power on, useful in case of GPS failure

`<rtc 15:0>`

Type: unsigned 16 bit integer

#### Time

Time stores the time from the GPS

`<hour11:8><minutes 7:4><seconds 3:0><fractions ?>`

TODO: precision of fraction

#### GPS

GPS stores data from GPS output

`<lat 31:16><long 16:0>`

time is the time mentioned below

lat and long represent the latitude and longitude of the GPS

TODO: Fixed point precision?

#### Gyro

Gyro stores 9-axis gyro measurements

`<x 47:32><y 31:16><z 15:0>`

Type: Signed fixed point integer - 12/4


#### Pressure Tap

Pressure tap data

`<t5 95:80><t4 79:64><t3 63:48><t2 47:32><t1 31:16><t0 15:0>`

Type: Unsigned 16 bit integer

#### Footer

Footer represents the end of the message and is always the one byte ASCII `*`.

#### Checksum

The checksum is used to check the integrity of the message.
It is one byte and calculated by XORing everything previous to it.
