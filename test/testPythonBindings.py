#!/usr/bin/env python3
"""Exercise pyhermes object lifetimes and the skymap buffer protocol."""

import gc

import pyhermes


def main() -> None:
    nside = 16
    energy_steps = 16
    expected_pixels = 12 * nside * nside

    for iteration in range(50):
        spectrum = pyhermes.GammaSkymapRange(
            nside,
            pyhermes.units.TeV,
            1000 * pyhermes.units.TeV,
            energy_steps,
        )
        assert len(spectrum) == energy_steps
        assert len(spectrum.getEnergies()) == energy_steps

        for skymap in spectrum:
            view = memoryview(skymap)
            assert view.ndim == 1
            assert view.shape == (expected_pixels,)
            assert view.itemsize == 8

        copied_map = spectrum[0]
        assert memoryview(copied_map).shape == (expected_pixels,)

        del copied_map, skymap, spectrum, view
        if iteration % 10 == 0:
            gc.collect()


if __name__ == "__main__":
    main()
