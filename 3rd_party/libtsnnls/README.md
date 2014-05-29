3rd party dependency: libtsnnls
===============================

Rationale
---------

`libtsnnls` is a fast solver for least-squares problems in the form of `Ax=b`
under the constraint that each element of the `x` vector is non-negative.

In some situations encountered in tactile contact modelling, non-negativity of
the tractions acting on the surface is a powerful physicality constraint, e.g.
in a single-contact case, it is impossible (in a normal contact scenario) to
generate negative components of tractions.

Obtaining the library
---------------------

The source code -- licensed under GNU LGPL -- is available for download at [the
author's website][http://www.jasoncantarella.com/wordpress/software/tsnnls/].

Linking to libtsnnls
--------------------

After the `tsnnls` library has been compiled and installed, `contact-modelling`
will automatically find it and compile and link against it (please refer to
`ROOT/cmake/Findlibtsnnls.cmake`).
