# HERMES - High-Energy Radiative MESsangers

[![Build Status](https://travis-ci.org/cosmicrays/hermes.svg?branch=master)](https://travis-ci.org/cosmicrays/hermes)
[![Coverage Status](https://coveralls.io/repos/github/cosmicrays/hermes/badge.svg?branch=master)](https://coveralls.io/github/cosmicrays/hermes?branch=master)

## About

![gamma sky map from pi0](https://github.com/cosmicrays/hermes/raw/master/doc/hermes-pizero-example.png)

**HERMES** is a publicly available computational framework for the line of sight integration which creates sky maps in the [HEALPix](https://healpix.jpl.nasa.gov/)-compatibile format of various galactic radiative processes including Faraday rotation, synchrotron and free-free radio emission, gamma-ray emission from pion-decay, bremsstrahlung and inverse-Compton. The name is an acronym for "High-Energy Radiative MESsangers".

The code is written in C++ relying on features of the language's recent revisions (C++11, C++14). Once compiled, HERMES can optionally be used from Python thanks to [pybind11](https://github.com/pybind/pybind11) (Python 2.X is not supported). Some components of the code (such as galactic magnetic field models, vector and grid classes) were adopted from [CRPropa 3](https://crpropa.desy.de/), a code for cosmic ray propagation.

HERMES provides the following integrators:
- Dispersion measure
- Rotation measure
- Free-Free emission
- Synchrotron emission (with absorption)
- Pion decay gamma-ray emission
- Inverse Compton
- Bremsstrahlung

## Install

Experienced users:
```sh
mkdir build
cd build
cmake ..
make -j
```

Installation with [python virtualenv](https://virtualenv.pypa.io) is recommended:
```sh
export HERMES_DIR=$HOME"/.virtualenvs/hermes"
virtualenv -p python3 $HERMES_DIR
source $HERMES_DIR/bin/activate
```
Once the environment is activated, clone the repository and build it with cmake:
```sh
git clone https://github.com/cosmicrays/hermes.git
cd hermes
mkdir build
cd build
CMAKE_PREFIX_PATH=$HERMES_DIR cmake -DCMAKE_INSTALL_PREFIX=$HERMES_DIR ..
make -j
make install
```

## Usage

```python
from pyhermes import *
from pyhermes.units import TeV, deg

nside = 512
Egamma = 0.1*TeV

skymap = GammaSkymap(nside=nside, Egamma=Egamma)
mask = RectangularWindow([5*deg, 40*deg], [-5*deg, 90*deg]);
skymap.setMask(mask)

neutral_gas = RingModelDensity()
cosmicray_protons = Dragon2DCRDensity([Proton])
pp_crosssection = Kamae06()

integrator = PiZeroIntegrator(cosmicray_protons, neutral_gas, pp_crosssection)
integrator.setupCacheTable(100, 100, 20)

skymap.setIntegrator(integrator)
skymap.compute()

output = FITSOutput("!pizero-dragon2d.fits.gz")
skymap.save(output)
```

## Credits

Name | Institution
-----|--------------
Andrej Dundovic |  Gran Sasso Science Institute, L'Aquila, Italy
Carmelo Evoli | Gran Sasso Science Institute, L'Aquila, Italy
Daniele Gaggero | Instituto de Física Teórica UAM/CSIC, Madrid, Spain
