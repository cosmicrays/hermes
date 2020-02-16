# HERMES - High-Energy Radiative MESsangers

## About

![gamma sky map from pi0](https://github.com/adundovi/hermes/raw/master/doc/hermes-pizero-example.png)

**HERMES** is a publicly available computational framework for the line of sight integration which creates sky maps in the [HEALPix](https://healpix.jpl.nasa.gov/)-compatibile format of various galactic radiative processes including Faraday rotation, synchrotron and free-free radio emission, gamma-ray emission from pion-decay, bremsstrahlung and inverse-Compton. The name is an acronym for "High-Energy Radiative MESsangers".

The code is written in C++ relying on features of the language's recent revisions (C++11, C++14). Once compiled, HERMES can optionally be used from Python thanks to [pybind11](https://github.com/pybind/pybind11). Some components of the code (such as magnetic field models, vector and grid classes) were adopted from [CRPropa 3](https://crpropa.desy.de/), a code for cosmic ray propagation.

## Install

Experienced users:
```sh
mkdir build
cd build
cmake ..
make -j
```

Recommended installation in a python virtual environment:
```sh
export HERMES_DIR=$HOME"/.virtualenvs/hermes"
virtualenv -p python3 $HERMES_DIR
source $HERMES_DIR/bin/activate
cd $HERMES_DIR
git clone https://github.com/adundovi/hermes.git
cd CRPropa3
mkdir build
cd build
CMAKE_PREFIX_PATH=$HERMES_DIR cmake -DCMAKE_INSTALL_PREFIX=$HERMES_DIR ..
make -j
make install
```

## Usage

```python
from pyhermes import *

nside = 512
Egamma = 0.1*TeV

skymap = GammaSkymap(nside=nside, Egamma=Egamma)
mask = RectangularWindow(-5*deg, 5*deg, 40*deg, 90*deg);
skymap.setMask(mask)

neutral_gas = RingModelDensity()
cosmicray_protons = Dragon2DCRDensity([Proton])
pp_crosssection = Kamae06()

integrator = PiZeroIntegrator(cosmicray_protons, neutral_gas, pp_crosssection)
integrator.initCacheTable(Egamma, 100, 100, 20)

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
