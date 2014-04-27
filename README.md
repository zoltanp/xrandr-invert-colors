XRandR Invert Colors
====================

Simple utility that inverts colors on all screens, using XrandR.

Dependencies
-------------
- XrandR development files


Building
--------
From a terminal, type `make`


Using
-----
Run `xrandr-invert-colors.bin` for inverting colors on all screens.
The follwing program option is implemented:

* `-s <crtc_number>` : Instead of inverting all screens, invert only the colors of the screen identified by `<crtc_number>`.
* `--help` and `-h` : Print a short help for the program.
* `--version` : Print a (dummy) version information.
* `-d` : Enables debug log on standard output. Prints diagonstic messages.

Hint: put the executable in the a directory listed in $PATH, and bind the execution of `xrandr-invert-colors.bin` to a shortcut key.


Achknowledgements
-----------------

Redshift developers, for creating the XRandR gamma setting code. Their code is reused in this application.
http://jonls.dk/redshift/
https://launchpad.net/redshift

License
-------
GNU GPL v3


