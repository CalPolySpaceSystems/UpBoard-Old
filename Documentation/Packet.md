## Telemetry Packet Format

These telemetry packets are used to communicate rocket status with the ground station

There are several components that make up a complete packet:

`C<rtc 40:39><time 38:35><gps 34:27><acc 26:24><gryo 23:21><mag 20:18><baro 17:14><pres 13:2>*<CS 0>`

#### Header

The header represents the start of the message and it is always the one byte ACSII `C`

#### RTC

RTC stores the number of milliseconds since power on, useful in case of GPS failure

`<rtc 15:0>`

Type: unsigned 16 bit integer

#### Time

Time stores the time from the GPS

`<time 31:0>`

It is an unsigned 32 bit integer of the format `HHMMSSss`, where `H` is the hour,
`M` is the minute, `S` is the second, and `s` is the fraction of a second.

#### GPS

GPS stores data from GPS output

`<lat 63:32><long 31:0>`

The format is a signed 32 bit integer of the format `-DDDMMmm`, where `D` is degree,
`M` is minutes, and `m` is fraction of a minute.

#### Accelerometer

Accelerometer stores 9-axis measurements.

`<x 47:32><y 31:16><z 15:0>`

Type: Signed fixed point 16 bit integer - 6/10
Units: g

#### Gyro

Gyro stores 9-axis gyro measurements.

`<x 47:32><y 31:16><z 15:0>`

Type: Signed fixed point 16 bit integer - 12/4
Units: degrees per seconds

#### Magnetometer

Magnetometer stores 9-axis measurements.

`<x 47:32><y 31:16><z 15:0>`

Type: Signed fixed point 16 bit integer - 4/12
Units: gauss

#### Barometer

The barometer stores pressure and altitude data.

`<pres 31:16><alt 15:0>`

Types:
- Altitude: unsigned fixed point 16 bit integer - 14/2
- Pressure: unsigned fixed point 16 bit integer - 8/8

Units:
- Altitude: meters above sea level
- Pressure: millibar

#### Pressure Tap

Pressure tap data

`<t5 95:80><t4 79:64><t3 63:48><t2 47:32><t1 31:16><t0 15:0>`

Type: Unsigned 16 bit integer

#### Footer

Footer represents the end of the message and is always the one byte ASCII `*`.

#### Checksum

The checksum is used to check the integrity of the message.
It is one byte and calculated by XORing everything previous to it.
