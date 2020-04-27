# HERMES - High-Energy Radiative MESsangers

[![Build Status](https://travis-ci.org/cosmicrays/hermes.svg?branch=master)](https://travis-ci.org/cosmicrays/hermes)
[![Coverage Status](https://coveralls.io/repos/github/cosmicrays/hermes/badge.svg?branch=master)](https://coveralls.io/github/cosmicrays/hermes?branch=master)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/36b17ce072d6430aad28e457a5fa76d3)](https://app.codacy.com/gh/cosmicrays/hermes?utm_source=github.com&utm_medium=referral&utm_content=cosmicrays/hermes&utm_campaign=Badge_Grade_Dashboard)
![GitHub](https://img.shields.io/github/license/cosmicrays/hermes)

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

Quick procedure for those who know their way around:
```sh
mkdir build
cd build
cmake ..
make -j
```

For detailed installation guides and requirements see [INSTALL](INSTALL.md).

## Usage

```python
from pyhermes import *
from pyhermes.units import TeV, deg, kpc, pc

nside = 512
Egamma = 0.1*TeV
sun_pos = Vector3QLength(8.0*kpc, 0*pc, 0*pc)

skymap = GammaSkymap(nside, Egamma)
mask = RectangularWindow([5*deg, 40*deg], [-5*deg, 90*deg]);
skymap.setMask(mask)

neutral_gas = neutralgas.RingModel(neutralgas.RingType.HI)
cosmicray_protons = cosmicrays.Dragon2D(Proton)
pp_crosssection = interactions.Kamae06Gamma()

integrator = PiZeroIntegrator(cosmicray_protons, neutral_gas, pp_crosssection)
integrator.setSunPosition(sun_pos)
integrator.setupCacheTable(100, 100, 20)

skymap.setIntegrator(integrator)
skymap.compute()

output = outputs.HEALPixFormat("!pizero-dragon2d.fits.gz")
skymap.save(output)
```

More examples can be found in [the examples repository](https://github.com/cosmicrays/hermes-examples). Doxygen documentation generated from the source code is available [here](https://heat.gssi.it/hermes/doc/).

## Credits

| Name           | Institution                                         |
|----------------|-----------------------------------------------------|
|Andrej Dundovic | Gran Sasso Science Institute, L'Aquila, Italy       |
|Carmelo Evoli   | Gran Sasso Science Institute, L'Aquila, Italy       |
|Daniele Gaggero | Instituto de Física Teórica UAM/CSIC, Madrid, Spain |
