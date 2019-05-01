#include "hermes/chargedGasDensity/YMW16.h"

namespace hermes {

YMW16::YMW16() {
	setTemperature(1e4_K);
}

QPDensity YMW16::getDensity(const Vector3QLength& pos) const {
	auto conversion = [](QLength x) { return static_cast<double>(x.convertTo(parsec)); };

	double x = -1*conversion(pos.getY()); // change coordinates X<>Z since in YMW16
	double y = conversion(pos.getX()); // the sun position is at (0,+8.3_kpc,6.0_pc)
	double z = conversion(pos.getZ());
	double gl, gb, dd; // dummy vars
	int ncrd = 1; // x, y, z are input coordinates
	int vbs = 0; // verbose
	char dirname[] = "empty"; // not important since we put YMW16 parameters inside its source
	char text[10]; // dummy var

	return QPDensity(ne_crd(&x, &y, &z, &gl, &gb, &dd, ncrd, vbs, dirname, text))/1_cm3*1_m3;
};

} // namespace hermes
