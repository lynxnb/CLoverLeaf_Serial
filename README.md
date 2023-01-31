# CLoverLeaf_Serial

A C port of the serial (no-MPI) version of CloverLeaf, for use with [TAFFO](https://github.com/TAFFO-org/TAFFO) (Tuning Assistant for Floating Point to Fixed Point Optimization).

Original repository: [UK-MAC/CloverLeaf_Serial](https://github.com/UK-MAC/CloverLeaf_Serial)

## Performance

Performance should be on par with the Fortran version using C kernels.

## Building

Building is done using GNU Make. See the Makefile available targets and options.

## Motivation

This port was created as part of an individual university project.
The goal of the project was to apply TAFFO annotations to the CloverLeaf benchmark, to compare the possible peformance gains from replacing floating point variables with fixed point ones.
As TAFFO only supports C/C++, a C port of CloverLeaf was needed. The port was done by hand, without the use of any automatic tools.
It should also be feature complete with the Fortran version, and an attempt was made to keep both the code and the output as similar as possible.
For this reason, possible performance optimizations were not performed (e.g. replacing the indexing macros in the kernels).

## TAFFO Usage

To ease the process of annotating the code, an effort was made to be able to generate annotations automatically.
A rudimental usage tracker is able to periodically collect data about the program's working arrays, thus being able to report the minimum and maximum values used per-array.
It can then print out those values as a human-readable report, or as ready-to-use TAFFO annotations, which can then be applied to the code directly.

To enable the usage tracker, user callbacks need to be enabled at compile time:
```bash
make USER_CALLBACKS=1
```
Usage info will be saved to the `usage_tracker.txt` file.

## User Callbacks
User callbacks are functions that can be used to run custom code at specific execution points in the program, allowing for user defined code to be executed without having to modify the original source. For example, they are used to run the usage tracker.
