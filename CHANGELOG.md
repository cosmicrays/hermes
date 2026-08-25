## Unreleased

### Changed

- Modernized CMake configuration, installation, exported packages, Python runtime paths, and CPack archives.
- Made runtime-data downloads explicit, verified, and offline by default.
- Moved the pinned runtime-data download to the versioned `hermes-data` GitHub
  release and upgraded archive verification from MD5 to SHA-256.
- Kept unmaintained HDF5/PICARD support opt-in and excluded it from default packages.
- Removed obsolete Travis, Coveralls, and Read the Docs service configuration; GitHub Actions now covers builds, tests, installation, and packaging.
- Moved the canonical user and developer guide to the standalone Markdown-based
  `hermes-docs` site and retired the obsolete in-tree Sphinx/Doxygen build.
- Replaced the 720-file Vernetto16 radiation-field layout with one compressed,
  self-describing FITS cube while retaining compatibility with legacy data.

### Fixed

- Corrected numerical edge cases, data races, path handling, and coordinate calculations.
- Added scientific regression tests and broader unit-test coverage.
- Corrected ISRF spatial interpolation and made outer grid-boundary evaluation safe.

## HERMES v1.1.0 — 2026-06-05

### New features

- UHECR density and secondary-particle yield models.
- Gamma-ray absorption, neutron-decay pion emission, and HDMSpectra dark-matter spectra.
- AAfrag interaction tables and updated DRAGON2 models.
- Experimental PICARD/HDF5 reader support.

### Changed

- Adopted C++17 and updated the Python, container, and CI toolchains.
- Improved dark-matter and absorption performance.

### Fixed

- Corrected gas-ring boundaries, runtime-data paths, UHECR limits, and scientific tests.

## HERMES v1.0.1

### New features

- Variable X_CO model

### Other

- Continuous integration migrated from Travis CI to GitHub Actions.

## HERMES v1.0.0

The first official release of HERMES used in arXiv:2105.13165 and A&A 653, A18 (2021).
