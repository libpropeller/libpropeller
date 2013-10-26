## Wiki Navigation
Please take a look at the available pages:

[Available Wiki Pages](https://github.com/libpropeller/libpropeller/wiki/_pages)


## Introduction to libpropeller
libpropeller is an open source collection of C++ Propeller GCC objects to help facilitate development with the Parallax Propeller microcontroller. The objects range from motor control to sensor interfacing to internal operations. libpropeller is very similar to Parallax's Spin Obex.

libpropeller is licensed under the MIT license.

You can view the API documentation [here](https://rawgithub.com/libpropeller/libpropeller.docs/master/html/index.html).

## Getting Started
The easiest way to use libpropeller code in your project is to download the objects of interest to your project folder. Then, you can just include them in your .cpp files. Some objects (that take a seperate cog, usually) have an associated .S file that you'll have to include in your make file or on the command line.

libpropeller has all of it's objects organized into .h files (see [here](https://github.com/libpropeller/libpropeller/wiki/Inline-Code-in-Headers-Justification) for the reasoning behind that). This means that you don't need to explicitly include the objects in your makefile or on the command line. If you have the objects in your project folder you can just
`#include "numbers.h"`
and the compiler will automatically find the file in the local folder. Alternatively, you can pass the location of the libpropeller folder with the -I option. For example, if your libpropeller copy is in /home/coolCatUser/code/libpropeller you can pass
`-I /home/coolCatUser/`
and include the following in your project code:
`#include "libpropeller/numbers/numbers.h"`

## libpropeller Volatility
libpropeller is changing very rapidly. If a code change is deemed beneficial little to no consideration will be given to maintaining backwards compatibility. Typically these changes are minor (function name changes, etc.) but occasionally they may be more significant. Always look carefully at libpropeller code before you update to make sure that you know what changes need to be made in your projects.

## Developing and Contributing to libpropeller
libpropeller has been developed by SRLM, with support from Red9 and Kenneth B.

Any contributions are welcome. If you have any comments or would like to help please send me an email at srlm@srlmproductions.com.

[Inline Code In Headers Justification](https://github.com/libpropeller/libpropeller/wiki/Inline-Code-in-Headers-Justification)


