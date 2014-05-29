3rd party dependency: triangle
======================================

Rationale
---------

`triangle` is a 

> A Two-Dimensional Quality Mesh Generator and Delaunay Triangulator.

It is the primary tool used for performing Delaunay triangulation in the
`contact-modelling` library.

Obtaining the library
---------------------

`triangle` can be freely obtained from 
[the author's website](http://www.cs.cmu.edu/~quake/triangle.html). The code is
licensed under a custom license which states that:

> Private, research, and institutional use is free.

Making `triangle` available to contact-modelling
------------------------------------------------

To make the `triangle` library available to `contact-modelling`, place
`triangle.h` and `triangle.c` in `ROOT/3rd_party/triangle/{inc,src}/`
respectively. It then becomes a build target for the CMake toolchain.
