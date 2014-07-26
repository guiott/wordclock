wordclock
=========

this is a study for the realization of a  somehow unusual word clock
It's more a study for some special HW and SW techniques than a real application.
It uses a 10 x 11 matrix led display and not fixed words leds.
The matrix is refreshed by the PIC18F4620 I/O pins through a series of high and low side power drivers.
The time is maintained by a self powered I2C RTC.
The time is syncronized with INRIM via a WiFly internet connection.
