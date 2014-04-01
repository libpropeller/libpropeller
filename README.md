## Introduction to libpropeller
libpropeller is an open source collection of C++ Propeller GCC objects to help facilitate development with the Parallax Propeller microcontroller. The objects range from motor control to sensor interfacing to internal operations. libpropeller is very similar to Parallax's Spin Obex.

libpropeller is licensed under the MIT license.

You can view the API documentation [here](https://rawgithub.com/libpropeller/libpropeller.docs/master/html/index.html).

## Recent Changes
* 2014-04-01: Added ``libpropeller`` as the namespace for all classes and fields. This is a breaking change; but one that's easy to integrate into existing code bases. Simply prepend all libpropeller class and variable names with ``libpropeller``. More information about namespaces can be found [here](http://www.cplusplus.com/doc/tutorial/namespaces/).

## Getting Started
The easiest way to use libpropeller code in your project is to download the objects of interest to your project folder. Then, you can just include them in your .cpp files. Some objects (that take a seperate cog, usually) have an associated .S file that you'll have to include in your make file or on the command line.

libpropeller has all of it's objects organized into .h files (see [here](https://github.com/libpropeller/libpropeller/wiki/Inline-Code-in-Headers-Justification) for the reasoning behind that). This means that you don't need to explicitly include the objects in your makefile or on the command line. If you have the objects in your project folder you can just
`#include "numbers.h"`
and the compiler will automatically find the file in the local folder. Alternatively, you can pass the location of the libpropeller folder with the -I option. For example, if your libpropeller copy is in /home/coolCatUser/code/libpropeller you can pass
`-I /home/coolCatUser/`
and include the following in your project code:
`#include "libpropeller/numbers/numbers.h"`

## libpropeller Stability
libpropeller is changing very rapidly. If a code change is deemed beneficial little to no consideration will be given to maintaining backwards compatibility. Typically these changes are minor (function name changes, etc.) but occasionally they may be more significant. Always look carefully at libpropeller code before you update to make sure that you know what changes need to be made in your projects.


## Developing and Contributing to libpropeller
libpropeller has been developed by SRLM, with support from Red9 and Kenneth B.

Any contributions are welcome. If you have any comments or would like to help please send me an email at srlm@srlmproductions.com.

## Class Status

This page details that status of each class in libpropeller. Since libpropeller changes very fast it's intended to give a quick overview of what works, what doesn't, and so. Options for each class are:

Status | Description
-------|------------
Gold | Gold objects have been used extensively, have complete documentation, testing, and features, and are not expected to change significantly.
Silver | Silver objects have documentation, testing, and have been used in the field. Silver objects may undergo major overhauls.
Bronze | Bronze objects are new, with incomplete unit tests, documentation, or use in the field. Bronze objects are tested for basic functionality, but may still have bugs.
Deprecated | Deprecated objects have been decommissioned for one reason or another, and are unsupported.

Roughly speaking, a higher rating means code is more stable and less likely to change.

Any of the above categories may be modified with a + or -. A + indicates that it is close to one level above while a - indicates that it is close to one level below.

Class | Status | Notes
------|--------|------
EEPROM | Silver |
Elum | Deprecated | See Pin for most of the same functionality.
FullDulpexSerial | Deprecated | See Serial for a better object.
GPSParser | Silver |
I2C | Silver+ |
I2CBase | Silver+ |
L3GD20 | Silver |
LSM303DLHC | Silver |
MAX17048 | Silver |
MCP3208 | Silver |
MS5611 | Silver- |
MTK3339 | Bronze+ | Good code, but no unit testing available
Numbers | Silver |
PCF8523 | Silver |
Pin | Silver+ |
PulseWidthReader | Bronze |
PWM2 | Bronze |
PWM32 | Bronze |
QuadratureEncoder | Bronze |
Scheduler | Silver- |
SD | Silver- | Works fine, but FAT32 things are messy
SDSafeSPI | Silver- |  Works fine, but FAT32 things are messy
Serial | Silver+ |
Stopwatch | Silver |
VNH2SP30 | Bronze |


## Code Standards

To maintain the consistency and quality of the code base all libpropeller classes must adhere to the following:

1. All classes must be have the code defined in the header (inline). The only exceptions are .S assembly files and .cpp files with static variable allocation.
2. All classes must have Javadoc Doxygen compatible documentation (as shown [here](https://rawgithub.com/libpropeller/libpropeller.docs/master/html/index.html)).
3. All classes must follow the C++ style guidelines.
4. All classes must have some sort of unit tests that pass(even if it does nothing more than pass a single test).

If you're interested in contributing the project contact SRLM.

## Additional Documentation
Please take a look at the available pages:

[Available Wiki Pages](https://github.com/libpropeller/libpropeller/wiki/_pages)
