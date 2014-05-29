3rd party dependency: gnuplot-iostream
======================================

Rationale
---------

`gnuplot-iostream` is a thin wrapper for accessing and controlling
[gnuplot](http://www.gnuplot.info/) from C++ code. It was used in early versions
of the `contact-modelling` library as a primary plotting engine. Now, the
visualisation is done with the help of `mayavi`, but `gnuplot-iostream` remains
legacy dependency.

Obtaining the library
---------------------

`gnuplot-iostream` is a header-only library. The source code -- licensed under
MIT license -- is available for download at 
[the author's website](http://www.stahlke.org/dan/gnuplot-iostream/).  

Making gnuplot-iostream available to contact-modelling
------------------------------------------------------

Since `gnuplot-iostream` is a header-only library, place the header
`gnuplot-iostream.h` in `ROOT/3rd_party/gnuplot-iostream/inc/`.
`contact-modelling` is configured to look for said header in this directory

