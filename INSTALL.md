# Installation Instructions

**HERMES** can be installed on GNU/Linux and macOS (OS X), while other operating systems have not been tested and are generally not supported.

## Dependencies and requirements

Required:
- **CMake** (`cmake`) is absolutely necessary to configure and build the source code (ver. 3.1+)
- **GCC** (`g++`) or **Clang** (`clang++`) are supported compilers; as modern C++ is employed (C++14), at least [GCC 6.1](https://gcc.gnu.org/projects/cxx-status.html#cxx14) / [Clang 3.4](https://clang.llvm.org/cxx_status.html) should be considered
- **GNU Scientific Library (GSL)** (`gsl`) is mandatory for numerical integration and for special functions

Optional:
- **Git** is needed if one wants to clone and keep in sync the source code from the git repository (**recommended**)
- **Python** (Python 3.x) is needed for the Python interface (**recommended**)
- **CFITSIO** is highly recommended since it enables reading DRAGON CR files and saving results in FITS files (**recommended**)
- **FFTW3F** provides turbulent components for galactic magnetic field models
- **Doxygen** generates documentation from the source code
- **LCOV** is used with `gcov` (GCC) to generate the code coverage reports
- **Clang Tools** are necessary to use clang-format and clang-tidy which maintain code hygiene (recommended for developers)

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
CMAKE_PREFIX_PATH=$HERMES_DIR cmake -DCMAKE_INSTALL_PREFIX=$HERMES_DIR -DENABLE_TESTING=On ..
make -j
make install
```

## Install on macOS (OS X)

To obtain the required and optional packages, one can use [Homebrew](https://brew.sh):
```sh
brew install python3 cfitsio gsl jq
brew link --overwrite python # to make Python3 default in Homebrew
brew install gcc # only if GCC is desired
brew link --overwrite gcc # same as above
```

CMake offers out-of-source build, so we need to make a new folder inside of the source folder:
```sh
cd hermes # the folder where we cloned the git repository
mkdir build
cd build
```

To be on the safe side regarding the Python version (if multiple versions are found on the system):
```sh
export PYTHON_BREW_PATH=$(brew --cellar python)/$(brew info --json python | jq -r '.[0].installed[0].version');
```

For Clang:
```sh                
export CC=`xcrun -find cc`
export CXX=`xcrun -find c++`
```

Or for GCC:
```sh
export GCC_BREW_PATH=$(brew --cellar gcc)/$(brew info --json gcc | jq -r '.[0].installed[0].version');
export CC=$GCC_BREW_PATH/bin/gcc-9
export CXX=$GCC_BREW_PATH/bin/g++-9
```

Run `cmake` to generate Makefile
```sh
cmake .. \
  -DPYTHON_EXECUTABLE=$PYTHON_BREW_PATH/bin/python3 \
  -DPYTHON_LIBRARY=$PYTHON_BREW_PATH/Frameworks/Python.framework/Versions/3.7/lib/libpython3.7.dylib \
  -DPYTHON_INCLUDE_PATH=$PYTHON_BREW_PATH/Frameworks/Python.framework/Versions/3.7/include \
  -DENABLE_TESTING=On
```

Finally, make the library and test executables:
```sh
make -j
make install
```
