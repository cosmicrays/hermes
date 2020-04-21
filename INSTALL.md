# Installation Instructions

**HERMES** can be installed on GNU/Linux and macOS (OS X), while other operating systems have not been tested and are generally not supported.

## Dependencies and requirements

Required:
- **CMake** (`cmake`) is absolutely necessary to configure and build the source code (ver. 3.1+)
- **GCC** (`g++`) is the only supported compiler at the moment (support for `clang` and intel compilers will be added later); as C++14 is needed, at least GCC 6 should be used
- **GNU Scientific Library (GSL)** (`gsl`) is required for numerical integration and for special functions

Optional:
- **Git** is needed if one wants to clone and keep in sync the source code from the git repository (**recommended**)
- **Python** (Python 3.x) is needed for the Python interface (**recommended**)
- **CFITSIO** is highly recommended since it enables reading DRAGON CR files and saving results in FITS files (**recommended**)
- **FFTW3F** provides turbulent components for galactic magnetic field models
- **Doxygen** generates documentation from the source code
- **LCOV** is used with `gcov` (GCC) to generate the code coverage reports

Provided with the source:
- **Pybind11** is used to build the python interface to C++ classes and functions
- **Google Test** is employed as a framework for unit tests
- **cparamlib** is used to calculate fluxes of stable secondary particles from p-p interactions (Kamae et al. 2006, 2007)

## Obtaining HERMES source

The preferred way is to clone the source from the git repository (for what [`git`](https://git-scm.com/book/en/v2) is required):

```sh
git clone https://github.com/cosmicrays/hermes.git
```

The other method is to download a ZIP file from [the GitHub page](https://github.com/cosmicrays/hermes) by clicking "Clone and download" and then "Download ZIP".

## Install on GNU/Linux

Installation with [python virtualenv](https://virtualenv.pypa.io) is recommended:
```sh
export HERMES_DIR=$HOME"/.virtualenvs/hermes"
virtualenv -p python3 $HERMES_DIR
source $HERMES_DIR/bin/activate
```
Once the environment is activated, clone the repository and build it with cmake:
```sh
cd hermes
mkdir build
cd build
CMAKE_PREFIX_PATH=$HERMES_DIR cmake -DCMAKE_INSTALL_PREFIX=$HERMES_DIR ..
make -j
make install
```

## Install on macOS (OS X)

TODO (see commands in .travis.yml for now)
