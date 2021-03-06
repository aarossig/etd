# etd - Embedded Tower Defense

## Overview

The goal of this project is to implement a tower defense game that uses a
terminal emulator over UART as a user interface.

## Terminal Emulator Support

This has been tested in gnome-terminal using miniterm.py. Minicom attempts to
emulate a VT102 and ignores some non-critical escape sequences.

It has also been tested in Putty on Windows.

## Supporting Documentation

The following articles have been useful for developing this application.

* Terminal Escape Codes
    * http://invisible-island.net/xterm/ctlseqs/ctlseqs.html
* XTerm 256 Colors
    * http://www.calmar.ws/vim/256-xterm-24bit-rgb-color-chart.html
