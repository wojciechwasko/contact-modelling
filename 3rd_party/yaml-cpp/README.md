3rd party dependency: yaml-cpp
==============================

Rationale
---------

`yaml-cpp` is an excellent library for YAML format manipulation from C++ code. 
It is extensively used during the simulation use of `contact-modelling` as the
input files are encoded using YAML.

Obtaining the library
---------------------

`yaml-cpp` can be freely obtained from 
[Google Code website](https://code.google.com/p/yaml-cpp/).
The source code is licensed under MIT license.  

Making yaml-cpp available to contact-modelling
------------------------------------------------------

Unpacking the source code to `ROOT/3rd_party/yaml-cpp` will make the build
system pick it up automatically.


