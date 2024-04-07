# NAME

`kea` - Game Boy emulator

# SYNOPSIS

`kea`

# DESCRIPTION

This is a DMG Game Boy emulator program written in C++17, conforming to the 
architecture specifications in [gbdev.io](https://gbdev.io/pandocs/) and the
[Game Boy: Complete Technical Reference](https://gekkio.fi/files/gb-docs/gbctr.pdf).

Graphics, sound, and user input are all handled via the [SDL2 Library](https://wiki.libsdl.org/SDL2/FrontPage).

For now, the code does not strive for full cycle accuracy or bug-for-bug
emulation. Game Boy Color features are also not currently implemented.

# DEPENDENCIES

In order to compile successfully, this code requires the SDL2 libraries to be
present on the system, along with the Boost program_options library. Running tests
requires the Boost.Test libraries.

# OPTIONS

TBD

# NOTES

TBD

# KNOWN ISSUES
