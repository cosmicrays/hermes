# hermes

HERMES is a publicly available computational framework for generating sky maps of various galactic radiative processes including Faraday rotation, synchrotron and free-free radio emission, gamma-ray emission from pion-decay, bremsstrahlung and inverse-Compton. 


## Install

Experienced users:
```sh
mkdir build
cd build
cmake ..
make -j
```

Recommended installation in a python virtual environment:
``sh
export HERMES_DIR=$HOME"/.virtualenvs/hermes"
cd $HERMES_DIR
git clone https://github.com/adundovi/hermes.git
cd CRPropa3
mkdir build
cd build
CMAKE_PREFIX_PATH=$HERMES_DIR cmake -DCMAKE_INSTALL_PREFIX=$HERMES_DIR ..
make -j
make install
``

## Credits

name | institution
-----|--------------
Andrej Dundovic |  Gran Sasso Science Institute, L'Aquila, Italy
Carmelo Evoli | Gran Sasso Science Institute, L'Aquila, Italy
Daniele Gaggero | Instituto de Física Teórica UAM/CSIC, Madrid, Spain
