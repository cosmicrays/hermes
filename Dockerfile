# Copyright (c) HERMES Development Team.
# Distributed under the terms of the GPLv3.
ARG REGISTRY=quay.io
ARG OWNER=jupyter
ARG BASE_CONTAINER=$REGISTRY/$OWNER/scipy-notebook
FROM $BASE_CONTAINER

LABEL maintainer="HERMES Developers <hermes-developers@icpn.hr>"

USER root

RUN apt-get update --yes && \
    apt-get install --yes --no-install-recommends \
    build-essential \
    cmake \
    doxygen \
    libgsl-dev \
    libfftw3-dev \
    libcfitsio-dev && \
    apt-get clean && rm -rf /var/lib/apt/lists/*

USER ${NB_UID}

# Install Python 3 packages
RUN mamba install --yes \
    'conda-forge::sphinx' \
    'healpy' && \
    mamba clean --all -f -y && \
    fix-permissions "${CONDA_DIR}" && \
    fix-permissions "/home/${NB_USER}"

# Install facets package which does not have a `pip` or `conda-forge` package at the moment
WORKDIR /tmp
RUN git clone https://github.com/cosmicrays/hermes.git
WORKDIR /tmp/hermes
ENV PATH /opt/conda/bin:$PATH
RUN /bin/bash -c "source activate base"
RUN mkdir build && \
    cd build && \
    CMAKE_PREFIX_PATH=${CONDA_DIR} cmake \
         -DPython3_EXECUTABLE=${CONDA_DIR}/bin/python${PYTHON_VERSION} \
         -DPython3_INCLUDE_DIRS=${CONDA_DIR}/include/python${PYTHON_VERSION} \
         -DPython3_LIBRARIES=${CONDA_DIR}/lib/python${PYTHON_VERSION} \
         -DENABLE_PYTHON=On \
         -DCMAKE_INSTALL_PREFIX=${CONDA_DIR} \
         -DENABLE_TESTING=On .. && \
    make -j && \
    make install && \
    rm -rf /tmp/hermes && \
    fix-permissions "${CONDA_DIR}" && \
    fix-permissions "/home/${NB_USER}"

USER ${NB_UID}

WORKDIR "${HOME}"
