# AT (Hayes) commands host library

C library that allows to implement AT commands interpreter.

# Buildin commands:

* AT
* ATE0 ATE1
* AT+CMEE
* A/

# Requirements

Tests and examples requires CMake, C++ compiler, and gtest (Google C++ test library).

# How to use

Copy source files from ath/ directory in to your project, check examples.

# Examples

## livetest

Uses stdin to parse input commands, and stdout to output results.

Livetest program registers "at+exit" command. Also some unsolicited status messages are displayed.

## pstest

Pstest example  program opens pseudo-terminal, and sent its name to stdout. Use terminal program (picocom, minicom, etc) to connect to the file. 

This example also suports "at+exit" command.

## AT command parameters parsing

Check tests/at_tests.cpp file, test_10 for single parameter parsing, and test_19 for two parameters parsing examples.

# License 

MIT



