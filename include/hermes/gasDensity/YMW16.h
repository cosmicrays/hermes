#ifndef HERMES_YMW16_H
#define HERMES_YMW16_H

#include "hermes/gasDensity/GasDensity.h"

extern "C" {
	#include "cn.h"
}

namespace hermes {

class YMW16: public GasDensity {
private:
	QPDensity fne1, fne2;
	QLength	  H1, A1, H2, A2, R2;
public:
	YMW16() { };
	QPDensity getDensity(const Vector3QLength& pos) const {
		auto conversion = [](QLength x) { return static_cast<double>(x.convertTo(parsec)); };
		double x = -1*conversion(pos.getY()); // change coordinates X<>Z since in YMW16
		double y = conversion(pos.getX()); // the sun position is at (0,+8.3_kpc,6.0_pc)
		double z = conversion(pos.getZ());
		double gl, gb, dd;
		int ncrd = 1; // x, y, z are input coordinates
		int vbs = 0; // verbose
		char dirname[] = "/home/andy/Projects/category_science/hermes/libs/ymw16/";
		char text[10];

		return QPDensity(ne_crd(&x, &y, &z, &gl, &gb, &dd, ncrd, vbs, dirname, text))/1_cm3*1_m3;
	};
};

} // namespace hermes

#endif // HERMES_YMW16_H
