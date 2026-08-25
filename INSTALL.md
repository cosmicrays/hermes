# Installation Instructions

**HERMES** can be installed on GNU/Linux and macOS (OS X), while other operating systems have not been tested and are generally not supported.

The maintained online guide is available in the
[`hermes-docs` repository](https://hermes-skymaps.github.io/hermes-docs/getting-started/installation/).

## Dependencies and requirements

Required:

- **CMake** (`cmake`) 3.20 or newer is required to configure and build the source code
- **GCC** (`g++`) or **Clang** (`clang++`) with C++17 support
- **GNU Scientific Library (GSL)** (`gsl`) is mandatory for numerical integration and for special functions
- **zlib** is a file (de)compression library (or more precisely "a massively spiffy yet delicately unobtrusive compression library") and is needed for reading compressed data files
- **CFITSIO** is required by the gas models and FITS input/output interface

Optional:

- **Git** is needed if one wants to clone and keep in sync the source code from the git repository (**recommended**)
- **Python** (Python 3.x) is needed for the Python interface (**recommended**)
- **FFTW3F** provides turbulent components for galactic magnetic field models
- **LCOV** is used with `gcov` (GCC) to generate the code coverage reports
- **Clang Tools** are necessary to use clang-format and clang-tidy which maintain code hygiene (recommended for developers)

Provided with the source:

- **Pybind11** is used to build the python interface to C++ classes and functions
- **Google Test** is employed as a framework for unit tests
- **cparamlib** is used to calculate fluxes of stable secondary particles from p-p interactions (Kamae et al. 2006, 2007)
- **zstream-cpp** provides support for zlib compressed streams

## Obtaining HERMES source

The preferred way is to clone the source from the git repository (for what [git](https://git-scm.com/book/en/v2) is required):

```sh
git clone https://github.com/HERMES-SkyMaps/hermes.git
```

The other method is to download a ZIP file from [the GitHub page](https://github.com/HERMES-SkyMaps/hermes) by clicking "Code" and then "Download ZIP".

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
CMAKE_PREFIX_PATH=$HERMES_DIR cmake -S . -B build \
    -DCMAKE_INSTALL_PREFIX=$HERMES_DIR \
    -DENABLE_TESTING=ON \
    -DDOWNLOAD_DATA=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure
cmake --install build
```

The approximately 282 MiB runtime dataset is deliberately not downloaded during
ordinary configuration. Use `-DDOWNLOAD_DATA=ON`, point
`-DHERMES_DATA_SOURCE_DIR=/path/to/data` at an existing unpacked dataset, or set
`HERMES_DATA_PATH` when running HERMES.

Important build options include:

- `ENABLE_PYTHON=ON|OFF` — build the Python extension (default: `ON`).
- `ENABLE_TESTING=ON|OFF` — build unit tests (default: `OFF`).
- `DOWNLOAD_DATA=ON|OFF` — download the pinned runtime data (default: `OFF`).
- `HERMES_INSTALL_DATA=ON|OFF` — package available runtime data (default: `ON`).
- `ENABLE_SYS_HDF5=ON|OFF` — opt into unmaintained PICARD/HDF5 support (default: `OFF`).
- `HERMES_ENABLE_FAST_MATH=ON|OFF` — allow non-IEEE math optimizations (default: `OFF`).

Installed C++ packages can be consumed directly from CMake:

```cmake
find_package(Hermes 1 CONFIG REQUIRED)
target_link_libraries(my_target PRIVATE Hermes::hermes)
```

### Dependencies

Required packages to build on RHEL/CentOS/Fedora systems:

```sh
dnf install git cmake g++ gsl-devel zlib-devel cfitsio-devel fftw-devel \
python3-virtualenv python3-virtualenvwrapper python3-devel
```

Recommended Python modules:

```sh
pip install astropy healpy matplotlib numpy
```

## Install on macOS (OS X)

To obtain the required and optional packages, one can use [Homebrew](https://brew.sh):

```sh
brew install cmake python cfitsio gsl fftw
brew install gcc # only if GCC is desired
```

Configure an out-of-source build with Apple Clang:

```sh
CC="$(xcrun -find cc)" CXX="$(xcrun -find c++)" \
cmake -S . -B build \
    -DPython3_EXECUTABLE="$(brew --prefix python)/bin/python3" \
    -DENABLE_TESTING=ON \
    -DDOWNLOAD_DATA=ON
```

Then build, test, and install:

```sh
cmake --build build --parallel
ctest --test-dir build --output-on-failure
cmake --install build
```

## Use with Docker/Podman image

The simplest, yet the most inflexible method of installation and usage
is with [Docker](https://www.docker.com) or  [Podman](https://podman.io).
One has to install Docker or Podman first on their operating
system (GNU/Linux, macOS, Windows are supported).

Running already prepared container is straightforward:

```sh
docker run -it --rm -p 8888:8888 quay.io/cosmicrays/jupyter-hermes:latest
```

Jupyter can be accessed through a web browser following a link
given in the output of the previous command (such as `http://127.0.0.1:8888/lab?token=b680...`).

To build an image based on [Jupyter Docker Stacks](https://jupyter-docker-stacks.readthedocs.io/en/latest/index.html)
with HERMES installed and enabled,one has to run inside
the locally cloned HERMES repository:

```sh
docker build -t jupyter-pyhermes .
```

Finally, the Jupyter notebook server is started within a new container

```sh
docker run --name jupyter -it --rm -p 8888:8888 \
    -v $HOME/jupyter-work:/home/jovyan/work localhost/jupyter-pyhermes:latest
```

where `$HOME/jupyter-work` is any directory on the host in which notebooks
and files can be stored permanently, after the container is stopped and removed.
It is linked to the `work` folder within the container.
