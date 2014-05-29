Introduction
============

`contact-modelling` is a library developed as part of the requirements for my
Master thesis. It deals with processing tactile data -- output of capacitive
transduction-based artificial skin systems.

License
=======
Copyright (C) 2014 Wojciech Waśko

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Source code layout
==================
The library is located in `contact-modelling` directory. Directory `cm-clients`
contains various toy consumers of the library I use along the way.

The directory named `3rd_party` contains informations and placeholder
directories for external libraries `contact-modelling` depends on. In each of
the dependencies' directory, there's a README file with instructions on how to
obtain the required libraries.

Building and testing
====================
The build toolchain is `CMake`. To build the library,

    cd build/ && cmake .. && make

Unit tests are written using [boost test](http://www.boost.org/doc/libs/release/libs/test/)
library. To build and execute tests,

    cd build/ && cmake .. && make check

Accessing documentation
=======================
Source code is documented using [doxygen](www.doxygen.org). To generate the
documentation,

    cd build/ && cmake .. && make doc

This generates a user's documentation (i.e. implementation details are hidden).
To build full documentation,

    cd build/ && cmake .. && make devdoc

Each of these commands generates an HTML version of the documentation in
`doc/html` and `doc/dev-html` respectively.
