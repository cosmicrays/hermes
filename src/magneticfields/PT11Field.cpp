#include "hermes/magneticfields/PT11Field.h"

#include <algorithm>

#include "hermes/Units.h"

namespace hermes { namespace magneticfields {

PT11Field::PT11Field() : useASS(true), useBSS(false), useHalo(true) {
	// disk parameters
	d = -0.6_kpc;
	R_sun = 8.5_kpc;
	R_c = 5.0_kpc;
	z0_D = 1.0_kpc;
	B0_D = 2.0_muG;

	// halo parameters
	z0_H = 1.3_kpc;
	R0_H = 8.0_kpc;
	B0_Hn = 4.0_muG;
	B0_Hs = 4.0_muG;
	z11_H = 0.25_kpc;
	z12_H = 0.4_kpc;

	// set ASS specific parameters
	setUseASS(true);
}

void PT11Field::SetParams() {
	cos_pitch = cos(pitch);
	sin_pitch = sin(pitch);
	PHI = cos_pitch / sin_pitch * log(QNumber(1) + d / R_sun) * 1_rad -
	      QAngle(pi / 2);
	cos_PHI = cos(PHI);
}

void PT11Field::setUseASS(bool use) {
	useASS = use;
	if (not(use)) return;

	if (useBSS) {
		std::cout << "PT11Field: Disk field changed to ASS" << std::endl;
		useBSS = false;
	}

	pitch = -5.0 * pi / 180;
	B0_Hs = 2.0_muG;
	SetParams();
}

void PT11Field::setUseBSS(bool use) {
	useBSS = use;
	if (not(use)) return;

	if (useASS) {
		std::cout << "PT11Field: Disk field changed to BSS" << std::endl;
		useASS = false;
	}

	pitch = -6.0 * pi / 180;
	B0_Hs = 4.0_muG;
	SetParams();
}

void PT11Field::setUseHalo(bool use) { useHalo = use; }

bool PT11Field::isUsingASS() { return useASS; }

bool PT11Field::isUsingBSS() { return useBSS; }

bool PT11Field::isUsingHalo() { return useHalo; }

Vector3QMField PT11Field::getField(const Vector3QLength &pos_) const {
	Vector3QLength pos = pos_;
	// pos.set(-pos_.getY());

	QLength r = sqrt(pos.x * pos.x + pos.y * pos.y);  // in-plane radius

	Vector3d b(0.);

	// disk field
	if ((useASS) or (useBSS)) {
		// PT11 paper has B_theta = B * cos(p) but this seems because
		// they define azimuth clockwise, while we have anticlockwise.
		// see Tinyakov 2002 APh 18,165: "local field points to l=90+p"
		// so p=-5 deg gives l=85 and hence clockwise from above. so to
		// get local B clockwise in our system, need minus (like Sun
		// etal). Ps base their system on Han and Qiao 1994 A&A 288,759
		// which has a diagram with azimuth clockwise, hence confirmed.

		// PT11 paper define Earth position at (+8.5, 0, 0) kpc; but
		// usual convention is (-8.5, 0, 0) thus we have to rotate our
		// position by 180 degree in azimuth
		QAngle theta =
		    QAngle(pi) - pos.getPhi();  // azimuth angle theta: PT11 paper uses
		                                // opposite convention for azimuth
		// the following is equivalent to sin(pi - phi) and cos(pi -
		// phi) which is computationally slower orig: QNumber cos_theta
		// = - pos.x / r;
		QNumber cos_theta = pos.x / r;
		QNumber sin_theta = pos.y / r;

		// After some geometry calculations (on whiteboard) one finds:
		// Bx = +cos(theta) * B_r - sin(theta) * B_{theta}
		// By = -sin(theta) * B_r - cos(theta) * B_{theta}
		// Use from paper: B_theta = B * cos(pitch)	and B_r = B *
		// sin(pitch)
		b.x =
		    static_cast<double>(sin_pitch * cos_theta - cos_pitch * sin_theta);
		b.y =
		    -static_cast<double>(sin_pitch * sin_theta - cos_pitch * cos_theta);
		b *= -1;  // flip magnetic field direction, as B_{theta} and
		          // B_{phi} refering to 180 degree rotated field

		double bMag = static_cast<double>(
		    cos(theta - 1_rad * cos_pitch / sin_pitch * log(r / R_sun) + PHI));
		if (useASS) bMag = fabs(bMag);
		bMag *= static_cast<double>(B0_D * R_sun / std::max(r, R_c) / cos_PHI *
		                            exp(-fabs(pos.z) / z0_D));
		b *= bMag;
	}

	// halo field
	if (useHalo) {
		double bMag = static_cast<double>(pos.z > 0_m ? B0_Hn : -B0_Hs);
		QLength z1 = static_cast<double>(fabs(pos.z) < z0_H ? z11_H : z12_H);
		bMag *= static_cast<double>(
		    (r / R0_H) * exp(QNumber(1) - r / R0_H) /
		    (QNumber(1) + pow<2>((fabs(pos.z) - z0_H) / z1)));
		// equation (8) in paper: theta uses now the conventional
		// azimuth definition in contrast to equation (3) cos(phi) =
		// pos.x / r (phi going counter-clockwise) sin(phi) = pos.y / r
		// unitvector of phi in polar coordinates: (-sin(phi), cos(phi),
		// 0) b += bMag * Vector3d(-static_cast<double>(pos.y / r),
		// static_cast<double>(pos.x / r), 0);
		b += bMag * Vector3d(-static_cast<double>(pos.y / r),
		                     static_cast<double>(-pos.x / r), 0);
	}

	return Vector3QMField(b);
}

}}  // namespace hermes::magneticfields
