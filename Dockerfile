# Copyright (c) HERMES Development Team.
# Distributed under the terms of the GPLv3.
ARG REGISTRY=quay.io
ARG BUILD_CONTAINER=$REGISTRY/jupyter/docker-stacks-foundation
ARG RUNNER_CONTAINER=$REGISTRY/jupyter/scipy-notebook
FROM $BUILD_CONTAINER AS builder

LABEL maintainer="HERMES Developers <hermes-developers@icpn.hr>"

USER root

RUN apt-get update --yes && \
    apt-get install --yes --no-install-recommends \
    build-essential \
    pkg-config \
    cmake \
    git \
    libgsl-dev \
    libfftw3-dev \
    libcfitsio-dev && \
    apt-get clean && rm -rf /var/lib/apt/lists/*

USER ${NB_UID}

# Install Python packages
RUN mamba install --yes \
    'conda-forge::zlib' && \
    mamba clean --all -f -y && \
    fix-permissions "${CONDA_DIR}" && \
    fix-permissions "/home/${NB_USER}"

ENV PATH=/opt/conda/bin:$PATH
RUN /bin/bash -c "source activate base" && \
    mkdir -p /tmp/hermes
WORKDIR /tmp/hermes
COPY . .
RUN CMAKE_PREFIX_PATH=${CONDA_DIR} cmake -S . -B build-container \
         -DPython3_EXECUTABLE=${CONDA_DIR}/bin/python${PYTHON_VERSION} \
         -DPython3_INCLUDE_DIRS=${CONDA_DIR}/include/python${PYTHON_VERSION} \
         -DPython3_LIBRARIES=${CONDA_DIR}/lib/python${PYTHON_VERSION} \
         -DENABLE_PYTHON=On \
         -DCMAKE_INSTALL_PREFIX=${CONDA_DIR} \
         -DENABLE_TESTING=On \
         -DDOWNLOAD_DATA=On && \
    cmake --build build-container --parallel && \
    cmake --install build-container && \
    git clone --depth 1 --branch master https://github.com/HERMES-SkyMaps/hermes-examples.git build-container/hermes-examples && \
    fix-permissions "${CONDA_DIR}" && \
    fix-permissions "/home/${NB_USER}"

FROM $RUNNER_CONTAINER AS runner

USER root

RUN apt-get update --yes && \
    apt-get install --yes --no-install-recommends \
    libgsl27 \
    libfftw3-single3 libfftw3-double3 \
    libcfitsio10t64 && \
    apt-get clean && rm -rf /var/lib/apt/lists/*

COPY --from=builder ${CONDA_DIR} ${CONDA_DIR}
COPY --from=builder /tmp/hermes/build-container/hermes-examples/jupyter /home/${NB_USER}/hermes-examples 

# Install Python packages
RUN mamba install --yes \
    'conda-forge::astropy' \
    'conda-forge::healpy' && \
    mamba clean --all -f -y && \
    fix-permissions "${CONDA_DIR}" && \
    fix-permissions "/home/${NB_USER}"

USER ${NB_UID}

WORKDIR "${HOME}"
