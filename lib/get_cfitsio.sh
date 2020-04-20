#!/bin/sh

curl -o cfitsio.tar.gz  https://heasarc.gsfc.nasa.gov/FTP/software/fitsio/c/cfitsio_latest.tar.gz
tar xvf cfitsio.tar.gz
rm -rf cfitsio.tar.gz
rm -rf cfitsio/docs/
